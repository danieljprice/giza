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
#include "giza-io-private.h"
#include "giza-stroke-private.h"
#include "giza-window-private.h"
#include "giza-viewport-private.h"
#include "giza-drivers-private.h"
#include "giza-axis-private.h"
#include "giza-tick-private.h"
#include <giza.h>
#include <math.h>
#include <stdio.h>

/**
 * Draw major and minor tick marks along an axis segment.
 */
static void
_giza_axis_draw_ticks (double x1, double y1, double x2, double y2,
                       double v1, double v2, double intervalMin, int nMinTicks,
                       int draw_majticks, int draw_minticks, int draw_log,
                       const double *logTab, double dmajl, double dmajr,
                       double fmin, double tick_sign, double angle,
                       double theta_deg, double tick_perp_x, double tick_perp_y)
{
  int i, i1, i2, j, jmax, major;
  double val, ratio, vtol, currentTickL_l, currentTickL_r;

  _giza_tick_intervals (v1, v2, intervalMin, &i1, &i2);
  jmax = 0;
  if (draw_log) jmax = 8;

  for (i = i1; i <= i2; i++)
    {
      for (j = 0; j <= jmax; j++)
        {
          major = (i % nMinTicks == 0) && draw_majticks && (j == 0);
          currentTickL_l = dmajl * fmin;
          currentTickL_r = dmajr * fmin;
          if (major)
            {
              currentTickL_l = dmajl;
              currentTickL_r = dmajr;
            }
          val = (i + logTab[j]) * intervalMin;
          ratio = (val - v1) / (v2 - v1);

          vtol = 1.e-10 * fabs (v2 - v1);
          if ((val > ((v1 > v2) ? v1 : v2) + vtol) ||
              (val < ((v1 < v2) ? v1 : v2) - vtol))
            continue;
          if (!((major && draw_majticks) || draw_minticks))
            continue;

          _giza_draw_tick_mark (x1, y1, x2, y2, ratio,
                                tick_sign * currentTickL_l,
                                tick_sign * currentTickL_r,
                                0., angle, "",
                                theta_deg, tick_perp_x, tick_perp_y);
        }
    }
}

/**
 * Drawing: giza_axis
 *
 * Synopsis: Draw a labelled axis from (x1,y1) to (x2,y2)
 *
 * Input:
 *  -opt  :- String of options for the axis.
 *           The options may be in any order. See
 *           below for details
 *  -x1   :- starting x position in world coordinates
 *  -y1   :- starting y position in world coordinates
 *  -x2   :- finishing x position in world coordinates
 *  -y2   :- finishing y position in world coordinates
 *  -v1   :- axis value at starting position
 *  -v2   :- axis value at finishing position
 *  -tick :- The distance, in world coordinates,
 *            between major ticks on the axis.
 *            If 0.0 the interval is chosen
 *            automatically.
 *  -nsub :- The number of minor ticks to be placed
 *            between each major tick. If 0 the
 *            number is chosen automatically. Ignored if log axis.
 *  -dmajl :- Length of major tick marks drawn to "left/bottom" of axis
 *             in units of character height
 *  -dmajr :- Length of major tick marks drawn to "right/top" of axis
 *             in units of character height
 *  -fmin  :- Length of minor tick marks as fraction of major
 *  -disp  :- Displacement of labels from axis
 *             in units of character height
 *  -angle :- Label orientation; angle between text and direction of axis; in degrees
 *
 * Options:
 *  -T :- Draw major ticks.
 *  -S :- Draw minor ticks.
 *  -N :- Label the axis
 *  -L :- Label axis logarithmically
 *  -H :- Hide the axis (draw ticks only)
 *  -I :- 'Invert' tick marks, draw them on opposite side
 *  -1 :- Force decimal labelling instead of automatic choice (see giza_format_number)
 *  -2 :- Force exponential labelling instead of automatic choice (see giza_format_number)
 *
 * See Also: giza_axis_float, giza_box, giza_tick, giza_box_time
 *
 */
