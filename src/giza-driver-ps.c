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
#include "giza-driver-ps-private.h"
#include <giza.h>
#include <stdio.h>
#include <string.h>
#include <cairo/cairo-ps.h>

/*
 * Note (DJP): it is wise to use 8.5 in x 11 in
 *  or lower as the default size as this is the default
 *  letter page size (e.g. as used by ghostscript)
 *  This means that ps2ps and the like work without
 *  needing fancy arguments.
 */
#define GIZA_DEFAULT_WIDTH 612
#define GIZA_DEFAULT_HEIGHT 459 /* use 4:3 aspect ratio */
#define GIZA_DEVICE_UNITS_PER_MM 2.8346
#define GIZA_DEVICE_UNITS_PER_PIXEL 0.765 /* so pixel resolution is same as pixel devices */
#define GIZA_DEVICE_INTERACTIVE 0
#define GIZA_DEVICE_EXTENSION ".ps"

/**
 * Opens a postscript device for drawing to.
 *
 * The routine does the following:
 *  -Sets variables in the GIZA_Device structure Dev
 *  -Creates the cairo ps surface which is associated with a file
 *
 * Input:
 *  vert :- If set to one the created surface is portrait
 */
int
_giza_open_device_ps (double width, double height, int units, int vert)
{
  int lenext = strlen(GIZA_DEVICE_EXTENSION);
  int length = strlen (Dev[id].prefix) + lenext;
  char fileName[length + 1];
  _giza_get_filename_for_device(fileName,Dev[id].prefix,0,GIZA_DEVICE_EXTENSION,1);

  Dev[id].deviceUnitsPermm    = GIZA_DEVICE_UNITS_PER_MM;
  Dev[id].deviceUnitsPerPixel = GIZA_DEVICE_UNITS_PER_PIXEL;
  Dev[id].isInteractive       = GIZA_DEVICE_INTERACTIVE;
  Dev[id].defaultBackgroundAlpha = 0.;

  /* set all device specific settings */
  if (width > 0. && height > 0. && units > 0)
    {
      _giza_get_specified_size(width,height,units,&Dev[id].width, &Dev[id].height);
    }
  else if (vert)
    {
      Dev[id].height = GIZA_DEFAULT_WIDTH;
      Dev[id].width  = GIZA_DEFAULT_HEIGHT;
    }
  else
    {
      Dev[id].width  = GIZA_DEFAULT_WIDTH;
      Dev[id].height = GIZA_DEFAULT_HEIGHT;
    }

  Dev[id].surface = cairo_ps_surface_create (fileName, Dev[id].width, Dev[id].height);
  cairo_ps_surface_dsc_comment (Dev[id].surface, "%%Title: giza postscript plot");
  cairo_ps_surface_dsc_comment (Dev[id].surface, "%%Copyright: Copyright (C) 2010-2011 Daniel Price and James Wetter");
  
  if (!Dev[id].surface)
    {
      _giza_error ("giza_open_device_ps", "Could not create cairo PS surface");
      return -1;
    }
  return 0;
}

/**
 * Flushes the ps surface.
 */
void
_giza_flush_device_ps (void)
{
  cairo_surface_flush (Dev[id].surface);
}

/**
 * Advances the ps device to its next page.
 */
void
_giza_change_page_ps (void)
{
  cairo_show_page (Dev[id].context);
}

/**
 * Closes the ps device.
 *
 * Destroys the cairo surface freeing the associated memory
 */
void
_giza_close_device_ps (void)
{
  cairo_surface_finish (Dev[id].surface);
  cairo_status_t status = cairo_surface_status (Dev[id].surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_close_device_ps",cairo_status_to_string(status));
  cairo_surface_destroy (Dev[id].surface);
}
