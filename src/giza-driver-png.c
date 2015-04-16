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
#include "giza-drivers-private.h"
#include "giza-io-private.h"
#include "giza-driver-png-private.h"
#include <giza.h>
#include <stdio.h>
#include <string.h>

#define GIZA_DEFAULT_WIDTH 800
#define GIZA_DEFAULT_HEIGHT 600
#define GIZA_DEVICE_UNITS_PER_MM 3.7054 /* so "width" is 8.5 inches as in ps driver */
#define GIZA_DEVICE_UNITS_PER_PIXEL 1.0 /* device units are pixels */
#define GIZA_DEVICE_INTERACTIVE 0
#define GIZA_DEVICE_EXTENSION ".png"

/**
 * Creates a png device to be drawn to.
 *
 * The routine does the following:
 *  -Set variables in the GIZA_Device structure.
 *  -Creates the cairo image surface.
 */
int
_giza_open_device_png (double width, double height, int units)
{
  Dev[id].deviceUnitsPermm    = GIZA_DEVICE_UNITS_PER_MM;
  Dev[id].deviceUnitsPerPixel = GIZA_DEVICE_UNITS_PER_PIXEL;
  Dev[id].isInteractive       = GIZA_DEVICE_INTERACTIVE;

  if (width > 0. && height > 0. && units > 0)
    {
      _giza_get_specified_size(width,height,units,&Dev[id].width, &Dev[id].height);
    }
  else
    {
      Dev[id].width  = GIZA_DEFAULT_WIDTH;
      Dev[id].height = GIZA_DEFAULT_HEIGHT;
    }

  Dev[id].surface =
    cairo_image_surface_create (CAIRO_FORMAT_ARGB32, Dev[id].width,
				Dev[id].height);
  if (!Dev[id].surface)
    {
      _giza_error ("_giza_open_device_png", "Could not create cairo surface");
      return -1;
    }
  return 0;
}

/**
 * Flushes the png surface.
 */
void
_giza_flush_device_png (void)
{
  cairo_surface_flush (Dev[id].surface);
}

/**
 * Saves the current png to file and opens a new one.
 */
void
_giza_change_page_png (void)
{
  /* Destroy old context */
  cairo_destroy (Dev[id].context);

  /* Write current surface to png file and destroy the cairo surface */
  _giza_close_device_png(0);

  /* Create a new surface */
  Dev[id].surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, Dev[id].width, Dev[id].height);

  if (!Dev[id].surface)
    {
      _giza_error ("_giza_change_page_png", "Could not create cairo PNG surface");
      return;
    }
  Dev[id].context = cairo_create (Dev[id].surface);
  if (!Dev[id].context)
    {
      _giza_error ("_giza_change_page_png", "Could not create cairo context");
      return;
    }
  return;
}

/**
 * Closes an open png device.
 *
 * If only one page has been used the current cairo surface is written
 * to a file with no numeric prefix.
 * If multiple pages have been used the current surface is written
 * to a file prefixed with the current page number.
 */
void
_giza_close_device_png (int last)
{
  int lenext = strlen (GIZA_DEVICE_EXTENSION);
  int length = strlen (Dev[id].prefix) + lenext + 5;
  char fileName[length + 1];
  _giza_get_filename_for_device(fileName,Dev[id].prefix,Dev[id].pgNum,GIZA_DEVICE_EXTENSION,last);

  cairo_status_t status = cairo_surface_write_to_png (Dev[id].surface, fileName);
  if (status != CAIRO_STATUS_SUCCESS)
     {
       _giza_error (fileName, cairo_status_to_string(status));
     }
  else
     {
       char tmp[length + 10];
       sprintf(tmp, "%s created", fileName);
       _giza_message (tmp);
     }

  cairo_surface_destroy (Dev[id].surface);
}

