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
#include "giza-driver-eps-private.h"
#include "giza-driver-ps-private.h"
#include "giza-drivers-private.h"

#ifdef _GIZA_HAS_EPS
#include <giza.h>
#include <string.h>
#include <stdio.h>
#include <cairo/cairo-ps.h>

#define GIZA_DEFAULT_WIDTH 612
#define GIZA_DEFAULT_HEIGHT 459 /* use 4:3 aspect ratio */
#define GIZA_DEVICE_UNITS_PER_MM 2.8346
#define GIZA_DEVICE_UNITS_PER_PIXEL 0.765 /* so pixel resolution is same as pixel devices */
#define GIZA_DEVICE_INTERACTIVE 0
#define GIZA_DEVICE_EXTENSION ".eps"

/**
 * Opens an encapsulated postscript device for drawing to.
 *
 * The routine does the following:
 *  -Sets variables in the GIZA_Device structure Dev
 *  -Creates the cairo eps surface which is associated with a file
 *
 * Input:
 *  vert :- If set to one the created surface is portrait
 */
int
_giza_open_device_eps (double width, double height, int units, int vert)
{
  int length;
  length = strlen (Dev[id].prefix) + strlen (GIZA_DEVICE_EXTENSION) + 5;
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

  Dev[id].surface = cairo_ps_surface_create (fileName, Dev[id].width, Dev[id].height);

  if (!Dev[id].surface)
    {
      _giza_error ("giza_open_device_eps", "Could not create cairo eps surface");
      return -1;
    }

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  cairo_ps_surface_set_eps (Dev[id].surface, 1);
#endif
  return 0;
}

/**
 * Flush the eps
 */
void
_giza_flush_device_eps (void)
{
  cairo_surface_flush (Dev[id].surface);
}

/**
 * Closes the eps device
 */
void
_giza_close_device_eps (void)
{
  cairo_surface_finish (Dev[id].surface);
  cairo_status_t status = cairo_surface_status (Dev[id].surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_close_device_eps",cairo_status_to_string(status));
  cairo_surface_destroy (Dev[id].surface);
}

/**
 * Change the page
 */
void
_giza_change_page_eps (void)
{
  /* Close the old eps */
  cairo_destroy (Dev[id].context);
  _giza_close_device_eps();

  int lenext = strlen (GIZA_DEVICE_EXTENSION);
  int length = strlen (Dev[id].prefix) + lenext + 5;
  char fileName[length + 1];

  /* rename the first file from giza.eps to giza_0000.eps if part of a sequence */
  if (Dev[id].pgNum == 0) {
     char fileNameold[length + 1];
     /* retrieve the name of the original file and what revised name should be */
     _giza_get_filename_for_device(fileNameold,Dev[id].prefix,Dev[id].pgNum,GIZA_DEVICE_EXTENSION,1);
     _giza_get_filename_for_device(fileName,   Dev[id].prefix,Dev[id].pgNum,GIZA_DEVICE_EXTENSION,0);
     /* rename the file */
     rename(fileNameold,fileName);
  }

  /* name the new eps */
  _giza_get_filename_for_device(fileName,Dev[id].prefix,Dev[id].pgNum + 1,GIZA_DEVICE_EXTENSION,0);

  /* Open it */
  Dev[id].surface = cairo_ps_surface_create (fileName, Dev[id].width, Dev[id].height);

  if (!Dev[id].surface)
    {
      _giza_error ("giza_change_page_eps", "Could not create cairo eps surface");
      return;
    }
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
      cairo_ps_surface_set_eps (Dev[id].surface, 1);
#endif

  Dev[id].context = cairo_create (Dev[id].surface);
  if (!Dev[id].context)
    {
      _giza_error ("giza_change_page_eps", "Could not create cairo context");
      return;
    }
}
#endif
