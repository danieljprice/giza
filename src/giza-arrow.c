/* giza - a scientific plotting layer built on cairo
 *
 * This file is (or was) part of GIZA, a scientific plotting
 * layer built on cairo.
 * GIZA comes with ABSOLUTELY NO WARRANTY.
 * This is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * Copyright (C) 2010 James Wetter. All rights reserved.
 * Contact: wetter.j@gmail.com
 *
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
 *  -x2 :- The world x-coord of the tail of the arrow.
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
  cairo_user_to_device (context, &x1, &y1);
  cairo_user_to_device (context, &x2, &y2);

  /* Set the current transformation to the idenity */
  _giza_set_trans (GIZA_TRANS_IDEN);

  double xpts[4], ypts[4], dx, dy, chx, chy, dxUnit, dyUnit, magnitude;
  double dxPerp, dyPerp;

  /* set dx and dy to the change in the x and y direction respectivly */
  dx = x2 - x1;
  dy = y2 - y1;

  /* Get the character size in device coords and scale a little */
  giza_get_character_size (GIZA_UNITS_DEVICE, &chx, &chy);
  chx = 0.5 * chx;

  if (chx > 0)
    {
      if (dx != 0 || dy != 0)
	{
	  /* find a unit vector in the direction of the arrow */
	  magnitude = sqrt (dx * dx + dy * dy);
	  dxUnit = dx / magnitude;
	  dyUnit = dy / magnitude;

	  /* find a prependicular unit vector */
	  if (dxUnit == 0)
	    {
	      dxPerp = 1;
	      dyPerp = 0;
	    }
	  else if (dyUnit == 0)
	    {
	      dxPerp = 0;
	      dyPerp = 1;
	    }
	  else
	    {
	      dxPerp = 1;
	      dyPerp = -dxUnit / dyUnit;
	      magnitude = dxPerp * dxPerp + dyPerp * dyPerp;
	      dxPerp = dxPerp / magnitude;
	      dyPerp = dyPerp / magnitude;
	    }

	  /* Calculate where the verticies of the arrow head are */
	  /* The point */
	  xpts[0] = x2;
	  ypts[0] = y2;
	  /* 'above' the unit vector */
	  xpts[1] = x2 - (dxUnit + dxPerp * tan (Arrow.angle * GIZA_DEG_TO_RAD)) * chx;
	  ypts[1] = y2 - (dyUnit + dyPerp * tan (Arrow.angle * GIZA_DEG_TO_RAD)) * chx;
	  /* on the unit vector, the cutback */
	  xpts[2] = x2 - (1 - Arrow.cutback) * chx * dxUnit;
	  ypts[2] = y2 - (1 - Arrow.cutback) * chx * dyUnit;
	  /* 'below' the unit vector */
	  xpts[3] = x2 - (dxUnit - dxPerp * tan (Arrow.angle * GIZA_DEG_TO_RAD)) * chx;
	  ypts[3] = y2 - (dyUnit - dyPerp * tan (Arrow.angle * GIZA_DEG_TO_RAD)) * chx;

	  /* draw the head */
	  cairo_move_to (context, xpts[0], ypts[0]);
	  cairo_line_to (context, xpts[1], ypts[1]);
	  cairo_line_to (context, xpts[2], ypts[2]);
	  cairo_line_to (context, xpts[3], ypts[3]);
	  cairo_line_to (context, xpts[0], ypts[0]);

	  /* fill it in! */
	  double oldMiter = cairo_get_miter_limit (context);
	  cairo_set_miter_limit (context, 0.);
	  giza_set_fill (Arrow.fs);
	  cairo_stroke_preserve (context);
	  _giza_fill ();
	  cairo_set_miter_limit (context, oldMiter);  

	  /* draw the tail */
	  cairo_move_to (context, x1, y1);
	  cairo_line_to (context, xpts[2], ypts[2]);
	  
	  _giza_stroke ();
	}
    }
  _giza_set_trans (oldTrans);

  if (!Sets.buf)
    {
      giza_flush_device ();
    }
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
