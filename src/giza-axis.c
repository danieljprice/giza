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
#include "giza-io-private.h"
#include "giza-stroke-private.h"
#include "giza-window-private.h"
#include "giza-viewport-private.h"
#include "giza-drivers-private.h"
#include "giza-tick-private.h"
#include <giza.h>
#include <math.h>
#include <stdio.h>

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
  int nMinTicks, major;
  double majTickL_l, subTickL_l, currentTickL_l;
  double majTickL_r, subTickL_r, currentTickL_r;
  char tmp[100];
  int i, i1, i2, j, jmax, jtmp;
  double x,y,theta,theta_deg,dr;

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

  /* get the angle to rotate in viewport coordinates */
  theta = atan2(y2-y1,x2-x1);
  theta_deg = theta / GIZA_DEG_TO_RAD;
  dr = sqrt(pow(x2-x1,2) + pow(y2-y1,2));

  cairo_matrix_t mat;
  cairo_matrix_init_translate(&mat,x1,y1);
  cairo_matrix_rotate(&mat,theta);

  double xch, ych;
  giza_get_character_size (GIZA_UNITS_WORLD, &xch, &ych);

  /* draw the line */
  if (draw_axis)
    {
      cairo_move_to (Dev[id].context, x1, y1);
      cairo_line_to (Dev[id].context, x2, y2);
    }

  /* set major tick length in pixels */
  majTickL_l = Dev[id].fontExtents.max_x_advance * dmajl;
  majTickL_r = Dev[id].fontExtents.max_x_advance * dmajr;
  subTickL_l = 0.;
  subTickL_r = 0.;

  /* convert to world coords */
  cairo_device_to_user_distance (Dev[id].context, &subTickL_l, &majTickL_l);
  cairo_device_to_user_distance (Dev[id].context, &subTickL_r, &majTickL_r);
  majTickL_l = -majTickL_l;
  majTickL_r = -majTickL_r;

  /* set minor tick length as a fraction of the major tick length */
  subTickL_l = majTickL_l * fmin;
  subTickL_r = majTickL_r * fmin;

  /* Choose x tick intervals */
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

  /* Only enter tick drawing if (1) any of the ticks/grid must be drawn AND
     (2) at least one of top, bottom or axis must be drawn because the ticks
     can only be drawn on any/all of these lines */
  if (draw_majticks || draw_minticks)
    {
      _giza_tick_intervals (v1, v2, intervalMin, &i1, &i2);
      jmax = 0;
      /* If log axis ticks always have 9 minor ticks */
      if (draw_log) jmax = 8;

      for (i = i1; i <= i2; i++)
        {
          for (j = 0; j <= jmax; j++)
            {
              /* log axis ticks are major when j = 0 */
              major = (i % nMinTicks == 0) && draw_majticks && (j == 0);
              currentTickL_l = subTickL_l;
              currentTickL_r = subTickL_r;
              if (major)
                 {
                 currentTickL_l = majTickL_l;
                 currentTickL_r = majTickL_r;
                 }
              val = (i + logTab[j]) * intervalMin;
              ratio = (val - v1) / (v2 - v1);

              /* don't draw outside the box */
              if ((val >= v2) || (val <= v1))
                continue;
              /* are we supposed to draw this tick anyway? */
              if ( !((major && draw_majticks) || draw_minticks) )
                continue;

              /* draw tick, rotate as necessary */
              _giza_draw_tick(mat,ratio,dr,currentTickL_l,currentTickL_r);

            }
        }
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
                 sprintf (tmp, "10");
              } else if (jtmp == 0) {
                 sprintf (tmp, "1");
              } else {
                 sprintf (tmp, "10^{%i}", jtmp);
              }
            }
          else
            {
              giza_format_number (i*nv, np, number_format, tmp, sizeof(tmp));
            }

          /* write the label */
          x = dr * ratio;
          y = ych * disp;
          cairo_matrix_transform_point (&mat,&x,&y);
          giza_ptext (x, y, theta_deg + angle, 0.5, tmp);

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

              /* write the label */
              x = dr * ratio;
              y = ych * disp;
              cairo_matrix_transform_point (&mat,&x,&y);
              giza_ptext (x, y, theta_deg + angle, 0.5, tmp);
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
