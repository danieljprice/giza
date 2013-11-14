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
#include "giza-driver-pdf-private.h"
#include <giza.h>
#include <stdio.h>
#include <string.h>
#include <cairo/cairo-pdf.h>

#define GIZA_DEFAULT_WIDTH 612  /* same as ps driver */
#define GIZA_DEFAULT_HEIGHT 459 /* use 4:3 aspect ratio */
#define GIZA_DEVICE_UNITS_PER_MM 2.8346
#define GIZA_DEVICE_UNITS_PER_PIXEL 0.765 /* so pixel resolution is same as pixel devices */
#define GIZA_DEVICE_INTERACTIVE 0
#define GIZA_DEVICE_EXTENSION ".pdf"

/**
 * Opens a pdf device for drawing to.
 *
 * The routine does the following:
 *  -Sets the variables in the GIZA_Device structure Dev
 *  -Creates the cairo pdf surface, which is associated with a file
 *
 * Input:
 *  -vert :- if set to one the created surface is portrait
 */
int
_giza_open_device_pdf (double width, double height, int units, int vert)
{
  int lenext = strlen (GIZA_DEVICE_EXTENSION);
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

  Dev[id].surface = cairo_pdf_surface_create (fileName, Dev[id].width, Dev[id].height);

  if (!Dev[id].surface)
    {
      _giza_error ("giza_open_device_pdf", "Could not create cairo surface");
      return -1;
    }
  return 0;
}

/**
 * Flushes the pdf surface.
 */
void
_giza_flush_device_pdf (void)
{
  cairo_surface_flush (Dev[id].surface);
}

/**
 * Advances the pdf device to its next page.
 */
void
_giza_change_page_pdf (void)
{
  cairo_show_page (Dev[id].context);
}

/**
 * Closes the pdf device.
 *
 * Destroys the cairo surface freeing the associated memory
 */
void
_giza_close_device_pdf (void)
{
  cairo_surface_finish (Dev[id].surface);
  cairo_status_t status = cairo_surface_status (Dev[id].surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_close_device_pdf",cairo_status_to_string(status));
  cairo_surface_destroy (Dev[id].surface);
}
