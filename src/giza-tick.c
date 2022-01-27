/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2022 Daniel Price
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

  double currentTickL_l;
  double currentTickL_r;
  double x,y,theta,theta_deg,dr;

  theta = atan2(y2-y1,x2-x1);
  theta_deg = theta / GIZA_DEG_TO_RAD;
  dr = sqrt(pow(x2-x1,2) + pow(y2-y1,2));

  cairo_matrix_t mat;
  cairo_matrix_init_translate(&mat,x1,y1);
  cairo_matrix_rotate(&mat,theta);

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  double xch, ych;
  giza_get_character_size (GIZA_UNITS_WORLD, &xch, &ych);

  /* set major tick length in pixels */
  currentTickL_l = Dev[id].fontExtents.max_x_advance * tickl;
  currentTickL_r = Dev[id].fontExtents.max_x_advance * tickr;
  double TickL_l_tmp = 0.;
  double TickL_r_tmp = 0.;

  /* convert to world coords */
  cairo_device_to_user_distance (Dev[id].context, &TickL_l_tmp, &currentTickL_l);
  cairo_device_to_user_distance (Dev[id].context, &TickL_r_tmp, &currentTickL_r);
  currentTickL_l = -currentTickL_l;
  currentTickL_r = -currentTickL_r;

  _giza_draw_tick(mat,v,dr,currentTickL_l,currentTickL_r);

  /* write the label */
  x = dr * v;
  y = ych * disp;
  cairo_matrix_transform_point (&mat,&x,&y);
  giza_ptext (x, y, theta_deg + angle, 0.5, label);

  /*_giza_stroke ();*/

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
