/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
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

/* Regression test for external cairo device API (issue #68). */

#include <giza.h>
#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int surface_has_content (cairo_surface_t *surface);
static int check_png_file (const char *filename);
static void draw_test_plot (void);
static int test_null_context_rejected (int id);
static int test_string_open_rejected (void);
static int test_plot_before_bind_rejected (int id);

int
main (void)
{
  cairo_surface_t *surface;
  cairo_t *cr;
  int id;
  double x1, x2, y1, y2;

  giza_start_warnings ();

  id = giza_open_device_cairo (NULL, 400., 300., GIZA_UNITS_PIXELS);
  if (id <= 0)
    {
      fprintf (stderr, "Error: deferred giza_open_device_cairo failed\n");
      return 1;
    }

  if (!test_null_context_rejected (id))
    return 1;

  if (!test_plot_before_bind_rejected (id))
    return 1;

  if (giza_get_cairo_surface () != NULL)
    {
      fprintf (stderr, "Error: surface bound before set_cairo_context\n");
      giza_close_device ();
      return 1;
    }

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 400, 300);
  cr = cairo_create (surface);

  if (giza_set_cairo_context (cr) != 0)
    {
      fprintf (stderr, "Error: giza_set_cairo_context failed\n");
      return 1;
    }

  draw_test_plot ();

  giza_release_cairo_context ();

  if (giza_resize_device_cairo (200., 150., GIZA_UNITS_PIXELS) != 0)
    {
      fprintf (stderr, "Error: giza_resize_device_cairo failed\n");
      giza_close_device ();
      return 1;
    }

  giza_get_surface_size (&x1, &x2, &y1, &y2);
  if (x2 != 200. || y2 != 150.)
    {
      fprintf (stderr, "Error: resize did not update surface size\n");
      giza_close_device ();
      return 1;
    }

  if (giza_get_cairo_surface () != NULL)
    {
      fprintf (stderr, "Error: expected no bound surface after release\n");
      giza_close_device ();
      return 1;
    }

  if (giza_set_cairo_context (cr) != 0)
    {
      fprintf (stderr, "Error: rebind failed\n");
      giza_release_cairo_context ();
      giza_close_device ();
      return 1;
    }
  draw_test_plot ();

  giza_release_cairo_context ();

  if (!surface_has_content (surface))
    {
      fprintf (stderr, "Error: surface has no drawn content\n");
      giza_close_device ();
      return 1;
    }

  if (cairo_surface_write_to_png (surface, "test-cairo-device.png") != CAIRO_STATUS_SUCCESS)
    {
      fprintf (stderr, "Error: could not write PNG\n");
      giza_close_device ();
      return 1;
    }

  giza_close_device ();

  if (!check_png_file ("test-cairo-device.png"))
    return 1;

  if (cairo_surface_write_to_png (surface, "test-cairo-device-after-close.png") != CAIRO_STATUS_SUCCESS)
    {
      fprintf (stderr, "Error: surface invalid after giza_close_device\n");
      return 1;
    }

  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  if (!test_string_open_rejected ())
    return 1;

  printf ("External cairo device test passed (C)\n");
  return 0;
}

static void
draw_test_plot (void)
{
  giza_start_warnings ();
  giza_set_environment (0., 1., 0., 1., 0, 0);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_set_colour_index (2);
  giza_move (0.1, 0.5);
  giza_draw (0.9, 0.5);
}

static int
surface_has_content (cairo_surface_t *surface)
{
  unsigned char *data;
  int width, height, stride, x, y;
  unsigned char first;

  cairo_surface_flush (surface);
  data = cairo_image_surface_get_data (surface);
  width = cairo_image_surface_get_width (surface);
  height = cairo_image_surface_get_height (surface);
  stride = cairo_image_surface_get_stride (surface);

  if (data == NULL || width <= 0 || height <= 0)
    return 0;

  first = data[0];
  for (y = 0; y < height; y++)
    for (x = 0; x < width; x++)
      if (data[y * stride + x * 4] != first)
        return 1;

  return 0;
}

static int
check_png_file (const char *filename)
{
  if (access (filename, F_OK) != 0)
    {
      fprintf (stderr, "Error: %s was not created\n", filename);
      return 0;
    }
  return 1;
}

static int
test_null_context_rejected (int id)
{
  (void) id;

  if (giza_set_cairo_context (NULL) == 0)
    {
      fprintf (stderr, "Error: giza_set_cairo_context(NULL) should fail\n");
      giza_close_device ();
      return 0;
    }
  return 1;
}

static int
test_plot_before_bind_rejected (int id)
{
  (void) id;

  giza_start_warnings ();
  giza_set_environment (0., 1., 0., 1., 0, 0);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);

  if (giza_get_cairo_surface () != NULL)
    {
      fprintf (stderr, "Error: surface bound after failed pre-bind plot\n");
      giza_close_device ();
      return 0;
    }

  return 1;
}

static int
test_string_open_rejected (void)
{
  int id;

  giza_start_warnings ();
  id = giza_open_device ("/cairo", "test-cairo-reject");
  if (id > 0)
    {
      fprintf (stderr, "Error: giza_open_device(\"/cairo\") should fail\n");
      giza_close_device ();
      return 0;
    }
  return 1;
}
