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
#include "giza-transforms-private.h"
#include "giza-stroke-private.h"
#include "giza-io-private.h"
#include <giza.h>

/**
 * Draw a curve defined by x = func(y), where
 * func(y) is a user-supplied routine
 * with a single double argument, e.g.
 *
 * double myfunc(double* y)
 *
 */
void
giza_function_y (double (*func)(double *y), int n, double ymin, double ymax, int flag)
{
  if (!_giza_check_device_ready ("giza_function_y"))
    return;

  if (n < 1) return;
  double xpt,ypt,dy,xpt0;
  int i;
  dy   = (ymax - ymin)/((double) n);
  xpt0 = func(&ymin);

  if (flag==0)  /* Find min/max in y automatically */
     {          /* and call giza_environment */
       double xmin = xpt0;
       double xmax = xpt0;
       for (i = 1; i <= n; i++)
         {
            ypt = ymin + i*dy;
            xpt = func(&ypt);
            if (xpt < xmin) xmin = xpt;
            if (xpt > xmax) xmax = xpt;
         }
       double dxlim = xmax - xmin;
       if (_giza_equal(dxlim,0.))
         {
           xmax += 1.;
           xmin -= 1.;
         }
       else
         {
           xmax += 0.05*(xmax-xmin);
           xmax -= 0.05*(xmax-xmin);
         }
       giza_set_environment(xmin,xmax,ymin,ymax,0,0);
     }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  cairo_move_to(Dev[id].context,xpt0,ymin);
  for (i = 1; i <= n; i++)
     {
        ypt = ymin + i*dy;
        xpt = func(&ypt);
        cairo_line_to(Dev[id].context,xpt,ypt);
     }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}

/**
 * Draw a curve defined by x = func(y), where
 * func(y) is a user-supplied routine
 * with a single float argument, e.g.
 *
 * float myfunc(float* y)
 *
 */
void
giza_function_y_float (float (*func)(float *y), int n, float ymin, float ymax, int flag)
{
  if (!_giza_check_device_ready ("giza_function_y_float"))
    return;

  if (n < 1) return;
  float xpt,ypt,dy,xpt0;
  int i;
  dy   = (ymax - ymin)/((float) n);
  xpt0 = func(&ymin);

  if (flag==0)  /* Find min/max in y automatically */
     {        /* and call giza_environment */
       float xmin = xpt0;
       float xmax = xpt0;
       for (i = 1; i <= n; i++)
         {
            ypt = ymin + i*dy;
            xpt = func(&ypt);
            if (xpt < xmin) xmin = xpt;
            if (xpt > xmax) xmax = xpt;
         }
       float dxlim = xmax - xmin;
       if (_giza_equal(dxlim,0.))
         {
           xmax += 1.f;
           xmin -= 1.f;
         }
       else
         {
           xmax += 0.05f*(xmax-xmin);
           xmax -= 0.05f*(xmax-xmin);
         }
       giza_set_environment_float(xmin,xmax,ymin,ymax,0,0);
     }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  cairo_move_to(Dev[id].context,(double) xpt0,(double) ymin);
  for (i = 1; i <= n; i++)
     {
        ypt = ymin + i*dy;
        xpt = func(&ypt);
        cairo_line_to(Dev[id].context,(double) xpt,(double) ypt);
     }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}
