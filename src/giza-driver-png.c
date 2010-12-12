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
#include "giza-drivers-private.h"
#include "giza-io-private.h"
#include "giza-driver-png-private.h"
#include <giza.h>
#include <stdio.h>
#include <string.h>

#define GIZA_DEFAULT_WIDTH 800
#define GIZA_DEFAULT_HEIGHT 600
#define GIZA_DEVICE_UNITS_PER_MM 3.7054 // so "width" is 8.5 inches as in ps driver
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
_giza_open_device_png (void)
{
  Dev.deviceUnitsPermm = GIZA_DEVICE_UNITS_PER_MM;
  Dev.isInteractive    = GIZA_DEVICE_INTERACTIVE;

  if(_giza_sizeSpecified ())
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
    cairo_image_surface_create (CAIRO_FORMAT_ARGB32, Dev.width,
				Dev.height);
  if (!surface)
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
  cairo_surface_flush (surface);
}

/**
 * Saves the current png to file and opens a new one.
 */
void
_giza_change_page_png (void)
{
  int length;
  length = strlen (Dev.prefix) + strlen (GIZA_DEVICE_EXTENSION) + 5;
  char fileName[length + 1];
  sprintf (fileName, "%s_%04d%s", Dev.prefix, Dev.pgNum, GIZA_DEVICE_EXTENSION);
  cairo_surface_write_to_png (surface, fileName);
  char tmp[length + 10];
  sprintf(tmp, "%s created", fileName);
  _giza_draw_background_png ();
  _giza_message (tmp);
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
_giza_close_device_png (void)
{
  int length;
  length = strlen (Dev.prefix) + strlen (GIZA_DEVICE_EXTENSION) + 5;
  char fileName[length + 1];
  if (Dev.pgNum == 0)
    {
      sprintf (fileName, "%s%s", Dev.prefix, GIZA_DEVICE_EXTENSION);
    }
  else
    {
      sprintf (fileName, "%s_%04d%s", Dev.prefix, Dev.pgNum, GIZA_DEVICE_EXTENSION);
    }
  cairo_surface_write_to_png (surface, fileName);

  char tmp[length + 10];
  sprintf(tmp, "%s created", fileName);
  _giza_message (tmp);

  cairo_surface_destroy (surface);
}

/**
 * Redraws the background.
 */
void
_giza_draw_background_png (void)
{
  cairo_save (context);
  cairo_reset_clip (context);
  giza_set_colour_index (0);
  cairo_paint (context);
  cairo_restore (context);
}
