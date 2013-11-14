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
#include "giza-driver-null-private.h"
#include <giza.h>

#define GIZA_DEFAULT_WIDTH 800
#define GIZA_DEFAULT_HEIGHT 600
#define GIZA_DEVICE_UNITS_PER_MM 3.7054 /* same as png driver */
#define GIZA_DEVICE_UNITS_PER_PIXEL 1.0 /* device units are pixels */
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
_giza_open_device_null (double width, double height, int units)
{
  Dev[id].deviceUnitsPermm = GIZA_DEVICE_UNITS_PER_MM;
  Dev[id].isInteractive    = GIZA_DEVICE_INTERACTIVE;

  if (width > 0. && height > 0. && units > 0)
    {
      _giza_get_specified_size(width,height,units,&Dev[id].width, &Dev[id].height);
    }
  else
    {
      Dev[id].width  = GIZA_DEFAULT_WIDTH;
      Dev[id].height = GIZA_DEFAULT_HEIGHT;
    }

  Dev[id].surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, Dev[id].width, Dev[id].height);
  if (!Dev[id].surface)
    {
      _giza_error ("_giza_open_device_null", "Could not create cairo surface");
      return -1;
    }
  return 0;
}

void
_giza_flush_device_null (void)
{
  cairo_surface_flush (Dev[id].surface);
}

/**
 * Advances the null device to its 'next page'.
 *
 */
void
_giza_change_page_null (void)
{
  /* Destroy old context */
  cairo_destroy (Dev[id].context);

  /* Destroy the cairo surface */
  _giza_close_device_null();

  /* Create a new surface */
  Dev[id].surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, Dev[id].width, Dev[id].height);

  if (!Dev[id].surface)
    {
      _giza_error ("_giza_change_page_null", "Could not create cairo surface");
      return;
    }
  Dev[id].context = cairo_create (Dev[id].surface);
  if (!Dev[id].context)
    {
      _giza_error ("_giza_change_page_null", "Could not create cairo context");
      return;
    }
  return;

  giza_draw_background ();
}

/**
 * Closes an open "null" device.
 * Just destroys the drawing surface
 */
void
_giza_close_device_null (void)
{
  cairo_surface_destroy (Dev[id].surface);
}
