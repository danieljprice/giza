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
#include "giza-driver-svg-private.h"
#include "giza-drivers-private.h"

#include <giza.h>
#include <string.h>
#include <stdio.h>
#include <cairo/cairo-svg.h>

#define GIZA_DEFAULT_WIDTH 612
#define GIZA_DEFAULT_HEIGHT 459 /* use 4:3 aspect ratio */
#define GIZA_DEVICE_UNITS_PER_MM 2.8346
#define GIZA_DEVICE_UNITS_PER_PIXEL 0.765 /* so pixel resolution is same as pixel devices */
#define GIZA_DEVICE_INTERACTIVE 0
#define GIZA_DEVICE_EXTENSION ".svg"

/**
 * Opens an svg device for drawing to.
 *
 * The routine does the following:
 *  -Sets variables in the GIZA_Device structure Dev
 *  -Creates the cairo svg surface which is associated with a file
 *
 * Input:
 *  vert :- If set to one the created surface is portrait
 */
int
_giza_open_device_svg (double width, double height, int units, int vert)
{
  int lenext = strlen (GIZA_DEVICE_EXTENSION);
  int length = strlen (Dev[id].prefix) + lenext + 5;
  char fileName[length + 1];
  _giza_get_filename_for_device(fileName,Dev[id].prefix,Dev[id].pgNum,GIZA_DEVICE_EXTENSION,1);

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

  Dev[id].surface = cairo_svg_surface_create (fileName, Dev[id].width, Dev[id].height);

  if (!Dev[id].surface)
    {
      _giza_error ("_giza_open_device_svg", "Could not create cairo svg surface");
      return -1;
    }

  return 0;
}

/**
 * Flush the svg
 */
void
_giza_flush_device_svg (void)
{
  cairo_surface_flush (Dev[id].surface);
}

/**
 * Closes the svg device
 */
void
_giza_close_device_svg (void)
{
  cairo_surface_finish (Dev[id].surface);
  cairo_status_t status = cairo_surface_status (Dev[id].surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_close_device_svg",cairo_status_to_string(status));
  cairo_surface_destroy (Dev[id].surface);
}

/**
 * Change the page
 */
void
_giza_change_page_svg (void)
{
  /* Close the old svg */
  cairo_destroy (Dev[id].context);
  _giza_close_device_svg();

  int lenext = strlen (GIZA_DEVICE_EXTENSION);
  int length = strlen (Dev[id].prefix) + lenext + 5;
  char fileName[length + 1];

  /* rename the first file from giza.svg to giza_0000.svg if part of a sequence */
  if (Dev[id].pgNum == 0) {
     char fileNameold[length + 1];
     /* retrieve the name of the original file and what revised name should be */
     _giza_get_filename_for_device(fileNameold,Dev[id].prefix,Dev[id].pgNum,GIZA_DEVICE_EXTENSION,1);
     _giza_get_filename_for_device(fileName,   Dev[id].prefix,Dev[id].pgNum,GIZA_DEVICE_EXTENSION,0);
     /* rename the file */
     rename(fileNameold,fileName);
  }

  /* name the new svg */
  _giza_get_filename_for_device(fileName,Dev[id].prefix,Dev[id].pgNum + 1,GIZA_DEVICE_EXTENSION,0);

  /* Open it */
  Dev[id].surface = cairo_svg_surface_create (fileName, Dev[id].width, Dev[id].height);

  if (!Dev[id].surface)
    {
      _giza_error ("giza_change_page_svg", "Could not create cairo svg surface");
      return;
    }

  Dev[id].context = cairo_create (Dev[id].surface);
  if (!Dev[id].context)
    {
      _giza_error ("giza_change_page_svg", "Could not create cairo context");
      return;
    }
}
