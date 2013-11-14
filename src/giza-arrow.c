/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the GPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

#include "giza-private.h"
#include "giza-fill-private.h"
#include "giza-stroke-private.h"
#include "giza-arrow-style-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <math.h>

/**
 * Drawing: giza_arrow
 *
 * Synopsis: Draws an arrow. The style of the head of the arrow is set
 * by giza_set_arrow_style.
 *
 * Input:
 *  -x1 :- The world x-coord of the tail of the arrow.
 *  -y1 :- The world y-coord of the tail of the arrow.
 *  -x2 :- The world x-coord of the head of the arrow.
 *  -y2 :- The world y-coord of the head of the arrow.
 *
 * See Also: giza_set_arrow_style
 */
void
giza_arrow (double x1, double y1, double x2, double y2)
{
  if (!_giza_check_device_ready ("giza_arrow"))
    return;

  /* Set the current transformation to world coords */
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  /* convert the points to device coords */
  cairo_user_to_device (Dev[id].context, &x1, &y1);
  cairo_user_to_device (Dev[id].context, &x2, &y2);

  /* Set the current transformation to the idenity */
  _giza_set_trans (GIZA_TRANS_IDEN);

  double xpts[4], ypts[4], dx, dy, chx, chy, dxUnit, dyUnit, magnitude;
  double dxPerp, dyPerp, sinangle, cosangle;
  giza_arrow_t Arrow = Dev[id].Arrow;

  /* set dx and dy to the change in the x and y direction respectively */
  dx = x2 - x1;
  dy = y2 - y1;

  /* Get the character size in device coords and scale a little */
  giza_get_character_size (GIZA_UNITS_DEVICE, &chx, &chy);
  chx = 1.0 * chx;
  chy = 1.0 * chy;

  if (chx > 0)
    {
      if (!(_giza_equal(dx,0.) && _giza_equal(dy,0.)))
	{
	  /* find a unit vector in the direction of the arrow */
	  magnitude = sqrt (dx * dx + dy * dy);
	  dxUnit = dx / magnitude;
	  dyUnit = dy / magnitude;

	  /* find a perpendicular unit vector */
	  if (_giza_equal(dxUnit,0.))
	    {
	      dxPerp = 1.;
	      dyPerp = 0.;
	    }
	  else if (_giza_equal(dyUnit,0.))
	    {
	      dxPerp = 0.;
	      dyPerp = 1.;
	    }
	  else
	    {
	      dxPerp = 1.;
	      dyPerp = -dxUnit / dyUnit;
	      magnitude = sqrt(dxPerp * dxPerp + dyPerp * dyPerp);
	      dxPerp = dxPerp / magnitude;
	      dyPerp = dyPerp / magnitude;
	    }
	  
	  if (magnitude > 0)
	    {
	      dx = dx/magnitude;
	      dy = dy/magnitude;
	    }
	  else
	    {
	      dx = 1.;
	      dy = 0.;
	      magnitude = 1.0;
	    }

	  /* Calculate where the vertices of the arrow head are */
	  /* The point */
	  xpts[0] = x2;
	  ypts[0] = y2;

          /* 'above' the unit vector 
           * Note that we want the hypoteneuse of the arrow head
           * to always have same length regardless of the angle:
           *      |\
           *    b | \ r
           *      |  \ 
           * -----|   \
           * hence we use sin(angle) = b/r, with r=1 to get b.
           */
          sinangle = sin (0.5 * Arrow.angle * GIZA_DEG_TO_RAD);
          cosangle = cos (0.5 * Arrow.angle * GIZA_DEG_TO_RAD);
          xpts[1] = x2 - (dxUnit* cosangle + dxPerp * sinangle) * chx;
	  ypts[1] = y2 - (dyUnit* cosangle + dyPerp * sinangle) * chy;

          /* on the unit vector, the cutback */
	  xpts[2] = x2 - (1. - Arrow.cutback) * chx * dxUnit * cosangle;
	  ypts[2] = y2 - (1. - Arrow.cutback) * chy * dyUnit * cosangle;

	  /* 'below' the unit vector */
	  xpts[3] = x2 - (dxUnit* cosangle - dxPerp * sinangle) * chx ;
	  ypts[3] = y2 - (dyUnit* cosangle - dyPerp * sinangle) * chy;

	  /* draw the head */
	  cairo_move_to (Dev[id].context, xpts[0], ypts[0]);
	  cairo_line_to (Dev[id].context, xpts[1], ypts[1]);
	  cairo_line_to (Dev[id].context, xpts[2], ypts[2]);
	  cairo_line_to (Dev[id].context, xpts[3], ypts[3]);
	  cairo_line_to (Dev[id].context, xpts[0], ypts[0]);

	  /* fill it in! */
	  double oldMiter = cairo_get_miter_limit (Dev[id].context);
	  cairo_set_miter_limit (Dev[id].context, 0.);
	  giza_set_fill (Arrow.fs);
	  cairo_stroke_preserve (Dev[id].context);
	  _giza_fill ();
	  cairo_set_miter_limit (Dev[id].context, oldMiter);

	  /* draw the tail */
	  cairo_move_to (Dev[id].context, x1, y1);
	  cairo_line_to (Dev[id].context, xpts[2], ypts[2]);

	  _giza_stroke ();
	}
    }
  _giza_set_trans (oldTrans);

  giza_flush_device ();
}

/**
 * Drawing: giza_arrow_float
 *
 * Synopsis: Same functionality as giza_arrow.
 *
 * See Also: giza_arrow_float
 */
void
giza_arrow_float (float x1, float y1, float x2, float y2)
{
  giza_arrow ((double) x1, (double) y1, (double) x2, (double) y2);
}
