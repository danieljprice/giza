/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2022 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING.LESSER file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2 of the LGPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the LGPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

#include "giza-private.h"
#include "giza-transforms-private.h"
#include "giza-stroke-private.h"
#include "giza-window-private.h"
#include "giza-viewport-private.h"
#include "giza-drivers-private.h"
#include "giza-tick-private.h"
#include <giza.h>
#include <math.h>

/**
 * Drawing: giza_tick
 *
 * Synopsis: Draw a single tick along an axis. The axis extends from
 *           (x1,y1) to (x2,y2) and the tick is drawn perpendicular to the axis
 *           which is not drawn by this routine. Optional text label drawn
 *           parallel to the axis if the orientation angle is zero
 *
 * Input:
 *  -x1   :- starting x position in world coordinates
 *  -y1   :- starting y position in world coordinates
 *  -x2   :- finishing x position in world coordinates
 *  -y2   :- finishing y position in world coordinates
 *  -v    :- axis value at tick location
 *  -tickl :- Length of tick mark drawn to "left/bottom" of axis
 *             in units of character height
 *  -tickr :- Length of tick mark drawn to "right/top" of axis
 *             in units of character height
 *  -disp  :- Displacement of labels from axis
 *             in units of character height
 *  -angle :- Label orientation; angle between text and direction of axis; in degrees
 *  -label :- Text string used for label (can be blank)
 *
 */
void
giza_tick (double x1, double y1, double x2, double y2, double v,
           double tickl, double tickr, double disp, double angle, const char *label)
{
  if (!_giza_check_device_ready ("giza_tick"))
    return;

  _giza_expand_clipping ();

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  /* This routine follows PGPLOT's pgtick.f closely so that tick sides,
   * label position, justification and rotation agree with PGPLOT in
   * every window orientation (including mirrored x or y axes). */

  /* axis direction in device coords (device y runs down) */
  double ddx = x2 - x1, ddy = y2 - y1;
  cairo_user_to_device_distance (Dev[id].context, &ddx, &ddy);
  double dlen = sqrt (ddx*ddx + ddy*ddy);
  if (_giza_equal (dlen, 0.))
    {
      _giza_set_trans (oldTrans);
      return;
    }
  double theta_screen_deg = -atan2 (ddy, ddx) / GIZA_DEG_TO_RAD;

  /* (tikx,tiky): world-coordinate displacement of one character height
   * perpendicular to the axis, pointing to the screen-left of the
   * direction of travel (x1,y1) -> (x2,y2), as in pgtick.f.
   * PGPLOT's "character height" unit here is PGQCS/PGYSP = the line
   * spacing, which pgsch.f defines geometrically as ch x height/40 of
   * the view surface - NOT a font metric (cairo font extents vary by
   * device, PGPLOT's unit must not) */
  double ych = Dev[id].ch * Dev[id].panelheight / 40.;
  double tikx = ddy/dlen * ych;
  double tiky = -ddx/dlen * ych;
  cairo_device_to_user_distance (Dev[id].context, &tikx, &tiky);

  /* draw the tick mark at fraction v along the axis */
  double x = x1 + v*(x2 - x1);
  double y = y1 + v*(y2 - y1);
  if (!(_giza_equal(tickl,0.) && _giza_equal(tickr,0.)))
    {
      cairo_move_to (Dev[id].context, x - tickr*tikx, y - tickr*tiky);
      cairo_line_to (Dev[id].context, x + tickl*tikx, y + tickl*tiky);
    }

  /* write the label, with PGPLOT's justification and displacement
   * rules for each quadrant of the orientation angle */
  if (label && label[0] != '\0')
    {
      double d = disp;
      double lang = theta_screen_deg;
      double just;
      double or = fmod (angle, 360.);
      if (or < 0.) or += 360.;
      if (or > 45. && or <= 135.)
        {
          just = (d < 0.) ? 1. : 0.;
        }
      else if (or > 135. && or <= 225.)
        {
          just = 0.5;
          if (d < 0.) d = d - 1.;
        }
      else if (or > 225. && or <= 315.)
        {
          lang = lang + 90.;
          just = (d < 0.) ? 0. : 1.;
        }
      else
        {
          just = 0.5;
          if (d > 0.) d = d + 1.;
        }
      giza_ptext (x - d*tikx, y - d*tiky, lang - or, just, label);
    }

  /* stroke all the paths */
  int lc;
  giza_get_line_cap (&lc);
  giza_set_line_cap (CAIRO_LINE_CAP_SQUARE);
  _giza_stroke ();
  giza_set_line_cap (lc);
  _giza_set_trans (oldTrans);

  giza_flush_device ();

  /* Restore clipping */
  giza_set_viewport (Dev[id].VP.xmin, Dev[id].VP.xmax, Dev[id].VP.ymin, Dev[id].VP.ymax);
}

/**
 * Drawing: giza_tick_float
 *
 * Synopsis: Same functionality as giza_tick but takes floats instead of doubles.
 *
 * See Also: giza_tick
 */
void
giza_tick_float (float x1, float y1, float x2, float y2, float v,
                 float tickl, float tickr, float disp, float angle, const char *label)
{
  if (!_giza_check_device_ready ("giza_tick"))
    return;

  giza_tick((double) x1, (double) y1, (double) x2, (double) y2, (double) v,
            (double) tickl, (double) tickr, (double) disp, (double) angle, label);

}

/**
 *  Internal functionality for giza_tick, also used by giza_axis
 */
void
_giza_draw_tick(cairo_matrix_t mat, double ratio, double dr,
                double currentTickL_l, double currentTickL_r)
{
  double x,y,xpt,ypt;

  /* set location of tick start and end in non-rotated coords */
  x   = dr * ratio;
  xpt = x;
  y   = -currentTickL_l;
  ypt = currentTickL_r;

  /* rotate and translate */
  cairo_matrix_transform_point (&mat,&x,&y);
  cairo_matrix_transform_point (&mat,&xpt,&ypt);

  /* draw the tick(s) along the axis */
  cairo_move_to (Dev[id].context, x, y);
  cairo_line_to (Dev[id].context, xpt, ypt);

}
