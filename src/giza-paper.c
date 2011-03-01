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
 * Copyright (C) 2010 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include "giza-private.h"
#include "giza-io-private.h"
#include <giza.h>

/**
 * Set the paper size (physical size of the view surface)
 *
 * Input:
 *  -width  :- Width of the device surface in cm
 *  -aspect :- Aspect ratio of the device surface
 *
 */
void
giza_set_paper_size (double width, double aspect)
{
//  if (!_giza_check_device_ready ("giza_set_paper_size"))
//    return;

  Dev.widthCM = width;
  Dev.heightCM = width * aspect;
  _giza_set_sizeSpecified ();
  //giza_flush_device ();
}

void
giza_set_paper_size_float (float width, float aspect)
{
  giza_set_paper_size ((double) width, (double) aspect);
}


/**
 * Query the paper size (physical size of the view surface)
 * in a variety of units
 *
 * Input:
 *  -units  :- Specify the units to return values in
 *  -width  :- Width of the device surface
 *  -height :- Height of the device surface
 *
 * Units:
 *  -0 or GIZA_UNITS_NORMALIZED :- normalised device units
 *  -1 or GIZA_UNITS_INCHES     :- inches
 *  -2 or GIZA_UNITS_MM         :- mm
 *  -3 or GIZA_UNITS_DEVICE     :- device units (pixels or points)
 *  -default                    :- cm
 */
void
giza_get_paper_size (int units, double *width, double *height)
{
  if (!_giza_check_device_ready ("giza_set_paper_size"))
    return;

  switch(units)
     {
     case GIZA_UNITS_MM:
       *width  = 10.*Dev.widthCM;
       *height = 10.*Dev.heightCM;
       break;
     case GIZA_UNITS_INCHES:
       *width  = Dev.widthCM * 2.54;
       *height = Dev.heightCM * 2.54;
       break;
     case GIZA_UNITS_NORMALIZED:
       *width  = 1.;
       *height = 1.;
       break;
     case GIZA_UNITS_DEVICE:
       *width  = Dev.width;
       *height = Dev.height;
       break;
     default:
       *width  = Dev.widthCM;
       *height = Dev.heightCM;
       break;
     }
}

void
giza_get_paper_size_float (int units, float *width, float *height)
{
  double dwidth, dheight;
  giza_get_paper_size (units, &dwidth, &dheight);
  *width = (float) dwidth;
  *height = (float) dheight;
}
