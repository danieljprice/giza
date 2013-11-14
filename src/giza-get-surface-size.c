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
#include <giza.h>

/**
 * Device: giza_get_surface_size
 *
 * Synopsis: Gets the size of the current surface that can be drawn to in device
 * units (pixels or points).
 *
 * Input:
 *  -x1 :- Always gets set to 0.0
 *  -x2 :- Gets set to the width of the current surface
 *  -y1 :- Always gets set to 0.0
 *  -y2 :- Gets set to the width of the surface
 */
void
giza_get_surface_size (double *x1, double *x2, double *y1, double *y2)
{
  if (!_giza_check_device_ready ("giza_get_surface_size"))
    return;

  *x1 = 0.;
  *x2 = Dev[id].width;
  *y1 = 0.;
  *y2 = Dev[id].height;
}

/**
 * Device: giza_get_surface_size_float
 *
 * Synopsis: Same functionality as giza_get_surface_size, but uses floats.
 *
 * See Also: giza_get_surface_size
 */
void
giza_get_surface_size_float (float *x1, float *x2, float *y1, float *y2)
{
  if (!_giza_check_device_ready ("giza_get_surface_size_float"))
    return;

  *x1 = 0.;
  *x2 = (float) Dev[id].width;
  *y1 = 0.;
  *y2 = (float) Dev[id].height;
}
