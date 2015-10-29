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
#include "giza-stroke-private.h"
#include "giza-io-private.h"
#include "giza-transforms-private.h"
#include <giza.h>

static void _giza_error_bar_vert (double x, double y, double error, double term);
static void _giza_error_bar_hori (double x, double y, double error, double term);

/**
 * Drawing: giza_error_bars
 *
 * Synopsis: Draws error bars.
 *
 * Input:
 *  -dir  :- the direction of the error bar, see below
 *  -n    :- the number of points
 *  -xpts :- the x world coords of the points
 *  -ypts :- the y world coords of the points
 *  -error:- the length of the bar to be drawn in world coords
 *  -term :- the length of the terminals as a multiple of the default length
 *
 * Directions:
 *  -1 :- +x
 *  -2 :- +y
 *  -3 :- -x
 *  -4 :- -y
 *  -5 :- +x and -x
 *  -6 :- +y and -y
 *  -7 :- -y but instead of drawing bars use a semi-transparent shaded region
 *  -8 :- +y using a semi-transparent shaded region
 *  -9 :- +y and -y using a semi-transparent shaded region
 */
void
giza_error_bars (int dir, int n, const double *xpts, const double *ypts, const double *error, double term)
{
  if (!_giza_check_device_ready ("giza_error_bars"))
    return;
  if (n < 1) return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  int sign, vert;

  switch (dir)
    {
    case 1:
      sign = 1;
      vert = 1;
      break;
    case 2:
      sign = 1;
      vert = 0;
      break;
    case 3:
      sign = -1;
      vert = 1;
      break;
    case 4:
      sign = -1;
      vert = 0;
      break;
    case 5:
      giza_error_bars (1, n, xpts, ypts, error, term);
      giza_error_bars (3, n, xpts, ypts, error, term);
      return;
    case 6:
      giza_error_bars (2, n, xpts, ypts, error, term);
      giza_error_bars (4, n, xpts, ypts, error, term);
      return;
    case 7:
    case 8:
    case 9:
      vert = 1;
      sign = 0;
      break;
    default:
      _giza_warning ("giza_error_bars", "Invalid dir, skipping error bars");
      return;
    }

  int i;
  if (dir >= 7) 
/*
 *  Semi-transparent shading of error region
 */     
    {
      double a;
      _giza_get_alpha(&a);
      _giza_set_alpha(0.15);

      if (dir == 7 || dir == 9) {
         cairo_move_to (Dev[id].context, xpts[0], ypts[0] + error[0]);
         for (i = 0; i < n; i++)
           {
             /* draw line along top of error bars */
             cairo_line_to (Dev[id].context, xpts[i], ypts[i] + error[i]);
           }

         /*
          * For +y shaded error region only, close path
          * by tracing along original data line 
          */
         if (dir == 7) {
            for (i = n-1; i >= 0; i--)
              {
                cairo_line_to (Dev[id].context, xpts[i], ypts[i]);
              }
         }
	 else
	   {	      
	      /* trace from top line to bottom one */
	      cairo_line_to (Dev[id].context, xpts[n-1], ypts[n-1] - error[n-1]);
	   }	 
      }
      
      if (dir == 8 || dir == 9) {
         cairo_move_to (Dev[id].context, xpts[n-1], ypts[n-1] - error[n-1]);
         for (i = n-1; i >= 0; i--)
           {
             /* draw line along bottom of error bars */
             cairo_line_to (Dev[id].context, xpts[i], ypts[i] - error[i]);
           }

         /*
          * For -y shaded error region only, close path
          * by tracing along original data line 
          */
         if (dir == 8) {
            for (i = 0; i < n; i++)
              {
                cairo_line_to (Dev[id].context, xpts[i], ypts[i]);
              }
         } else {
          /*
           * Otherwise, close path by tracing back to original point
           */
           cairo_line_to(Dev[id].context, xpts[0], ypts[0] + error[0]);
         }
      }
      cairo_close_path(Dev[id].context);
      cairo_fill(Dev[id].context);
      _giza_stroke ();
      _giza_set_alpha(a);
    }
  else
/*                          ___
 *  Normal error bar style _|_  or |-|
 */
    {
      for (i = 0; i < n; i++)
        {
          if (vert)
	    _giza_error_bar_vert (xpts[i], ypts[i], error[i] * sign, term);
          else
	    _giza_error_bar_hori (xpts[i], ypts[i], error[i] * sign, term);
        }
      _giza_stroke ();
    }

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}

/**
 * Drawing: giza_error_bars_float
 *
 * Synopsis: Same as giza_error_bars but takes floats.
 */
