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
#include "giza-io-private.h"
#include "giza-drivers-private.h"
#include <giza.h>

/**
 * Change the paper size (physical size of the view surface)
 *  in a variety of units, after the device has been opened
 *
 * Input:
 *  -width  :- Width of the device surface
 *  -height :- Aspect ratio of the device surface
 *  -units  :- Units in which the width/height are given
 *
 */
void
giza_set_paper_size (int units, double width, double height)
{
  if (!_giza_check_device_ready ("giza_set_paper_size"))
    return;

  if (width <= 0.)
    {
      _giza_error("giza_set_paper_size","width <= 0");
      width = 8.;
    }
  if (height <= 0.)
    {
      _giza_error("giza_set_paper_size","height <= 0");
      height = 6.;
    }

  _giza_get_specified_size(width, height, units,
                           &Dev[id].width, &Dev[id].height);
  
  /* reset panel size after paper size change */
  Dev[id].panelwidth  = Dev[id].width/Dev[id].nx;
  Dev[id].panelheight = Dev[id].height/Dev[id].ny;

  Dev[id].resize = 1;
  giza_change_page();
  /*giza_flush_device (); */
}
/**
 * Same as giza_get_paper_size but takes floats
 */
void
giza_set_paper_size_float (int units, float width, float height)
{
  giza_set_paper_size(units, (double) width, (double) height);
}

/**
 * Internal routine returning the specified paper size
 *  in device units. This is called AFTER the device has been opened.
 *
 * Input:
 *  -paperwidth  :- desired width in specified units
 *  -paperheight :- desired height in specified units
 *  -units  :- units in which paperwidth/paperheight are specified
 * Output:
 *  -width  :- Width of the specified surface size in device units
 *  -height :- Height of the specified surface size in device units
 *
 */
void _giza_get_specified_size(double paperwidth, double paperheight, int units,
                              int *width, int *height)
{
  switch(units)
     {
     case GIZA_UNITS_MM:
       *width  = _giza_nint(paperwidth  * Dev[id].deviceUnitsPermm);
       *height = _giza_nint(paperheight * Dev[id].deviceUnitsPermm);
       break;
     case GIZA_UNITS_INCHES:
       *width  = _giza_nint(paperwidth  * Dev[id].deviceUnitsPermm * 25.4);
       *height = _giza_nint(paperheight * Dev[id].deviceUnitsPermm * 25.4);
       break;
     case GIZA_UNITS_PIXELS:
       *width  = _giza_nint(paperwidth  * Dev[id].deviceUnitsPerPixel);
       *height = _giza_nint(paperheight * Dev[id].deviceUnitsPerPixel);
       break;
     case GIZA_UNITS_DEVICE:
       *width  = _giza_nint(paperwidth);
       *height = _giza_nint(paperheight);
       break;
     default:
       *width  = _giza_nint(paperwidth  * Dev[id].deviceUnitsPermm * 10.);
       *height = _giza_nint(paperheight * Dev[id].deviceUnitsPermm * 10.);
       break;
     }
   /*
      printf("size was %f x %f in units %i, actual size = %i x %i \n",
            paperwidth,paperheight,paperunits,*width,*height);
    */
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
 *  -3 or GIZA_UNITS_PIXELS     :- pixels
 *  -5 or GIZA_UNITS_DEVICE     :- device units (pixels or points)
 *  -default                    :- cm
 */
void
giza_get_paper_size (int units, double *width, double *height)
{
  if (!_giza_check_device_ready ("giza_get_paper_size"))
    {
      *width  = 1.;
      *height = 1.;
      return;
    }

  switch(units)
     {
     case GIZA_UNITS_MM:
       *width  = ((double) Dev[id].width ) / Dev[id].deviceUnitsPermm;
       *height = ((double) Dev[id].height) / Dev[id].deviceUnitsPermm;
       break;
     case GIZA_UNITS_INCHES:
       *width  = (((double) Dev[id].width ) / Dev[id].deviceUnitsPermm) / 25.4;
       *height = (((double) Dev[id].height) / Dev[id].deviceUnitsPermm) / 25.4;
       break;
     case GIZA_UNITS_NORMALIZED:
       *width  = 1.;
       *height = 1.;
       break;
     case GIZA_UNITS_PIXELS:
       *width  = ((double) Dev[id].width ) / Dev[id].deviceUnitsPerPixel;
       *height = ((double) Dev[id].height) / Dev[id].deviceUnitsPerPixel;
       break;
     case GIZA_UNITS_DEVICE:
       *width  = (double) Dev[id].width;
       *height = (double) Dev[id].height;
       break;
     default:
       *width  = 0.1 * ((double) Dev[id].width ) / Dev[id].deviceUnitsPermm;
       *height = 0.1 * ((double) Dev[id].height) / Dev[id].deviceUnitsPermm;
       break;
     }
}

void
giza_get_paper_size_float (int units, float *width, float *height)
{
  double dwidth, dheight;
  giza_get_paper_size (units, &dwidth, &dheight);
  *width  = (float) dwidth;
  *height = (float) dheight;
}
