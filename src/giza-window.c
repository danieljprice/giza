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
 * Copyright (C) 2010-2011 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include "giza-private.h"
#include "giza-window-private.h"
#include "giza-io-private.h"
#include "giza-transforms-private.h"
#include "giza-viewport-private.h"
#include <giza.h>

/**
 * Settings: giza_set_window
 *
 * Synopsis: Sets the limits of the axis.
 *
 * Input:
 *  -x1 :- the lowest value of the x axis
 *  -x2 :- the highest value of the x axis
 *  -y1 :- the lowest value of the y axis
 *  -y2 :- the highest value of the y axis
 */
void
giza_set_window (double x1, double x2, double y1, double y2)
{
  if (!_giza_check_device_ready ("giza_set_window"))
    return;

  /* Check the input is valid */
  if (_giza_equal(x2,x1) || _giza_equal(y2,y1))
    {
      _giza_error ("giza_set_window", "Invalid arguments, reverting to default");
      x1 = GIZA_DEFAULT_WINDOW_X1;
      x2 = GIZA_DEFAULT_WINDOW_X2;
      y1 = GIZA_DEFAULT_WINDOW_Y1;
      y2 = GIZA_DEFAULT_WINDOW_Y2;
    }

  Win.xmin = x1;
  Win.xmax = x2;
  Win.ymin = y1;
  Win.ymax = y2;

  /* Transform to normalised device coords */
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_NORM);

  /* Transform from normalised device coords to world coords. */
  /* Scaling */
  double dxWin = (Win.xmax - Win.xmin);
  double dyWin = (Win.ymax - Win.ymin);

  double horiScale = (VP.xmax - VP.xmin) / dxWin; /* this is safe as we have already */
  double vertScale = (VP.ymax - VP.ymin) / dyWin; /* checked x1 /= x2 and y1 /= y2   */

  /* Translation: */
  double horiTrans = VP.xmin - Win.xmin * horiScale;
  double vertTrans = VP.ymin - Win.ymin * vertScale;

  cairo_matrix_init (&(Win.userCoords), horiScale, 0, 0, vertScale, horiTrans, vertTrans);
  cairo_matrix_multiply(&(Win.userCoords),&(Win.userCoords),&(Win.normCoords));

/*  cairo_transform (context, &(Win.userCoords)); */
/*  cairo_get_matrix (context, &(Win.userCoords)); */

  _giza_set_trans (oldTrans);
}

/**
 * Settings: giza_set_window_float
 *
 * Synopsis: Same functionalityas giza_set_window but uses floats.
 *
 * See Also: giza_set_window
 */
void
giza_set_window_float (float x1, float x2, float y1, float y2)
{
  giza_set_window ((double) x1, (double) x2, (double) y1, (double) y2);
}

/**
 * Settings: giza_set_window_equal_scale
 *
 * Synopsis: Sets the window so the x axis ranges from x1 to x2 and y from y1 to y2,
 * then adjusts the view port to the largest possible size that that
 * allows the axis have equal scales.
 *
 * Input:
 *  -x1 :- the lowest value of the x axis
 *  -x2 :- the highest value of the x axis
 *  -y1 :- the lowest value of the y axis
 *  -y2 :- the highest value of the y axis
 */
void
giza_set_window_equal_scale (double x1, double x2, double y1, double y2)
{
  if (!_giza_check_device_ready ("giza_set_window_equal_scale"))
    return;

  /* Check the input is valid */
  if (_giza_equal(x2,x1) || _giza_equal(y2,y1))
    {
      _giza_error ("giza_set_window_equal_scale", "Invalid arguments, reverting to default");
      x1 = GIZA_DEFAULT_WINDOW_X1;
      x2 = GIZA_DEFAULT_WINDOW_X2;
      y1 = GIZA_DEFAULT_WINDOW_Y1;
      y2 = GIZA_DEFAULT_WINDOW_Y2;
    }

  /* set xrange and yrange */
  double yrange = abs(y2 - y1);
  double xrange = abs(x2 - x1);
  double scale, scalex, scaley, newWidth, newHeight;

  /* Scale is Device units per World coords. */
  scaley = (VP.ymax - VP.ymin) * Dev.height / yrange;
  scalex = (VP.xmax - VP.xmin) * Dev.width /  xrange;
  if (scaley < scalex)
    {
      scale = scaley;
    }
  else
    {
      scale = scalex;
    }

  /* Find the position of the new viewport relative to the centre of the old */
  /* Find the width of the new vp in normalised device coords */
  newWidth = scale * xrange / Dev.width;
  VP.xmin = (VP.xmax + VP.xmin - newWidth) * 0.5;
  VP.xmax = VP.xmin + newWidth;

  /* Find the height of the new vp */
  newHeight = scale * yrange / Dev.height;
  VP.ymin = (VP.ymax + VP.ymin - newHeight) * 0.5;
  VP.ymax = VP.ymin + newHeight;

  /* Set the window and vp */
  giza_set_window (x1, x2, y1, y2);
  giza_set_viewport (VP.xmin, VP.xmax, VP.ymin, VP.ymax);
}

/**
 * Settings: giza_set_window_equal_scale_float
 *
 * Synopsis: Same functionality as giza_set_window_equal_scale but uses floats
 *
 * See Also: giza_set_window_equal_scale
 */
void
giza_set_window_equal_scale_float (float x1, float x2, float y1, float y2)
{
  giza_set_window_equal_scale ((double) x1, (double) x2, (double) y1, (double) y2);
}

/**
 * Settings: giza_get_window
 *
 * Synopsis: Query the boundaries of the window in world coords
 *
 * Input:
 *  -x1 :- Gets set to the lowest value of the x axis
 *  -x2 :- Gets set to the highest value of the x axis
 *  -y1 :- Gets set to the lowest value of the y axis
 *  -y2 :- Gets set to the highest value of the y axis
 */
void
giza_get_window (double *x1, double *x2, double *y1, double *y2)
{
  if(!_giza_check_device_ready("giza_get_window")) return;

  *x1 = Win.xmin;
  *x2 = Win.xmax;
  *y1 = Win.ymin;
  *y2 = Win.ymax;
}

/**
 * Settings: giza_get_window_float
 *
 * Synopsis: Same functionality as giza_get_window but uses floats
 */
void
giza_get_window_float (float *x1, float *x2, float *y1, float *y2)
{
  if(!_giza_check_device_ready("giza_get_window_float")) return;

  *x1 = (float) Win.xmin;
  *x2 = (float) Win.xmax;
  *y1 = (float) Win.ymin;
  *y2 = (float) Win.ymax;
}

/**
 * Sets the window to the default (before viewport is set). Must be followed by
 * a call to set viewport, which in turn calls the set_window routine
 */
void
_giza_init_window (void)
{
  if(!_giza_check_device_ready("_giza_init_window")) return;

  Win.xmin = GIZA_DEFAULT_WINDOW_X1;
  Win.xmax = GIZA_DEFAULT_WINDOW_X2;
  Win.ymin = GIZA_DEFAULT_WINDOW_Y1;
  Win.ymax = GIZA_DEFAULT_WINDOW_Y2;

}