void
giza_error_bars_float (int dir, int n, const float *xpts, const float *ypts, const float *error, float term)
{
  if (!_giza_check_device_ready ("giza_error_bars"))
    return;
  if (n < 1) return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  int sign, vert;

  switch (dir)
    {
    case 1:
      sign = 1;
      vert = 1;
      break;
    case 2:
      sign = 1;
      vert = 0;
      break;
    case 3:
      sign = -1;
      vert = 1;
      break;
    case 4:
      sign = -1;
      vert = 0;
      break;
    case 5:
      giza_error_bars_float (1, n, xpts, ypts, error, term);
      giza_error_bars_float (3, n, xpts, ypts, error, term);
      return;
    case 6:
      giza_error_bars_float (2, n, xpts, ypts, error, term);
      giza_error_bars_float (4, n, xpts, ypts, error, term);
      return;
    case 7:
    case 8:
    case 9:
      vert = 1;
      sign = 0;
      break;
    default:
      _giza_warning ("giza_error_bars", "Invalid dir, skipping error bars");
      return;
    }

  int i;
  if (dir >= 7) 
/*
 *  Semi-transparent shading of error region
 */
    {
      double a;
      _giza_get_alpha(&a);
      _giza_set_alpha(0.15);

      if (dir == 7 || dir == 9) {
         cairo_move_to (Dev[id].context, xpts[0], ypts[0] + error[0]);
         for (i = 0; i < n; i++)
           {
             /* draw line along top of error bars */
             cairo_line_to (Dev[id].context, xpts[i], ypts[i] + error[i]);
           }

         /*
          * For +y shaded error region only, close path
          * by tracing along original data line 
          */
         if (dir == 7) {
            for (i = n-1; i >= 0; i--)
              {
                cairo_line_to (Dev[id].context, xpts[i], ypts[i]);
              }
         }
      }
      
      if (dir == 8 || dir == 9) {
         cairo_move_to (Dev[id].context, xpts[n-1], ypts[n-1] - error[n-1]);
         for (i = n-1; i >= 0; i--)
           {
             /* draw line along bottom of error bars */
             cairo_line_to (Dev[id].context, xpts[i], ypts[i] - error[i]);
           }

         /*
          * For -y shaded error region only, close path
          * by tracing along original data line 
          */
         if (dir == 8) {
            for (i = 0; i < n; i++)
              {
                cairo_line_to (Dev[id].context, xpts[i], ypts[i]);
              }
         } else {
          /*
           * Otherwise, close path by tracing back to original point
           */
           cairo_line_to(Dev[id].context, xpts[0], ypts[0] + error[0]);
         }
      }
      cairo_close_path(Dev[id].context);
      cairo_fill(Dev[id].context);
      _giza_stroke ();
      _giza_set_alpha(a);
    }
  else
/*                          ___
 *  Normal error bar style _|_  or |-|
 */
    {
      for (i = 0; i < n; i++)
        {
          if (vert)
	    _giza_error_bar_vert ( (double) xpts[i], (double) ypts[i], (double) error[i] * sign, (double) term);
          else
	    _giza_error_bar_hori ( (double) xpts[i], (double) ypts[i], (double) error[i] * sign, (double) term);
        }
      _giza_stroke ();
    }

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}

/**
 * Drawing: giza_error_bars_vert
 *
 * Synopsis: Draws n vertical error bars. A call to giza_points must be made to draw the actual points.
 *
 * Input:
 *  -n      :- the number of bars to draw
 *  -xpts   :- the x world coords of the points
 *  -ypts1  :- the y world coords of the lower part of the error bar
 *  -ypts2  :- the y world coords of the upper part of the error bar
 *  -term   :- length of the terminals, as a multiple of default length (T <= 0.0 means no terminals drawn)
 */
void
giza_error_bars_vert (int n, const double *xpts, const double *ypts1, const double *ypts2, double term)
{
  if (!_giza_check_device_ready ("giza_error_bars_vert"))
    return;

  if (n < 1) return;

  double endWidth, dummy;
  giza_get_character_size (GIZA_UNITS_WORLD, &endWidth, &dummy);
  endWidth = 0.5 * endWidth * term;
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  int i;
  for (i = 0; i < n; i++)
    {
      /* draw the bar */
      cairo_move_to (Dev[id].context, xpts[i], ypts1[i]);
      cairo_line_to (Dev[id].context, xpts[i], ypts2[i]);

      /* draw the ends */
      if (term > 0.)
        {
          cairo_move_to (Dev[id].context, xpts[i] - endWidth, ypts1[i]);
          cairo_line_to (Dev[id].context, xpts[i] + endWidth, ypts1[i]);
          cairo_move_to (Dev[id].context, xpts[i] - endWidth, ypts2[i]);
          cairo_line_to (Dev[id].context, xpts[i] + endWidth, ypts2[i]);
        }
    }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}

/**
 * Drawing: giza_error_bars_vert_float
 *
 * Synopsis: Same functionality as giza_error_bars_vert but takes floats.
 */
