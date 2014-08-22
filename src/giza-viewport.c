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
#include "giza-viewport-private.h"
#include "giza-io-private.h"
#include "giza-transforms-private.h"
#include "giza-window-private.h"
#include <giza.h>
#include <stdio.h>

/**
 * Settings: giza_set_viewport
 *
 * Synopsis: Changes the size and position of the viewport, all arguments are in
 * normalised device coordinates. The viewport is the region of the
 * device that can be drawn to.
 *
 * Input:
 *  -xleft    :- The x coordinate of the left edge of the viewport
 *  -xright   :- The x coordinate of the right edge of the viewport
 *  -ybottom  :- The y coordinate of the bottom edge of the viewport
 *  -ytop     :- The y coordinate of the top edge of the viewport
 */
void
giza_set_viewport (double xleft, double xright, double ybottom, double ytop)
{
  double xmin, xmax, ymin, ymax;
  if (!_giza_check_device_ready ("giza_set_viewport"))
    return;

  if (_giza_equal(xleft,xright) || _giza_equal(ybottom,ytop))
    {
      printf("giza_viewport: xmin %f xmax %f ymin %f ymax %f \n",xleft,xright,ybottom,ytop);
      _giza_warning ("giza_set_viewport", "Invalid arguments, using default viewport");

      _giza_get_default_viewport(&Dev[id].VP.xmin,&Dev[id].VP.xmax,&Dev[id].VP.ymin,&Dev[id].VP.ymax);

      xmin = Dev[id].VP.xmin;
      xmax = Dev[id].VP.xmax;
      ymin = Dev[id].VP.ymin;
      ymax = Dev[id].VP.ymax;

    } else {

      if (xleft > xright)
        {
          xmin = xright;
          xmax = xleft;
        } else {
          xmin = xleft;
          xmax = xright;
        }

      if (ybottom > ytop)
        {
          ymin = ytop;
          ymax = ybottom;
        } else {
          ymin = ybottom;
          ymax = ytop;
        }

      Dev[id].VP.xmin = xmin;
      Dev[id].VP.xmax = xmax;
      Dev[id].VP.ymin = ymin;
      Dev[id].VP.ymax = ymax;

    }

  _giza_set_trans (GIZA_TRANS_NORM);
  cairo_reset_clip (Dev[id].context);
  
  double x0 = (Dev[id].ix - 1)*Dev[id].panelwidth/Dev[id].width;
  double y0 = (Dev[id].ny - Dev[id].iy)*Dev[id].panelheight/Dev[id].height;
  double xfac = Dev[id].panelwidth/Dev[id].width;
  double yfac = Dev[id].panelheight/Dev[id].height;
  
  cairo_rectangle (Dev[id].context, xmin*xfac + x0, ymin*yfac + y0, (xmax - xmin)*xfac, (ymax - ymin)*yfac);

  int clip;
  giza_get_clipping(&clip);
  if (clip)
    cairo_clip (Dev[id].context);

  giza_set_window(Dev[id].Win.xmin,Dev[id].Win.xmax,
                  Dev[id].Win.ymin,Dev[id].Win.ymax);

}

/**
 * Settings: giza_set_viewport_float
 *
 * Synopsis: Same functionality as giza_set_viewport but takes floats
 *
 * See Also: giza_set_viewport
 */
void
giza_set_viewport_float (float xleft, float xright, float ybottom, float ytop)
{
  giza_set_viewport ((double) xleft, (double) xright, (double) ybottom, (double) ytop);
}

/**
 * Settings: giza_get_viewport
 *
 * Synopsis: Returns the viewport size and position.
 *
 * Input:
 *  -units :- specify the units to return values in.
 *  -x1    :- set to x-coord of upper-left corner
 *  -x2    :- set to x-coord of lower-right corner
 *  -y1    :- set to y-coord of upper-left corner
 *  -y2    :- set to y-coord of lower-right corner
 *
 * Units:
 *  -0 or GIZA_UNITS_NORMALIZED :- normalised device units
 *  -1 or GIZA_UNITS_INCHES     :- inches
 *  -2 or GIZA_UNITS_MM         :- mm
 *  -3 or GIZA_UNITS_PIXELS     :- pixels
 *  -5 or GIZA_UNITS_DEVICE     :- device units (pixels or points)
 *  -default                    :- normalised device units
 */
