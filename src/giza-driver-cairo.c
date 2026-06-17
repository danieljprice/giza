/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2022 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 5a) of the GPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
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
#include "giza-driver-cairo-private.h"
#include "giza-transforms-private.h"
#include "giza-viewport-private.h"
#include "giza-window-private.h"
#include "giza-character-size-private.h"
#include "giza-text-private.h"
#include "giza-set-font-private.h"
#include "giza-colour-private.h"
#include "giza-arrow-style-private.h"
#include "giza-fill-private.h"
#include "giza-line-style-private.h"
#include "giza-band-private.h"
#include "giza-stroke-private.h"
#include "giza-subpanel-private.h"
#include "giza-text-background-private.h"
#include <giza.h>
#include <stddef.h>

#define GIZA_DEFAULT_WIDTH 800
#define GIZA_DEFAULT_HEIGHT 600
#define GIZA_DEVICE_UNITS_PER_MM 3.7054
#define GIZA_DEVICE_UNITS_PER_PIXEL 1.0
#define GIZA_DEVICE_INTERACTIVE 0

static int _giza_check_cairo_device_open (const char *source);

/**
 * Opens a cairo device with the given logical size but no bound context yet.
 */
int
_giza_open_device_cairo (double width, double height, int units)
{
  Dev[id].deviceUnitsPermm = GIZA_DEVICE_UNITS_PER_MM;
  Dev[id].deviceUnitsPerPixel = GIZA_DEVICE_UNITS_PER_PIXEL;
  Dev[id].isInteractive = GIZA_DEVICE_INTERACTIVE;
  Dev[id].external_cairo = 1;
  Dev[id].cairo_context_bound = 0;
  Dev[id].cairo_deferred_init = 0;
  Dev[id].context = NULL;
  Dev[id].surface = NULL;

  if (width > 0. && height > 0. && units > 0)
    {
      _giza_get_specified_size (width, height, units, &Dev[id].width, &Dev[id].height);
    }
  else
    {
      Dev[id].width = GIZA_DEFAULT_WIDTH;
      Dev[id].height = GIZA_DEFAULT_HEIGHT;
    }

  return 0;
}

/**
 * Binds a caller-owned cairo context to the current device.
 * Does not take ownership of cr or its target surface.
 */
void
_giza_bind_cairo_context (cairo_t *cr)
{
  Dev[id].context = cr;
  Dev[id].surface = cairo_get_target (cr);
}

/**
 * Clears cached cairo pointers without restoring caller state.
 */
void
_giza_unbind_cairo_context (void)
{
  Dev[id].context = NULL;
  Dev[id].surface = NULL;
  Dev[id].cairo_context_bound = 0;
}

void
_giza_flush_device_cairo (void)
{
  if (Dev[id].surface)
    cairo_surface_flush (Dev[id].surface);
}

void
_giza_change_page_cairo (void)
{
  /* External cairo surfaces have no pages; nothing to do. */
}

void
_giza_close_device_cairo (void)
{
  /* Caller owns context and surface; nothing to destroy here. */
}

static int _giza_check_cairo_device_open (const char *source)
{
  if (!_giza_check_device_open ((char *) source))
    return 0;
  if (Dev[id].type != GIZA_DEVICE_CAIRO)
    {
      _giza_error ((char *) source, "Not a cairo device");
      return 0;
    }
  return 1;
}

/**
 * Device: giza_open_device_cairo
 *
 * Synopsis: Opens an external cairo device with default logical size (800x600 px).
 * Call giza_set_cairo_context before plotting.
 *
 * See Also: giza_open_device_size_cairo, giza_set_cairo_context
 */
int
giza_open_device_cairo (void)
{
  return giza_open_device_size_cairo (0., 0., 0);
}

/**
 * Device: giza_open_device_size_cairo
 *
 * Synopsis: Opens an external cairo device with the given logical size.
 * Call giza_set_cairo_context before plotting.
 */
int
giza_open_device_size_cairo (double width, double height, int units)
{
  int newId;

  _giza_init_all_devices_once ();

  for (newId = 0; newId < GIZA_MAX_DEVICES; newId++)
    if (Dev[newId].deviceOpen == 0)
      break;
  if (newId >= GIZA_MAX_DEVICES)
    {
      _giza_error ("giza_open_device_size_cairo", "No more free devices (%d in use). Close some first.", GIZA_MAX_DEVICES);
      return -1;
    }

  const int prevId = id;
  id = newId;

  _giza_init ();
  Dev[id].type = GIZA_DEVICE_CAIRO;
  Dev[id].number_format = GIZA_NUMBER_FORMAT_AUTO;

  if (_giza_open_device_cairo (width, height, units) != 0)
    {
      _giza_close_device_unchecked ();
      id = prevId;
      return -1;
    }

  Dev[id].deviceOpen = 1;

  return _giza_complete_device_open (0);
}