void
giza_error_bars_vert_float (int n, const float *xpts, const float *ypts1, const float *ypts2, float term)
{
  if (!_giza_check_device_ready ("giza_error_bars_vert_float"))
    return;

  if (n < 1) return;

  double endWidth, dummy, currentX, currentY1, currentY2;
  giza_get_character_size (GIZA_UNITS_WORLD, &endWidth, &dummy);
  endWidth = 0.5 * endWidth * (double)term;
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  int i;
  for (i = 0; i < n; i++)
    {
      currentX = (double) xpts[i];
      currentY1 = (double) ypts1[i];
      currentY2 = (double) ypts2[i];

      /* draw the bar */
      cairo_move_to (Dev[id].context, currentX, currentY1);
      cairo_line_to (Dev[id].context, currentX, currentY2);

      /* draw the ends */
      cairo_move_to (Dev[id].context, currentX - endWidth, currentY1);
      cairo_line_to (Dev[id].context, currentX + endWidth, currentY1);

      cairo_move_to (Dev[id].context, currentX - endWidth, currentY2);
      cairo_line_to (Dev[id].context, currentX + endWidth, currentY2);
    }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}

/**
 * Draws n horizontal error bars. A call to giza_points must be made to draw the actual points.
 *
 * Input:
 *  -n      :- the number of bars to draw
 *  -xpts1  :- the x world coords of the lower end of the error bars
 *  -xpts2  :- the x world coords of the upper end of the error bars
 *  -ypts   :- the y world coords of the points
 *  -term   :- length of the terminals, as a multiple of default length (T <= 0.0 means no terminals drawn)
 */
void
giza_error_bars_hori (int n, const double *xpts1, const double *xpts2, const double *ypts, double term)
{
  if (!_giza_check_device_ready ("giza_error_bars_hori"))
    return;

  if (n < 1) return;

  double endWidth, dummy;
  giza_get_character_size (GIZA_UNITS_WORLD, &dummy, &endWidth);
  endWidth = 0.5 * endWidth * term;
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  int i;
  for (i = 0; i < n; i++)
    {
      /* draw the bar */
      cairo_move_to (Dev[id].context, xpts1[i], ypts[i]);
      cairo_line_to (Dev[id].context, xpts2[i], ypts[i]);

      /* draw the ends */
      if (term > 0.)
        {
          cairo_move_to (Dev[id].context, xpts1[i], ypts[i] - endWidth);
          cairo_line_to (Dev[id].context, xpts1[i], ypts[i] + endWidth);
          cairo_move_to (Dev[id].context, xpts2[i], ypts[i] - endWidth);
          cairo_line_to (Dev[id].context, xpts2[i], ypts[i] + endWidth);
        }
    }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}

/**
 * Same functionality as giza_error_bars_hori but takes floats.
 */
void
giza_error_bars_hori_float (int n, const float *xpts1, const float *xpts2, const float *ypts, float term)
{
  if (!_giza_check_device_ready ("giza_error_bars_hori_float"))
    return;

  if (n < 1) return;

  double endWidth, dummy, currentX1, currentX2, currentY;
  giza_get_character_size (GIZA_UNITS_WORLD, &dummy, &endWidth);
  endWidth = 0.5 * endWidth * term;
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  int i;
  for (i = 0; i < n; i++)
    {
      currentX1 = (double) xpts1[i];
      currentX2 = (double) xpts2[i];
      currentY  = (double) ypts[i];

      /* draw the bar */
      cairo_move_to (Dev[id].context, currentX1, currentY);
      cairo_line_to (Dev[id].context, currentX2, currentY);

      /* draw the ends */
      if (term > 0.)
        {
          cairo_move_to (Dev[id].context, currentX1, currentY - endWidth);
          cairo_line_to (Dev[id].context, currentX1, currentY + endWidth);
          cairo_move_to (Dev[id].context, currentX2, currentY - endWidth);
          cairo_line_to (Dev[id].context, currentX2, currentY + endWidth);
        }
    }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}
/**
 * Plots a single vertical error bar at x, y of length error (measured in +ve x direction),
 * with terminals of length term
 */
static void
_giza_error_bar_vert (double x, double y, double error, double term)
{
  double endWidth, dummy;
  giza_get_character_size (GIZA_UNITS_WORLD, &dummy, &endWidth);
  endWidth = 0.5 * endWidth * term;

  /* draw the bar */
  cairo_move_to (Dev[id].context, x, y);
  cairo_line_to (Dev[id].context, x + error, y);

  /* draw the ends */
  if (term > 0.)
    {
      cairo_move_to (Dev[id].context, x + error, y - endWidth * 0.5);
      cairo_line_to (Dev[id].context, x + error, y + endWidth * 0.5);
    }
}

/**
 * Plots a single horizontal error bar at x, y of length error (measured in +ve y direction),
 * with terminals of length term
 */
static void
_giza_error_bar_hori (double x, double y, double error, double term)
{
  double endWidth, dummy;
  giza_get_character_size (GIZA_UNITS_WORLD, &endWidth, &dummy);
  endWidth = 0.5 * endWidth * term;

  /* draw the bar */
  cairo_move_to (Dev[id].context, x, y);
  cairo_line_to (Dev[id].context, x, y + error);

  /* draw the end */
  if (term > 0.)
    {
      cairo_move_to (Dev[id].context, x - endWidth * 0.5, y + error);
      cairo_line_to (Dev[id].context, x + endWidth * 0.5, y + error);
    }
}
