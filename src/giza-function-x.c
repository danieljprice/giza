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
 * Drawing: giza_function_x
 *
 * Synopsis: Draw a curve defined by y = func(x), where
 * func(x) is a user-supplied routine
 * with a single double argument, e.g.
 * double myfunc(double* x)
 *
 * Input:
 *  -func  :- The function describing y's dependence on x.
 *  -n     :- The number of segments with whichh to approximate the curve
 *  -xmin  :- The lower bound on the domain of x.
 *  -xmax  :- The upper bound on the domain of x.
 *
 */
void
giza_function_x (double (*func)(double *x), int n, double xmin, double xmax, int flag)
{
  if (!_giza_check_device_ready ("giza_function_x"))
    return;

  if (n < 1) return;
  double xpt,ypt,dx,ypt0;
  int i;
  dx   = (xmax - xmin)/((double) n);
  ypt0 = func(&xmin);

  if (flag==0)  /* Find min/max in y automatically */
     {          /* and call giza_environment */
       double ymin = ypt0;
       double ymax = ypt0;
       for (i = 1; i <= n; i++)
         {
            xpt = xmin + i*dx;
            ypt = func(&xpt);
            if (ypt < ymin) ymin = ypt;
            if (ypt > ymax) ymax = ypt;
         }
       double dylim = ymax - ymin;
       if (_giza_equal(dylim,0.))
         {
           ymax += 1.;
           ymin -= 1.;
         }
       else
         {
           ymax += 0.05*(ymax-ymin);
           ymin -= 0.05*(ymax-ymin);
         }
       giza_set_environment(xmin,xmax,ymin,ymax,0,0);
     }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  cairo_move_to(Dev[id].context,xmin,ypt0);
  for (i = 1; i <= n; i++)
     {
        xpt = xmin + i*dx;
        ypt = func(&xpt);
        cairo_line_to(Dev[id].context,xpt,ypt);
     }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}

/**
 * Drawing: giza_function_x_float
 *
 * Synopsis: Same functionality as giza_function_x but takes floats.
 * Draw a curve defined by y = func(x), where
 * func(x) is a user-supplied routine
 * with a single double argument, e.g.
 * float myfunc(float* x)
 *
 * See Also: giza_function_x
 *
 */
void
giza_function_x_float (float (*func)(float *x), int n, float xmin, float xmax, int flag)
{
  if (!_giza_check_device_ready ("giza_function_x_float"))
    return;

  if (n < 1) return;
  float xpt,ypt,dx,ypt0;
  int i;
  dx   = (xmax - xmin)/((float) n);
  ypt0 = func(&xmin);

  if (flag==0)  /* Find min/max in y automatically */
     {          /* and call giza_environment */
       float ymin = ypt0;
       float ymax = ypt0;
       for (i = 1; i <= n; i++)
         {
            xpt = xmin + i*dx;
            ypt = func(&xpt);
            if (ypt < ymin) ymin = ypt;
            if (ypt > ymax) ymax = ypt;
         }
       float dylim = ymax - ymin;
       if (_giza_equal(dylim,0.))
         {
           ymax += 1.f;
           ymin -= 1.f;
         }
       else
         {
           ymax += 0.05f*(ymax-ymin);
           ymin -= 0.05f*(ymax-ymin);
         }
       giza_set_environment_float(xmin,xmax,ymin,ymax,0,0);
     }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  cairo_move_to(Dev[id].context,(double) xmin,(double) ypt0);
  for (i = 1; i <= n; i++)
     {
        xpt = xmin + i*dx;
        ypt = func(&xpt);
        cairo_line_to(Dev[id].context,(double) xpt,(double) ypt);
     }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  giza_flush_device ();
}
