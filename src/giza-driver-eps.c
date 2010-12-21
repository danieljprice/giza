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
 * Copyright (C) 2010 James Wetter & Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@sci.monash.edu.au
 *
 */
#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-driver-eps-private.h"
#include "giza-driver-ps-private.h"

#ifdef _GIZA_HAS_EPS
#include <giza.h>
#include <stdio.h>
#include <string.h>
#include <cairo/cairo-ps.h>

#define GIZA_DEFAULT_WIDTH 612
#define GIZA_DEFAULT_HEIGHT 459 // use 4:3 aspect ratio
#define GIZA_DEVICE_UNITS_PER_MM 2.8346
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
_giza_open_device_eps (int vert)
{
  int length;
  length = strlen (Dev.prefix) + strlen (GIZA_DEVICE_EXTENSION) + 5;
  char fileName[length + 1];
  sprintf (fileName, "%s_%04d%s", Dev.prefix, Dev.pgNum, GIZA_DEVICE_EXTENSION);

  Dev.deviceUnitsPermm = GIZA_DEVICE_UNITS_PER_MM;
  Dev.isInteractive    = GIZA_DEVICE_INTERACTIVE;

  // set all device specific settings
  if (_giza_sizeSpecified() )
    {
      Dev.width  = (int) (Dev.deviceUnitsPermm * 10. * Dev.widthCM) + 1;
      Dev.height = (int) (Dev.deviceUnitsPermm * 10. * Dev.heightCM) + 1;
    }
  else if (vert)
    {
      Dev.height = GIZA_DEFAULT_WIDTH;
      Dev.width  = GIZA_DEFAULT_HEIGHT;
    }
  else
    {
      Dev.width  = GIZA_DEFAULT_WIDTH;
      Dev.height = GIZA_DEFAULT_HEIGHT;
    }

  surface = cairo_ps_surface_create (fileName, Dev.width, Dev.height);

  if (!surface)
    {
      _giza_error ("_giza_open_device_eps", "Could not create cairo PS surface");
      return -1;
    }

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  cairo_ps_surface_set_eps (surface, 1);
#endif
  return 0;
}

/**
 * Flush the eps
 */
void
_giza_flush_device_eps (void)
{
  //cairo_surface_flush (context);
}

/**
 * Closes the eps device
 */ 
void
_giza_close_device_eps (void)
{
  cairo_surface_destroy (surface);
}

/**
 * Draws the background
 */
void
_giza_draw_background_eps (void)
{
  cairo_save (context);
  cairo_reset_clip (context);
  giza_set_colour_index (0);
  cairo_paint (context);
  cairo_restore (context);
}

/**
 * Change the page
 */
void
_giza_change_page_eps (void)
{
  // Close the old eps
  cairo_destroy (context);
  cairo_surface_destroy (surface);

  // name the new eps
  int length;
  length = strlen (Dev.prefix) + strlen (GIZA_DEVICE_EXTENSION) + 5;
  char fileName[length + 1];
  sprintf (fileName, "%s_%04d%s", Dev.prefix, Dev.pgNum + 1, GIZA_DEVICE_EXTENSION);

  // Open it
  surface = cairo_ps_surface_create (fileName, Dev.width, Dev.height);

  if (!surface)
    {
      _giza_error ("_giza_change_page_eps", "Could not create cairo PS surface");
      return;
    }
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
      cairo_ps_surface_set_eps (surface, 1);
#endif

  context = cairo_create (surface);
  if (!context)
    {
      _giza_error ("_giza_change_page_eps", "Could not create cairo context");
      return;
    }
}
#endif
