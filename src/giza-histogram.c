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
#include <giza.h>

/**
 * Device: giza_histogram
 *
 * Synopsis: Plot a histogram (unbinned)
 *
 * Input:
 *  -n    :- number of sub-panels in x direction
 *  -dat  :- number of sub panels in y direction
 *  -min  :- minimum
 *  -max  :- maximum
 *  -nbin :- number of bins
 *  -flag :- flag to indicate page changes, see below
 *
 * See also: giza_histogram_binned, giza_histogram_float
 *
 */
void
giza_histogram (int n, const double *dat, double min, double max, int nbin, int flag)
{
  if (!_giza_check_device_ready ("giza_histogram"))
    return;  
 
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
 *  -nbin   :- number of bins
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
 
}
