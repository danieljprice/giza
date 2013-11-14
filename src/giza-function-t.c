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
 * Drawing: giza_function_t
 *
 * Synopsis: Draw a curve defined by x = func(t), y = func(t), where
 * funcx(t) and funcy(t) are user-supplied routines
 * with a single double argument passed by reference, e.g.
 * double myfuncx(double* t)
 * double myfuncy(double* t)
 *
 * Input:
 *   -funcx  :- The parametrisation of the x value.
 *   -funcy  :- The parametrisation of the y value.
 *   -n      :- The number of segments to use to approximate the curve.
 *   -tmin   :- The lower bound on the domain of t.
 *   -tmax   :- The upper bound on the domain of t.
 *
 */
void
giza_function_t (double (*funcx)(double *t), double (*funcy)(double *t),
                 int n, double tmin, double tmax, int flag)
{
  if (!_giza_check_device_ready ("giza_function_parametric"))
    return;

  if (n < 1) return;
  double tpt,xpt,ypt,dt,xpt0,ypt0;
  int i;
  dt = (tmax - tmin)/((double) n);
  xpt0 = funcx(&tmin);
  ypt0 = funcy(&tmin);

  if (flag==0)  /* Find min/max in x,y automatically */
     {          /* and call giza_environment */
       double xmin = xpt0;
       double xmax = xpt0;
       double ymin = ypt0;
       double ymax = ypt0;
       for (i = 1; i <= n; i++)
         {
            tpt = tmin + i*dt;
            xpt = funcx(&tpt);
            ypt = funcy(&tpt);
            if (xpt < xmin) xmin = xpt;
            if (xpt > xmax) xmax = xpt;
            if (ypt < ymin) ymin = ypt;
            if (ypt > ymax) ymax = ypt;
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
           xmin -= 0.05*(xmax-xmin);
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

  cairo_move_to(Dev[id].context,xpt0,ypt0);
  for (i = 1; i <= n; i++)
     {
        tpt = tmin + i*dt;
        xpt = funcx(&tpt);
        ypt = funcy(&tpt);
        cairo_line_to(Dev[id].context,xpt,ypt);
     }

  _giza_stroke ();
  _giza_set_trans (oldTrans);
  giza_flush_device ();

}

/**
 * Drawing: giza_function_t_float
 *
 * Synopsis: The same functionality as giza_function_t but uses floats.
 * Draw a curve defined by x = func(y), where
 * func(y) is a user-supplied routine
 * with a single float argument passed by reference, e.g.
 * float myfuncx(float* t)
 * float myfuncy(float* t)
 *
 * See Also: giza_function_t
 *
 */
void
giza_function_t_float (float (*funcx)(float *t), float (*funcy)(float *t),
                      int n, float tmin, float tmax, int flag)
{
  if (!_giza_check_device_ready ("giza_function_parametric_float"))
    return;

  if (n < 1) return;
  float tpt,xpt,ypt,dt,xpt0,ypt0;
  int i;
  dt = (tmax - tmin)/((float) n);
  xpt0 = funcx(&tmin);
  ypt0 = funcy(&tmin);

  if (flag==0)  /* Find min/max in x,y automatically */
     {          /* and call giza_environment */
       float xmin = xpt0;
       float xmax = xpt0;
       float ymin = ypt0;
       float ymax = ypt0;
       for (i = 1; i <= n; i++)
         {
            tpt = tmin + i*dt;
            xpt = funcx(&tpt);
            ypt = funcy(&tpt);
            if (xpt < xmin) xmin = xpt;
            if (xpt > xmax) xmax = xpt;
            if (ypt < ymin) ymin = ypt;
            if (ypt > ymax) ymax = ypt;
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
           xmin -= 0.05f*(xmax-xmin);
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

  cairo_move_to(Dev[id].context,(double) xpt0, (double) ypt0);
  for (i = 1; i <= n; i++)
     {
        tpt = tmin + i*dt;
        xpt = funcx(&tpt);
        ypt = funcy(&tpt);
        cairo_line_to(Dev[id].context,(double) xpt,(double) ypt);
     }

  _giza_stroke ();
  _giza_set_trans (oldTrans);
  giza_flush_device ();

}
