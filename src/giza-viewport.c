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
#include "giza-viewport-private.h"
#include "giza-io-private.h"
#include "giza-transforms-private.h"
#include "giza-window-private.h"
#include <giza.h>
#include <stdio.h>

#define GIZA_DEFAULT_VP_MARGIN_VERT 0.08
#define GIZA_DEFAULT_VP_MARGIN_HORI 0.08

/**
 * Settings: giza_set_viewport
 *
 * Synopsis: Changes the size and position of the viewport, all arguments are in 
 * normalised device coordinates. The viewport is the region of the
 * device that can be drawn to.
 *
 * Input:
 *  -xleft    :- The x coordinate of the left edge of the view port
 *  -xright   :- The x coordinate of the right edge of the view port
 *  -ybottom  :- The y coordinate of the bottom edge of the view port
 *  -ytop     :- The y coordinate of the top edge of the view port
 */
void
giza_set_viewport (double xleft, double xright, double ybottom, double ytop)
{
  if (!_giza_check_device_ready ("giza_set_viewport"))
    return;
  if (xleft >= xright || ybottom >= ytop)
    {
      printf("giza_viewport: xmin %f xmax %f ymin %f ymax %f \n",xleft,xright,ybottom,ytop);
      _giza_warning ("giza_set_viewport", "Invalid arguments, using default viewport");

      VP.xmin = GIZA_DEFAULT_VP_MARGIN_HORI;
      VP.xmax = 1. - GIZA_DEFAULT_VP_MARGIN_HORI;
      VP.ymin = GIZA_DEFAULT_VP_MARGIN_VERT; 
      VP.ymax = 1. - GIZA_DEFAULT_VP_MARGIN_VERT;

    } else {

      VP.xmin = xleft;
      VP.xmax = xright;
      VP.ymin = ybottom;
      VP.ymax = ytop;    

    }

  _giza_set_trans (GIZA_TRANS_NORM);
  cairo_reset_clip (context);
  cairo_rectangle (context, xleft, ybottom, xright - xleft, ytop - ybottom);

  int clip;
  giza_get_clipping(&clip);
  if (clip)
    cairo_clip (context);

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

  *x1 = (VP.xmin);
  *x2 = (VP.xmax);
  *y1 = (VP.ymin);
  *y2 = (VP.ymax);
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
      cairo_user_to_device (context, x1, &ymin);
      cairo_user_to_device (context, x2, &ymax);
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
      *x1 = *x1 / Dev.deviceUnitsPerPixel;
      *x2 = *x2 / Dev.deviceUnitsPerPixel;
      *y1 = *y1 / Dev.deviceUnitsPerPixel;
      *y2 = *y2 / Dev.deviceUnitsPerPixel;
      break;
   case GIZA_UNITS_MM:
      *x1 = *x1 / Dev.deviceUnitsPermm;
      *x2 = *x2 / Dev.deviceUnitsPermm;
      *y1 = *y1 / Dev.deviceUnitsPermm;
      *y2 = *y2 / Dev.deviceUnitsPermm;
      break;
   case GIZA_UNITS_INCHES:
      *x1 = *x1 / Dev.deviceUnitsPermm / 25.4;
      *x2 = *x2 / Dev.deviceUnitsPermm / 25.4;
      *y1 = *y1 / Dev.deviceUnitsPermm / 25.4;
      *y2 = *y2 / Dev.deviceUnitsPermm / 25.4;
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
 * Sets the viewport to the default
 */
void
giza_set_viewport_default (void)
{
  if(!_giza_check_device_ready("giza_set_viewport_default")) return;
  
  double xmin = GIZA_DEFAULT_VP_MARGIN_HORI;
  double xmax = 1. - GIZA_DEFAULT_VP_MARGIN_HORI;
  double ymin = GIZA_DEFAULT_VP_MARGIN_VERT; 
  double ymax = 1. - GIZA_DEFAULT_VP_MARGIN_VERT;
  
  giza_set_viewport(xmin,xmax,ymin,ymax);
  /* also set window if calling default viewport routine */
  /*giza_set_window(GIZA_DEFAULT_WINDOW_X1,GIZA_DEFAULT_WINDOW_X2, 
                  GIZA_DEFAULT_WINDOW_Y1,GIZA_DEFAULT_WINDOW_Y2);
  */

}

/**
 * Sets the viewport in inches (this is for PGPLOT-compatibility)
 */
void
giza_set_viewport_inches (double xleftin, double xrightin, double ybottomin, double ytopin)
{
  double xleft,xright,ybottom,ytop;
  xleft   = xleftin   * 2.54 / Dev.widthCM;
  xright  = xrightin  * 2.54 / Dev.widthCM;
  ybottom = ybottomin * 2.54 / Dev.heightCM;
  ytop    = ytopin    * 2.54 / Dev.heightCM;
  
  giza_set_viewport(xleft,xright,ybottom,ytop);

}

void
giza_set_viewport_inches_float (float xleftin, float xrightin, float ybottomin, float ytopin)
{
  giza_set_viewport_inches ((double) xleftin, (double) xrightin, (double) ybottomin, (double) ytopin);
}