void
giza_axis (const char *opt, double x1, double y1, double x2, double y2,
           double v1, double v2, double tick, int nsub,
           double dmajl, double dmajr, double fmin, double disp, double angle)
{
  if (!_giza_check_device_ready ("giza_axis"))
    return;

  _giza_expand_clipping ();

  int oldBuf;
  giza_get_buffering(&oldBuf);

  /* Table of log10() values for log axis ticks */
  double logTab[9];
  int k;
  for (k = 0; k < 9; k++)
    {
      logTab[k] = log10 (k + 1);
    }

  /* Begin buffering */
  giza_begin_buffer ();

  int draw_majticks = 0, draw_minticks = 0, draw_labels = 0,
      draw_log = 0, draw_invert = -1, draw_axis = 1;

  int number_format = Dev[id].number_format;

  double intervalMaj, intervalMin, val, ratio;
  int nv, np;
  int nMinTicks;
  double tick_sign;
  double theta_deg, tick_perp_x, tick_perp_y;
  char tmp[100];
  int i, i1, i2, j, jtmp;

  /* set x-options */
  for (i = 0; opt[i]; i++)
    {
    switch (opt[i])
      {
      case ('h'):
      case ('H'):
            draw_axis = 0;
        break;
      case ('t'):
      case ('T'):
        draw_majticks = 1;
        break;
      case ('s'):
      case ('S'):
        draw_minticks = 1;
        break;
          /* Any of nmNM means labels need to be drawn. mM says to do so unconventionally. */
      case ('n'):
      case ('N'):
        draw_labels = 1;
        break;
      case ('l'):
      case ('L'):
        draw_log = 1;
        break;
      case ('i'):
      case ('I'):
        draw_invert = 1;
        break;
      case ('1'):
        number_format = GIZA_NUMBER_FORMAT_DEC;
        break;
      case ('2'):
        number_format = GIZA_NUMBER_FORMAT_EXP;
        break;
      default:
        break;
      }
    }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  /* draw the line */
  if (draw_axis)
    {
      int oldCap;
      cairo_move_to (Dev[id].context, x1, y1);
      cairo_line_to (Dev[id].context, x2, y2);
      /* stroke immediately: giza_ptext below resets the cairo path,
       * so a deferred stroke would lose the axis line */
      giza_get_line_cap (&oldCap);
      giza_set_line_cap (CAIRO_LINE_CAP_SQUARE);
      _giza_stroke ();
      giza_set_line_cap (oldCap);
    }

  /* draw_invert<0 is default; >0 means option I was specified (see giza_box) */
  tick_sign = (draw_invert > 0) ? -1.0 : 1.0;

  /* Choose tick intervals */
  if (draw_log)
    {
      nMinTicks = 1;
      intervalMaj = 1.;
    }
  else if (_giza_equal(tick,0.))
    {
      intervalMaj = 7. * Dev[id].fontExtents.max_x_advance /
        ((Dev[id].VP.xmax - Dev[id].VP.xmin)*Dev[id].width);
      if (intervalMaj > 0.2)
             intervalMaj = 0.2;
      if (intervalMaj < 0.05)
             intervalMaj = 0.05;
      intervalMaj = intervalMaj * (v2 - v1);
      intervalMaj = giza_round (intervalMaj, &nMinTicks);
    }
  else
    {
      intervalMaj = tick;
      nMinTicks = nsub;
      if (nsub < 1 || !draw_minticks) nMinTicks = 1;
    }
  intervalMin = intervalMaj / (double) nMinTicks;

  if (_giza_axis_tick_vectors (x1, y1, x2, y2,
                               &theta_deg, &tick_perp_x, &tick_perp_y))
    {
      /* Only enter tick drawing if (1) any of the ticks/grid must be drawn AND
       * (2) at least one of top, bottom or axis must be drawn because the ticks
       * can only be drawn on any/all of these lines */
      if (draw_majticks || draw_minticks)
        {
          _giza_axis_draw_ticks (x1, y1, x2, y2, v1, v2, intervalMin, nMinTicks,
                                 draw_majticks, draw_minticks, draw_log, logTab,
                                 dmajl, dmajr, fmin, tick_sign, angle,
                                 theta_deg, tick_perp_x, tick_perp_y);
          _giza_stroke ();
        }

      /* labels */
      if (draw_labels)
        {
          _giza_tick_intervals (v1, v2, intervalMaj, &i1, &i2);
          np = (int) floor (log10 (fabs (intervalMaj)));
          nv = _giza_nint (intervalMaj/pow (10., np));

          for (i = i1; i <= i2; i++)
            {
              val = i * intervalMaj;
              ratio = (val - v1) / (v2 - v1);
              /* don't draw label if outside frame */
              if (ratio < 0. || ratio > 1.)
                continue;
              if (draw_log)
                {
                  jtmp = _giza_nint(val);
                  if (jtmp == 1) {
                     snprintf (tmp, sizeof(tmp), "10");
                  } else if (jtmp == 0) {
                     snprintf (tmp, sizeof(tmp), "1");
                  } else {
                     snprintf (tmp, sizeof(tmp), "10^{%i}", jtmp);
                  }
                }
              else
                {
                  giza_format_number (i*nv, np, number_format, tmp, sizeof(tmp));
                }

              _giza_draw_tick_mark (x1, y1, x2, y2, ratio, 0., 0., disp, angle, tmp,
                                    theta_deg, tick_perp_x, tick_perp_y);

            }
          _giza_stroke ();
        }

      /* extra labels for log axis */
      if (draw_labels && draw_log && (v2 - v1 < 2.))
        {
          _giza_tick_intervals (v1, v2, intervalMin, &i1, &i2);
          for (i = i1 - 1; i <= i2; i++)
            {
              for (j = 1; j <= 4; j += 3)
                {
                  val = (i + logTab[j]) * intervalMin;
                  if (val <= v2 && val >= v1)
                    {
                      ratio = (val - v1) / (v2 - v1);
                      val = pow (10, val);
                      giza_format_number (j+1, _giza_nint (i * intervalMin), number_format, tmp, sizeof(tmp));

                      _giza_draw_tick_mark (x1, y1, x2, y2, ratio, 0., 0., disp, angle, tmp,
                                            theta_deg, tick_perp_x, tick_perp_y);
                    }
                }
            }
        }
    }

  /* stroke all the paths */
  int lc;
  giza_get_line_cap (&lc);
  giza_set_line_cap (CAIRO_LINE_CAP_SQUARE);
  _giza_stroke ();
  giza_set_line_cap (lc);
  _giza_set_trans (oldTrans);

  /* end buffer if it was not on before this function call */
  if (!oldBuf)
    {
      giza_end_buffer ();
    }

  giza_flush_device ();

  /* Restore clipping */
  giza_set_viewport (Dev[id].VP.xmin, Dev[id].VP.xmax, Dev[id].VP.ymin, Dev[id].VP.ymax);
}

