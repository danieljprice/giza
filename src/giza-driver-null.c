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
 * Copyright (C) 2010 James Wetter. All rights reserved.
 * Contact: wetter.j@gmail.com
 *
 */

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-drivers-private.h"
#include "giza-driver-null-private.h"
#include <giza.h>

#define GIZA_DEFAULT_WIDTH 800
#define GIZA_DEFAULT_HEIGHT 600
#define GIZA_DEVICE_UNITS_PER_MM 3.7054 /* same as png driver */
#define GIZA_DEVICE_INTERACTIVE 0
#define GIZA_DEVICE_EXTENSION ".null"

/**
 * Creates a null device to be drawn to.
 *
 * The routine does the following:
 *  -Set variables in the GIZA_Device structure.
 *  -Creates the cairo image surface.
 */
int
_giza_open_device_null (void)
{
  Dev.deviceUnitsPermm = GIZA_DEVICE_UNITS_PER_MM;
  Dev.isInteractive    = GIZA_DEVICE_INTERACTIVE;

  if (_giza_sizeSpecified ())
    {  
      Dev.width  = (int) (Dev.deviceUnitsPermm * 10. * Dev.widthCM) + 1;
      Dev.height = (int) (Dev.deviceUnitsPermm * 10. * Dev.heightCM) + 1;
    }
  else
    {
      Dev.width  = GIZA_DEFAULT_WIDTH;
      Dev.height = GIZA_DEFAULT_HEIGHT;
    }

  surface =
    cairo_image_surface_create (CAIRO_FORMAT_ARGB32, (double) Dev.width,
				(double) Dev.height);
  if (!surface)
    {
      _giza_error ("_giza_open_device_null", "Could not create cairo surface");
      return -1;
    }
  return 0;
}

void
_giza_flush_device_null (void)
{
  cairo_surface_flush (surface);
}

/**
 * Advances the null device to its 'next page'.
 *
 */
void
_giza_change_page_null (void)
{
  giza_draw_background ();
}

/**
 * Closes an open "null" device.
 * Just destroys the drawing surface
 */
void
_giza_close_device_null (void)
{
  cairo_surface_destroy (surface);
}