/**
 * Device: giza_open_device_size_cairo_float
 *
 * Synopsis: Same as giza_open_device_size_cairo but takes floats.
 *
 * See Also: giza_open_device_size_cairo
 */
int
giza_open_device_size_cairo_float (float width, float height, int units)
{
  return giza_open_device_size_cairo ((double) width, (double) height, units);
}

/**
 * Device: giza_set_cairo_context
 *
 * Synopsis: Bind a caller-owned cairo_t for the current frame.
 * Performs cairo_save so giza_release_cairo_context can restore GTK state.
 */
int
giza_set_cairo_context (cairo_t *cr)
{
  cairo_surface_t *target;
  cairo_status_t status;

  if (!_giza_check_cairo_device_open ("giza_set_cairo_context"))
    return -1;

  if (cr == NULL)
    {
      _giza_error ("giza_set_cairo_context", "cairo context must not be NULL");
      return -1;
    }

  status = cairo_status (cr);
  if (status != CAIRO_STATUS_SUCCESS)
    {
      _giza_error ("giza_set_cairo_context", "Invalid cairo context (%s)",
                   cairo_status_to_string (status));
      return -1;
    }

  if (Dev[id].cairo_context_bound)
    giza_release_cairo_context ();

  target = cairo_get_target (cr);
  status = cairo_surface_status (target);
  if (status != CAIRO_STATUS_SUCCESS)
    {
      _giza_error ("giza_set_cairo_context", "Invalid cairo surface (%s)",
                   cairo_status_to_string (status));
      return -1;
    }

  cairo_save (cr);
  Dev[id].cairo_context_bound = 1;
  _giza_bind_cairo_context (cr);

  if (Dev[id].cairo_deferred_init)
    {
      _giza_init_colour_index ();
      _giza_init_colour_table ();
      _giza_set_trans (GIZA_TRANS_IDEN);
      _giza_init_norm ();
      _giza_init_font ();
      _giza_init_text_background ();
      giza_set_viewport_default ();
      giza_set_line_width (1);
      _giza_init_fill ();
      _giza_init_band_style ();
      _giza_init_save ();
      giza_set_clipping (1);
      Dev[id].cairo_deferred_init = 0;
    }
  else
    _giza_set_trans (Dev[id].CurrentTrans);

  return 0;
}

/**
 * Device: giza_release_cairo_context
 *
 * Synopsis: End a frame of drawing to an external cairo context.
 * Restores the caller's cairo state and clears the cached pointer.
 */
int
giza_release_cairo_context (void)
{
  cairo_t *cr;

  if (!_giza_check_cairo_device_open ("giza_release_cairo_context"))
    return -1;

  if (!Dev[id].cairo_context_bound || Dev[id].context == NULL)
    return 0;

  cr = Dev[id].context;
  _giza_unbind_cairo_context ();
  cairo_restore (cr);

  return 0;
}

/**
 * Device: giza_resize_device_cairo
 *
 * Synopsis: Update logical device size after a widget resize.
 */
int
giza_resize_device_cairo (double width, double height, int units)
{
  int prevTrans;

  if (!_giza_check_cairo_device_open ("giza_resize_device_cairo"))
    return -1;

  if (width <= 0. || height <= 0. || units <= 0)
    {
      _giza_error ("giza_resize_device_cairo", "Invalid width, height or units");
      return -1;
    }

  prevTrans = Dev[id].CurrentTrans;
  _giza_get_specified_size (width, height, units, &Dev[id].width, &Dev[id].height);
  _giza_init_norm ();

  if (Dev[id].context != NULL)
    _giza_set_trans (prevTrans);

  return 0;
}

/**
 * Device: giza_get_cairo_surface
 *
 * Synopsis: Return the cairo surface currently bound to the device.
 * Valid while the device is open; do not destroy the returned surface.
 */
cairo_surface_t *
giza_get_cairo_surface (void)
{
  if (!_giza_check_cairo_device_open ("giza_get_cairo_surface"))
    return NULL;

  if (Dev[id].surface == NULL)
    {
      _giza_error ("giza_get_cairo_surface", "No cairo surface bound (call giza_set_cairo_context first)");
      return NULL;
    }

  return Dev[id].surface;
}
