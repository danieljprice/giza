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
#include "giza-transforms-private.h"
#include "giza-fill-private.h"
#include <giza.h>

/**
 * Device: giza_histogram
 *
 * Synopsis: Plot a histogram (unbinned)
 *
 * Input:
 *  -n    :- number of data values
 *  -dat  :- data (n values)
 *  -min  :- minimum data value to use in histogram
 *  -max  :- maximum data value to use in histogram
 *  -nbin :- number of bins
 *  -flag :- flag to indicate page changes, see below
 *
 * See also: giza_histogram_binned, giza_histogram_float
 *
 *  Flag:
 *   -0   :- giza_histogram calls giza_environment to set up new plotting page
 *   -1   :- plot in current window and viewport
 *   -2   :- with filled area style, calls giza_environment
 *   -3   :- with filled area style, in current window and viewport
 *   -4   :- bins drawn as simple outlines, as in giza_histogram_binned, calls giza_environment
 *   -5   :- bins drawn as simple outlines, but in current window and viewport
 *  Other values treated as 1
 *
 */
void
giza_histogram (int n, const double *dat, double min, double max, int nbin, int flag)
{
  if (!_giza_check_device_ready ("giza_histogram"))
    return;

  int i,ibin;
  int nmax = 0;
  double bin_width = (max - min)/((double) nbin);
  int ninbin[nbin];
  for (ibin=0;ibin<nbin;ibin++)
    {
      ninbin[ibin] = 0;
    }
  
  for (i=0;i<n;i++) 
    {
      ibin = (int) ((dat[i] - min)/bin_width);
      if (ibin >= 0 && ibin < nbin)
        {
          ninbin[ibin] += 1;
          if (ninbin[ibin] > nmax) nmax = ninbin[ibin];
        }
    }

  double xmin,xmax,ymin,ymax;
  int nval,oldFill;
  ymax = nmax;
  ymax = giza_round(1.01*ymax, &nval);
  giza_get_fill(&oldFill);
  
  if (flag % 2 == 0) giza_set_environment(min,max,0.,(double) ymax,0,0);
  
  /* set up plotting environment and options */
  switch (flag) 
    {
    case 5:
    default:
      giza_set_fill(GIZA_FILL_HOLLOW);
      break;
    case 3:
      giza_set_fill(GIZA_FILL_SOLID);
      break;
    }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  /* plot the bars of the histogram */
  for (ibin=0;ibin<nbin;ibin++) 
    {
       xmin = min + ibin*bin_width;
       xmax = xmin + bin_width;
       ymin = 0.;
       ymax = (double) ninbin[ibin];
       
       /* plot only 3 sides of the rectangle for all except the last */
       cairo_move_to (Dev[id].context, xmin, ymin);
       cairo_line_to (Dev[id].context, xmin, ymax);
       cairo_line_to (Dev[id].context, xmax, ymax);
       cairo_line_to (Dev[id].context, xmax, ymin);
       cairo_line_to (Dev[id].context, xmin, ymin);
       _giza_fill ();
    }

  /* restore previous fill style */
  giza_set_fill(oldFill);  
  _giza_set_trans (oldTrans);
}

/**
 * Device: giza_histogram_float
 *
 * Synopsis: Same as giza_histogram but takes floats
 *
 * See also: giza_histogram, giza_histogram_binned_float
 *
 */
void
giza_histogram_float (int n, const float *dat, float min, float max, int nbin, int flag)
{
  if (!_giza_check_device_ready ("giza_histogram"))
    return;

  double ddat[n];
  int i;
  for (i=0;i<n;i++) {
      ddat[i] = (double) dat[i];
  }
  giza_histogram(n, ddat, (double) min, (double) max, nbin, flag);
 
}

/**
 * Device: giza_histogram_binned
 *
 * Synopsis: Plot a histogram of already binned data
 *
 * Input:
 *  -n      :- number of bins
 *  -x      :- x values of bins
 *  -dat    :- data values for each bin
 *  -center :- if true (1) x values correspond to centre of each bin
 *
 * See also: giza_histogram, giza_histogram_binned_float
 *
 */
void
giza_histogram_binned (int n, const double *x, const double *dat, int centre)
{
  if (!_giza_check_device_ready ("giza_histogram_binned"))
    return;

  if (n < 2) return;

  int i;
  double xmin, xmax, ymin, ymax;
  double offset = (double) centre;
  double dx;

  /* plot the bars of the histogram */
  for (i=0;i<n;i++) 
    {
       if (i==n-1) {
         dx = x[i] - x[i-1];
       } else {
         dx = x[i+1] - x[i];
       }
       xmin = x[i] - 0.5*offset*dx;
       xmax = xmin + dx;
       ymin = 0.;
       ymax = dat[i];

       giza_rectangle(xmin,xmax,ymin,ymax);
    }

}

/**
 * Device: giza_histogram_binned_float
 *
 * Synopsis: Same as giza_histogram_binned but takes floats
 *
 * See also: giza_histogram, giza_histogram_binned_float
 *
 */
void
giza_histogram_binned_float (int n, const float *x, const float *dat, int centre)
{
  if (!_giza_check_device_ready ("giza_histogram_binned"))
    return;

  /* convert arrays to double precision */
  double ddat[n],dx[n];
  int i;
  for (i=0;i<n;i++) {
      ddat[i] = (double) dat[i];
      dx[i]   = (double) x[i];
  }

  /* call the usual double precision routine */
  giza_histogram_binned(n, dx, ddat, centre);

}
