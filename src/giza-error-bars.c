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
 */
void
giza_error_bars (int dir, int n, double *xpts, double *ypts, double *error, double term)
{
  if (!_giza_check_device_ready ("giza_error_bars"))
    return;
  if (n < 1)
    { 
      _giza_warning ("giza_error_bars", "less than one point, skipping error bars");
      return;
    }

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
    default:
      _giza_warning ("giza_error_bars", "Invalid dir, skipping error bars");
      return;
    }

  int i;
  for (i = 0; i < n; i++)
    {
      if (vert)
	_giza_error_bar_vert (xpts[i], ypts[i], error[i] * sign, term);
      else
	_giza_error_bar_hori (xpts[i], ypts[i], error[i] * sign, term);
    }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}

/**
 * Drawing: giza_error_bars_float
 *
 * Synopsis: Same as giza_error_bars but takes floats.
 */
void
giza_error_bars_float (int dir, int n, float *xpts, float *ypts, float *error, float term)
{
  if (!_giza_check_device_ready ("giza_error_bars"))
    return;
  if (n < 1)
    { 
      _giza_warning ("giza_error_bars", "less than one point, skipping error bars");
      return;
    }

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
    default:
      _giza_warning ("giza_error_bars", "Invalid dir, skipping error bars");
      return;
    }

  int i;
  for (i = 0; i < n; i++)
    {
      if (vert)
	_giza_error_bar_vert ( (double) xpts[i], (double) ypts[i], (double) error[i] * sign, (double) term);
      else
	_giza_error_bar_hori ( (double) xpts[i], (double) ypts[i], (double) error[i] * sign, (double) term);
    }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}

/**
 * Drawing: giza_error_bars_vert
 *
 * Synopsis: Draws n vertical error bars. A call to giza_points must be made to draw the actual points.
 *
 * Input:
 *  -n      :- the number of bars to draw
 *  -xpts   :- the x world coords of the points
 *  -ypts1   :- the y world coords of the lower part of the error bar
 *  -ypts2   :- the y world coords of the upper part of the error bar
 */
void
giza_error_bars_vert (int n, double *xpts, double *ypts1, double *ypts2, double term)
{
  if (!_giza_check_device_ready ("giza_error_bars_vert"))
    return;
  
  if (n < 1)
    {
      _giza_warning ("giza_error_bars_vert", "Invalid number of points, not drawing the error bars");
      return;
    }
  
  double endWidth, dummy;
  giza_get_character_size (GIZA_UNITS_WORLD, &endWidth, &dummy);
  endWidth = 0.5 * endWidth * term;
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  
  int i;
  for (i = 0; i < n; i++)
    {
      // draw the bar
      cairo_move_to (context, xpts[i], ypts1[i]);
      cairo_line_to (context, xpts[i], ypts2[i]);
      
      // draw the ends
      cairo_move_to (context, xpts[i] - endWidth, ypts1[i]);
      cairo_line_to (context, xpts[i] + endWidth, ypts1[i]);
      
      cairo_move_to (context, xpts[i] - endWidth, ypts2[i]);
      cairo_line_to (context, xpts[i] + endWidth, ypts2[i]);
    }
  
  _giza_stroke ();
  
  _giza_set_trans (oldTrans);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}

/**
 * Drawing: giza_error_bars_vert_float
 *
 * Synopsis: Same functionality as giza_error_bars_vert but takes floats.
 */
void
giza_error_bars_vert_float (int n, float *xpts, float *ypts1, float *ypts2, float term)
{
  if (!_giza_check_device_ready ("giza_error_bars_vert_float"))
    return;

  if (n < 1)
    {
      _giza_warning ("giza_error_bars_vert_float", "Invalid number of points, not drawing the error bars");
      return;
    }
  
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

      // draw the bar
      cairo_move_to (context, currentX, currentY1);
      cairo_line_to (context, currentX, currentY2);
      
      // draw the ends
      cairo_move_to (context, currentX - endWidth, currentY1);
      cairo_line_to (context, currentX + endWidth, currentY1);
      
      cairo_move_to (context, currentX - endWidth, currentY2);
      cairo_line_to (context, currentX + endWidth, currentY2);
    }
  
  _giza_stroke ();
  
  _giza_set_trans (oldTrans);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}

