/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2014 Daniel Price
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
#include "giza-window-private.h"
#include "giza-io-private.h"
#include "giza-transforms-private.h"
#include "giza-viewport-private.h"
#include <giza.h>
#include <math.h>

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

  giza_window_t Win;
  Win.xmin = x1;
  Win.xmax = x2;
  Win.ymin = y1;
  Win.ymax = y2;
  Win.normCoords = Dev[id].Win.normCoords;

  /* Transform to normalised device coords */
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_NORM);

  /* Transform from normalised device coords to world coords. */
  /* Scaling */
  double dxWin = (Win.xmax - Win.xmin);
  double dyWin = (Win.ymax - Win.ymin);
  
  double x0,y0,xfac,yfac;
  if (Dev[id].ix > 0 && Dev[id].iy > 0) { /* sanity check */
     x0 = (Dev[id].ix - 1)*Dev[id].panelwidth/Dev[id].width;
     y0 = (Dev[id].ny - Dev[id].iy)*Dev[id].panelheight/Dev[id].height;
  } else {
     x0 = 0.;
     y0 = 0.;
  }
  xfac = Dev[id].panelwidth/Dev[id].width;
  yfac = Dev[id].panelheight/Dev[id].height;
  /*printf(" here nx = %i ny = %i ix = %i iy = %i xfac = %f yfac = %f\n",
         Dev[id].nx,Dev[id].ny,Dev[id].ix,Dev[id].iy,xfac,yfac);*/
  
  double vpxmin = Dev[id].VP.xmin*xfac + x0;
  double vpxmax = Dev[id].VP.xmax*xfac + x0;
  double vpymin = Dev[id].VP.ymin*yfac + y0;
  double vpymax = Dev[id].VP.ymax*yfac + y0;

  double horiScale = (vpxmax - vpxmin) / dxWin; /* this is safe as we have already */
  double vertScale = (vpymax - vpymin) / dyWin; /* checked x1 /= x2 and y1 /= y2   */

  /* Translation: */
  double horiTrans = vpxmin - Win.xmin * horiScale;
  double vertTrans = vpymin - Win.ymin * vertScale;

  cairo_matrix_init (&(Win.userCoords), horiScale, 0, 0, vertScale, horiTrans, vertTrans);
  cairo_matrix_multiply(&(Win.userCoords),&(Win.userCoords),&(Win.normCoords));

  Dev[id].Win = Win;

  _giza_set_trans (oldTrans);
}

/**
 * Settings: giza_set_window_float
 *
 * Synopsis: Same functionality as giza_set_window but uses floats.
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
  double yrange = fabs(y2 - y1);
  double xrange = fabs(x2 - x1);
  double scale, scalex, scaley, newWidth, newHeight;

  /* Scale is Device units per World coords. */
  scaley = (Dev[id].VP.ymax - Dev[id].VP.ymin) * Dev[id].panelheight / yrange;
  scalex = (Dev[id].VP.xmax - Dev[id].VP.xmin) * Dev[id].panelwidth /  xrange;
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
  if (Dev[id].panelwidth > 0) {
     newWidth = scale * xrange / Dev[id].panelwidth;
     Dev[id].VP.xmin = (Dev[id].VP.xmax + Dev[id].VP.xmin - newWidth) * 0.5;
     Dev[id].VP.xmax = Dev[id].VP.xmin + newWidth;
  }

  /* Find the height of the new vp */
  if (Dev[id].panelheight > 0) {
     newHeight = scale * yrange / Dev[id].panelheight;
     Dev[id].VP.ymin = (Dev[id].VP.ymax + Dev[id].VP.ymin - newHeight) * 0.5;
     Dev[id].VP.ymax = Dev[id].VP.ymin + newHeight;
  }

  /* Set the window and vp */
  giza_set_window (x1, x2, y1, y2);
  giza_set_viewport (Dev[id].VP.xmin, Dev[id].VP.xmax, Dev[id].VP.ymin, Dev[id].VP.ymax);
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

  *x1 = Dev[id].Win.xmin;
  *x2 = Dev[id].Win.xmax;
  *y1 = Dev[id].Win.ymin;
  *y2 = Dev[id].Win.ymax;
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

  double x1d, x2d, y1d, y2d;
  giza_get_window(&x1d,&x2d,&y1d,&y2d);
  *x1 = (float) x1d;
  *x2 = (float) x2d;
  *y1 = (float) y1d;
  *y2 = (float) y2d;
}

/**
 * Sets the window to the default (before viewport is set). Must be followed by
 * a call to set viewport, which in turn calls the set_window routine
 */
void
_giza_init_window (void)
{
  if(!_giza_check_device_ready("_giza_init_window")) return;

  Dev[id].Win.xmin = GIZA_DEFAULT_WINDOW_X1;
  Dev[id].Win.xmax = GIZA_DEFAULT_WINDOW_X2;
  Dev[id].Win.ymin = GIZA_DEFAULT_WINDOW_Y1;
  Dev[id].Win.ymax = GIZA_DEFAULT_WINDOW_Y2;

}
