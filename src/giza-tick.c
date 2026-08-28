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
#include "giza-axis-private.h"
#include "giza-tick-private.h"
#include <giza.h>
#include <math.h>

/**
 * Return one character height in device pixels (font cap height).
 */
double
_giza_character_height_device (void)
{
  return Dev[id].fontExtents.ascent;
}

/**
 * Label angle, justification and displacement for a tick label, following
 * the on-screen axis direction and the requested orientation angle.
 */
void
_giza_tick_label_style (double theta_deg, double disp, double angle,
                        double *label_angle, double *justification,
                        double *displacement)
{
  double orientation;

  *displacement = disp;
  *label_angle = theta_deg;
  orientation = fmod (angle, 360.);
  if (orientation < 0.) orientation += 360.;
  if (orientation > 45. && orientation <= 135.)
    {
      *justification = (*displacement < 0.) ? 1. : 0.;
    }
  else if (orientation > 135. && orientation <= 225.)
    {
      *justification = 0.5;
      if (*displacement < 0.) *displacement = *displacement - 1.;
    }
  else if (orientation > 225. && orientation <= 315.)
    {
      *label_angle = *label_angle + 90.;
      *justification = (*displacement < 0.) ? 0. : 1.;
    }
  else
    {
      *justification = 0.5;
      if (*displacement > 0.) *displacement = *displacement + 1.;
    }
}

/**
 * Draw one tick mark and optional label along an axis segment.
 * Caller must have GIZA_TRANS_WORLD active and must supply the precomputed
 * tick vectors from _giza_axis_tick_vectors.
 */
void
_giza_draw_tick_mark (double x1, double y1, double x2, double y2,
                      double ratio, double tickl, double tickr,
                      double disp, double angle, const char *label,
                      double theta_deg, double tick_perp_x, double tick_perp_y)
{
  int old_line_cap;
  double x, y, displacement, label_angle, justification, orientation;

  x = x1 + ratio * (x2 - x1);
  y = y1 + ratio * (y2 - y1);

  if (!(_giza_equal (tickl, 0.) && _giza_equal (tickr, 0.)))
    {
      cairo_move_to (Dev[id].context, x - tickr * tick_perp_x,
                     y - tickr * tick_perp_y);
      cairo_line_to (Dev[id].context, x + tickl * tick_perp_x,
                     y + tickl * tick_perp_y);
      /* stroke immediately: giza_ptext below resets the cairo path, so
       * a deferred stroke would lose the tick whenever a label is given */
      giza_get_line_cap (&old_line_cap);
      giza_set_line_cap (CAIRO_LINE_CAP_SQUARE);
      _giza_stroke ();
      giza_set_line_cap (old_line_cap);
    }

  if (label && label[0] != '\0')
    {
      _giza_tick_label_style (theta_deg, disp, angle, &label_angle,
                              &justification, &displacement);
      orientation = fmod (angle, 360.);
      if (orientation < 0.) orientation += 360.;
      giza_ptext (x - displacement * tick_perp_x,
                  y - displacement * tick_perp_y,
                  label_angle - orientation, justification, label);
    }
}

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
  int oldTrans;
  double theta_deg, tick_perp_x, tick_perp_y;

  if (!_giza_check_device_ready ("giza_tick"))
    return;

  _giza_expand_clipping ();

  oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  if (!_giza_axis_tick_vectors (x1, y1, x2, y2,
                                &theta_deg, &tick_perp_x, &tick_perp_y))
    {
      _giza_set_trans (oldTrans);
      giza_set_viewport (Dev[id].VP.xmin, Dev[id].VP.xmax,
                         Dev[id].VP.ymin, Dev[id].VP.ymax);
      return;
    }

  _giza_draw_tick_mark (x1, y1, x2, y2, v, tickl, tickr, disp, angle, label,
                        theta_deg, tick_perp_x, tick_perp_y);

  _giza_set_trans (oldTrans);

  giza_flush_device ();

  /* Restore clipping */
  giza_set_viewport (Dev[id].VP.xmin, Dev[id].VP.xmax,
                     Dev[id].VP.ymin, Dev[id].VP.ymax);
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

  giza_tick ((double) x1, (double) y1, (double) x2, (double) y2, (double) v,
             (double) tickl, (double) tickr, (double) disp, (double) angle, label);

}
