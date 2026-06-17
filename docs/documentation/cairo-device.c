/* Example: plot into a caller-owned cairo image surface (see cairo-device.html). */

#include <giza.h>
#include <cairo/cairo.h>
#include <stdio.h>

int
main (void)
{
  cairo_surface_t *surface;
  cairo_t *cr;
  int dev;

  dev = giza_open_device_size_cairo (400., 300., GIZA_UNITS_PIXELS);
  if (dev <= 0)
    {
      fprintf (stderr, "giza_open_device_size_cairo failed\n");
      return 1;
    }

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 400, 300);
  cr = cairo_create (surface);

  if (giza_set_cairo_context (cr) != 0)
    {
      fprintf (stderr, "giza_set_cairo_context failed\n");
      return 1;
    }

  giza_set_environment (0., 1., 0., 1., 0, 0);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_set_colour_index (2);
  giza_move (0.1, 0.5);
  giza_draw (0.9, 0.5);

  giza_release_cairo_context ();
  giza_close_device ();

  if (cairo_surface_write_to_png (surface, "cairo-device.png") != CAIRO_STATUS_SUCCESS)
    {
      fprintf (stderr, "could not write PNG\n");
      return 1;
    }

  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  return 0;
}