/**
 * Draws n horizontal error bars. A call to giza_points must be made to draw the actual points.
 *
 * Input:
 *  -n      :- the number of bars to draw
 *  -xpts   :- the x world coords of the points
 *  -ypts   :- the y world coords of the points
 *  -errors :- the distance in world coords that each error bar should extend above and below the point
 */
  /*
void
giza_error_bars_hori (int n, double *xpts, double *ypts, double *errors)
{
  if (!_giza_check_device_ready ("giza_error_bars_hori"))
    return;
  
  if (n < 1)
    {
      _giza_warning ("giza_error_bars_hori", "Invalid number of points, not drawing the error bars");
      return;
    }
  
  double endWidth, dummy;
  giza_get_character_size (GIZA_UNITS_WORLD, &dummy, &endWidth);
  endWidth = 0.25 * endWidth;
  int oldTrans = _giza_get_trans();
  _giza_set_trans (GIZA_TRANS_WORLD);
  
  int i;
  for (i = 0; i < n; i++)
    {
      // draw the bar
      cairo_move_to (context, xpts[i] - errors[i], ypts[i]);
      cairo_line_to (context, xpts[i] + errors[i], ypts[i]);
      
      // draw the ends
      cairo_move_to (context, xpts[i] - errors[i], ypts[i] - endWidth);
      cairo_line_to (context, xpts[i] - errors[i], ypts[i] + endWidth);
      
      cairo_move_to (context, xpts[i] + errors[i], ypts[i] - endWidth);
      cairo_line_to (context, xpts[i] + errors[i], ypts[i] + endWidth);
    }
  
  _giza_stroke ();
  
  _giza_set_trans (oldTrans);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}
*/
   
/**
 * Same functionality as giza_error_bars_hori but takes floats.
 */
   /*
void
giza_error_bars_hori_float (int n, float *xpts, float *ypts, float *errors)
{
  if (!_giza_check_device_ready ("giza_error_bars_hori_float"))
    return;

  if (n < 1)
    {
      _giza_warning ("giza_error_bars_hori_float", "Invalid number of points, not drawing the error bars");
      return;
    }
  
  double endWidth, dummy, currentX, currentY, currentError;
  giza_get_character_size (1, &endWidth, &dummy);
  endWidth = 0.25 * endWidth;
  int oldTrans = _giza_get_trans();
  _giza_set_trans (GIZA_TRANS_WORLD);
  
  int i;
  for (i = 0; i < n; i++)
    {
      currentX = (double) xpts[i];
      currentY = (double) ypts[i];
      currentError = (double) errors[i];

      // draw the bar
      cairo_move_to (context, currentX - currentError, currentY);
      cairo_line_to (context, currentX - currentError, currentY);
      
      // draw the ends
      cairo_move_to (context, currentX - currentError, currentY - endWidth);
      cairo_line_to (context, currentX - currentError , currentY + endWidth);
      
      cairo_move_to (context, currentX + currentError, currentY - endWidth);
      cairo_line_to (context, currentX + currentError, currentY + endWidth);
    }
  
  _giza_stroke ();
  
  _giza_set_trans (oldTrans);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}
*/
/**
 * Plots a single vertical error bar at x, y of length length (measured in +ve x direction),
 * with terminals of length term
 */
static void
_giza_error_bar_vert (double x, double y, double error, double term)
{
  double endWidth, dummy;
  giza_get_character_size (GIZA_UNITS_WORLD, &dummy, &endWidth);
  endWidth = 0.5 * endWidth * term;

  // draw the bar
  cairo_move_to (context, x, y);
  cairo_line_to (context, x + error, y);
  
  // draw the ends
  cairo_move_to (context, x + error, y - endWidth * 0.5);
  cairo_line_to (context, x + error, y + endWidth * 0.5);
}

/**
 * Plots a single horizontal error bar at x, y of length length (measured in +ve y direction),
 * with terminals of length term
 */
static void
_giza_error_bar_hori (double x, double y, double error, double term)
{
  double endWidth, dummy;
  giza_get_character_size (GIZA_UNITS_WORLD, &endWidth, &dummy);
  endWidth = 0.5 * endWidth * term;

  // draw the bar
  cairo_move_to (context, x, y);
  cairo_line_to (context, x, y + error);
  
  // draw the end
  cairo_move_to (context, x - endWidth * 0.5, y + error);
  cairo_line_to (context, x + endWidth * 0.5, y + error);
}