void
giza_get_viewport (int units, double *x1, double *x2, double *y1, double *y2)
{
  if(!_giza_check_device_ready("giza_get_viewport")) return;

  *x1 = (Dev[id].VP.xmin);
  *x2 = (Dev[id].VP.xmax);
  *y1 = (Dev[id].VP.ymin);
  *y2 = (Dev[id].VP.ymax);
  double ymin = *y1;
  double ymax = *y2;

  int oldTrans = _giza_get_trans ();

  switch (units)
    {
    case GIZA_UNITS_NORMALIZED:
      break;
    case GIZA_UNITS_MM:
    case GIZA_UNITS_INCHES:
    case GIZA_UNITS_DEVICE:
    case GIZA_UNITS_PIXELS:
      _giza_set_trans (GIZA_TRANS_NORM);
      cairo_user_to_device (Dev[id].context, x1, &ymin);
      cairo_user_to_device (Dev[id].context, x2, &ymax);
      /* y direction is done differently, because
         cairo counts 0,0 as the top left of the
         pixmap in device units, whereas we want it to be the
         bottom left, hence ymax->ymin and ymin->ymax
         (so our "device units" are slightly different
          to those of the cairo device) */
      *y1 = ymax;
      *y2 = ymin;
      break;
    default:
      _giza_warning ("giza_get_viewport", "Invalid units, using normalised device units.");
      break;
    }

   /* Now convert to mm/inches */
   switch (units)
   {
   case GIZA_UNITS_PIXELS:
      *x1 = *x1 / Dev[id].deviceUnitsPerPixel;
      *x2 = *x2 / Dev[id].deviceUnitsPerPixel;
      *y1 = *y1 / Dev[id].deviceUnitsPerPixel;
      *y2 = *y2 / Dev[id].deviceUnitsPerPixel;
      break;
   case GIZA_UNITS_MM:
      *x1 = *x1 / Dev[id].deviceUnitsPermm;
      *x2 = *x2 / Dev[id].deviceUnitsPermm;
      *y1 = *y1 / Dev[id].deviceUnitsPermm;
      *y2 = *y2 / Dev[id].deviceUnitsPermm;
      break;
   case GIZA_UNITS_INCHES:
      *x1 = *x1 / Dev[id].deviceUnitsPermm / 25.4;
      *x2 = *x2 / Dev[id].deviceUnitsPermm / 25.4;
      *y1 = *y1 / Dev[id].deviceUnitsPermm / 25.4;
      *y2 = *y2 / Dev[id].deviceUnitsPermm / 25.4;
      break;
   default:
     break;
   }

  _giza_set_trans (oldTrans);
}

/**
 * Settings: giza_get_viewport_float
 *
 * Synopsis: Same functionality as giza_get_viewport but uses floats
 *
 * See Also: giza_get_viewport
 */
void
giza_get_viewport_float (int units, float *x1, float *x2, float *y1, float *y2)
{
  double dx1, dx2, dy1,dy2;
  giza_get_viewport (units, &dx1, &dx2, &dy1, &dy2);
  *x1 = (float) dx1;
  *x2 = (float) dx2;
  *y1 = (float) dy1;
  *y2 = (float) dy2;
}

/**
 * Settings: giza_set_viewport_default
 *
 * Synopsis: Sets the viewport to the default settings
 *
 * See Also: giza_set_viewport
 * 
 */
void
giza_set_viewport_default (void)
{
  if(!_giza_check_device_ready("giza_set_viewport_default")) return;

  double xmin, xmax, ymin, ymax;
  _giza_get_default_viewport(&xmin,&xmax,&ymin,&ymax);
  giza_set_viewport(xmin,xmax,ymin,ymax);
}

/**
 * Internal routine returning the default viewport size
 */
void
_giza_get_default_viewport (double *xmin, double *xmax, double *ymin, double *ymax)
{
  double xch, ych;
  giza_get_character_size(GIZA_UNITS_NORMALIZED, &xch, &ych);
  *xmin = 4.*ych;
  *xmax = *xmin + (1. - 8.*ych);
  *ymin = 4.*ych;
  *ymax = *ymin + (1. - 8.*ych);

}

/**
 * Settings: giza_set_viewport_inches
 *
 * Synopsis: Sets the viewport size in inches
 *
 * See Also: giza_set_viewport
 * 
 */
void
giza_set_viewport_inches (double xleftin, double xrightin, double ybottomin, double ytopin)
{
  double xleft,xright,ybottom,ytop;
  double widthmm, heightmm;
  
  widthmm  = Dev[id].width  / Dev[id].deviceUnitsPermm;
  heightmm = Dev[id].height / Dev[id].deviceUnitsPermm;
  
  xleft   = xleftin   * 25.4 / widthmm;
  xright  = xrightin  * 25.4 / widthmm;
  ybottom = ybottomin * 25.4 / heightmm;
  ytop    = ytopin    * 25.4 / heightmm;

  giza_set_viewport(xleft,xright,ybottom,ytop);

}

/**
 * Settings: giza_set_viewport_inches_float
 *
 * Synopsis: Same as giza_set_viewport_inches but takes floats
 *
 * See Also: giza_set_viewport_inches
 * 
 */
void
giza_set_viewport_inches_float (float xleftin, float xrightin, float ybottomin, float ytopin)
{
  giza_set_viewport_inches ((double) xleftin, (double) xrightin, (double) ybottomin, (double) ytopin);
}