/**
 * Compute on-screen axis direction and a perpendicular unit step of one
 * character height in world coordinates.  Returns 1 on success, 0 if the
 * axis segment has zero length in device space.
 */
int
_giza_axis_tick_vectors (double x1, double y1, double x2, double y2,
                         double *theta_deg,
                         double *tick_perp_x, double *tick_perp_y)
{
  double axis_dx, axis_dy, axis_length, character_height;

  axis_dx = x2 - x1;
  axis_dy = y2 - y1;
  cairo_user_to_device_distance (Dev[id].context, &axis_dx, &axis_dy);
  axis_length = sqrt (axis_dx * axis_dx + axis_dy * axis_dy);
  if (_giza_equal (axis_length, 0.))
    return 0;

  *theta_deg = -atan2 (axis_dy, axis_dx) / GIZA_DEG_TO_RAD;
  character_height = _giza_character_height_device ();
  *tick_perp_x = axis_dy / axis_length * character_height;
  *tick_perp_y = -axis_dx / axis_length * character_height;
  cairo_device_to_user_distance (Dev[id].context, tick_perp_x, tick_perp_y);
  return 1;
}

/**
 * Drawing: giza_axis_float
 *
 * Synopsis: Same functionality as giza_axis but takes floats instead of doubles.
 *
 * See Also: giza_axis
 */
void
giza_axis_float (const char *opt, float x1, float y1, float x2, float y2,
          float v1, float v2, float step, int nsub,
          float dmajl, float dmajr, float fmin, float disp, float angle)
{
  if (!_giza_check_device_ready ("giza_axis"))
    return;

  giza_axis(opt, (double) x1, (double) y1, (double) x2, (double) y2,
          (double) v1, (double) v2, (double) step, nsub,
          (double) dmajl, (double) dmajr, (double) fmin, (double) disp, (double) angle);

}
