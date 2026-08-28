/* giza - regression test for box ticks on mirrored windows
 *
 * Draws giza_box frames with inverted x and/or y world limits (as used for
 * sky maps) and checks that tick marks extend toward the plot interior on
 * screen, not away from it.
 */

#include <giza.h>
#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>

#define SURFACE_SIZE 400
#define INK_PROBE    20
#define INK_THRESH   4

#define PROBE_BOTTOM 1
#define PROBE_TOP    2
#define PROBE_LEFT   4
#define PROBE_AXIS   8

typedef struct
{
  const char *name;
  double xmin, xmax, ymin, ymax;
  const char *xopt, *yopt;
  int probes;
} mirror_case_t;

static int count_ink_rect (cairo_surface_t *surface,
                           int x0, int y0, int x1, int y1);
static int find_frame_row (cairo_surface_t *surface, int top);
static int find_frame_col (cairo_surface_t *surface, int left);
static int ticks_inward_horizontal (cairo_surface_t *surface, int top);
static int ticks_inward_vertical (cairo_surface_t *surface, int left);
static int ticks_symmetric_on_axis (cairo_surface_t *surface);
static int run_case (cairo_t *cr, const mirror_case_t *test);
static void draw_case (const mirror_case_t *test);

static int
count_ink_rect (cairo_surface_t *surface, int x0, int y0, int x1, int y1)
{
  unsigned char *data, *p;
  int width, height, stride, x, y, ink_count;

  if (x0 > x1)
    {
      x = x0;
      x0 = x1;
      x1 = x;
    }
  if (y0 > y1)
    {
      y = y0;
      y0 = y1;
      y1 = y;
    }

  cairo_surface_flush (surface);
  data = cairo_image_surface_get_data (surface);
  width = cairo_image_surface_get_width (surface);
  height = cairo_image_surface_get_height (surface);
  stride = cairo_image_surface_get_stride (surface);
  ink_count = 0;

  for (y = y0; y <= y1; y++)
    {
      for (x = x0; x <= x1; x++)
        {
          if (x < 0 || y < 0 || x >= width || y >= height)
            continue;
          p = data + y * stride + x * 4;
          if (p[0] + p[1] + p[2] < 3 * 240)
            ink_count++;
        }
    }

  return ink_count;
}

static int
find_frame_row (cairo_surface_t *surface, int top)
{
  int width, height, x0, x1, y, y0, y1, step, best_y, best_ink, ink_count;

  width = cairo_image_surface_get_width (surface);
  height = cairo_image_surface_get_height (surface);
  x0 = width / 4;
  x1 = 3 * width / 4;
  best_y = top ? 0 : height - 1;
  best_ink = 0;

  if (top)
    {
      y0 = 0;
      y1 = height / 3;
      step = 1;
    }
  else
    {
      y0 = 2 * height / 3;
      y1 = height - 1;
      step = 1;
    }

  for (y = y0; y <= y1; y += step)
    {
      ink_count = count_ink_rect (surface, x0, y, x1, y);
      if (ink_count > best_ink)
        {
          best_ink = ink_count;
          best_y = y;
        }
    }

  return best_y;
}

static int
find_frame_col (cairo_surface_t *surface, int left)
{
  int width, height, y0, y1, x, x0, x1, step, best_x, best_ink, ink_count;

  width = cairo_image_surface_get_width (surface);
  height = cairo_image_surface_get_height (surface);
  y0 = height / 4;
  y1 = 3 * height / 4;
  best_x = left ? 0 : width - 1;
  best_ink = 0;

  if (left)
    {
      x0 = 0;
      x1 = width / 3;
      step = 1;
    }
  else
    {
      x0 = 2 * width / 3;
      x1 = width - 1;
      step = 1;
    }

  for (x = x0; x <= x1; x += step)
    {
      ink_count = count_ink_rect (surface, x, y0, x, y1);
      if (ink_count > best_ink)
        {
          best_ink = ink_count;
          best_x = x;
        }
    }

  return best_x;
}

/* Scan along a horizontal frame edge for a tick whose ink extends toward
 * the plot interior rather than away from it. */
static int
ticks_inward_horizontal (cairo_surface_t *surface, int top)
{
  int width, height, center_y, frame_y, x, x0, x1, inward_sign;
  int inward_ink, outward_ink;

  width = cairo_image_surface_get_width (surface);
  height = cairo_image_surface_get_height (surface);
  center_y = height / 2;
  frame_y = find_frame_row (surface, top);
  inward_sign = top ? 1 : -1;
  if (frame_y > center_y)
    inward_sign = -1;
  else if (frame_y < center_y)
    inward_sign = 1;

  x0 = width / 4;
  x1 = 3 * width / 4;
  for (x = x0; x <= x1; x++)
    {
      inward_ink = count_ink_rect (surface, x - 2,
                                   frame_y + inward_sign,
                                   x + 2,
                                   frame_y + inward_sign * INK_PROBE);
      outward_ink = count_ink_rect (surface, x - 2,
                                    frame_y - inward_sign,
                                    x + 2,
                                    frame_y - inward_sign * INK_PROBE);
      if (inward_ink > outward_ink + INK_THRESH && inward_ink > INK_THRESH)
        return 1;
    }

  return 0;
}

/* Scan along a vertical frame edge for a tick whose ink extends toward
 * the plot interior rather than away from it. */
static int
ticks_inward_vertical (cairo_surface_t *surface, int left)
{
  int width, height, center_x, frame_x, y, y0, y1, inward_sign;
  int inward_ink, outward_ink;

  width = cairo_image_surface_get_width (surface);
  height = cairo_image_surface_get_height (surface);
  center_x = width / 2;
  frame_x = find_frame_col (surface, left);
  inward_sign = left ? 1 : -1;
  if (frame_x < center_x)
    inward_sign = 1;
  else if (frame_x > center_x)
    inward_sign = -1;

  y0 = height / 4;
  y1 = 3 * height / 4;
  for (y = y0; y <= y1; y++)
    {
      inward_ink = count_ink_rect (surface, frame_x + inward_sign, y - 2,
                                   frame_x + inward_sign * INK_PROBE, y + 2);
      outward_ink = count_ink_rect (surface, frame_x - inward_sign, y - 2,
                                    frame_x - inward_sign * INK_PROBE, y + 2);
      if (inward_ink > outward_ink + INK_THRESH && inward_ink > INK_THRESH)
        return 1;
    }

  return 0;
}

static int
ticks_symmetric_on_axis (cairo_surface_t *surface)
{
  int width, height, axis_y, tick_x, x, above_ink, below_ink, diff;

  width = cairo_image_surface_get_width (surface);
  height = cairo_image_surface_get_height (surface);
  axis_y = height / 2;
  tick_x = -1;

  for (x = width / 4; x < 3 * width / 4; x++)
    {
      if (count_ink_rect (surface, x, axis_y - INK_PROBE, x, axis_y + INK_PROBE)
          > INK_THRESH)
        {
          tick_x = x;
          break;
        }
    }

  if (tick_x < 0)
    return 0;

  above_ink = count_ink_rect (surface, tick_x - 1, axis_y + 1,
                              tick_x + 1, axis_y + INK_PROBE);
  below_ink = count_ink_rect (surface, tick_x - 1, axis_y - INK_PROBE,
                              tick_x + 1, axis_y - 1);
  if (above_ink <= INK_THRESH || below_ink <= INK_THRESH)
    return 0;

  diff = above_ink - below_ink;
  if (diff < 0)
    diff = -diff;

  return diff * 3 < above_ink + below_ink;
}

static void
draw_case (const mirror_case_t *test)
{
  giza_start_warnings ();
  giza_set_viewport_default ();
  giza_set_window (test->xmin, test->xmax, test->ymin, test->ymax);
  giza_set_colour_index (1);
  giza_box (test->xopt, 0.25, 1, test->yopt, 0.25, 1);
  giza_flush_device ();
}

static int
run_case (cairo_t *cr, const mirror_case_t *test)
{
  cairo_surface_t *surface;
  int failed;

  failed = 0;
  surface = cairo_get_target (cr);
  cairo_save (cr);
  cairo_set_source_rgb (cr, 1., 1., 1.);
  cairo_paint (cr);
  cairo_restore (cr);

  if (giza_set_cairo_context (cr) != 0)
    {
      fprintf (stderr, "%s: giza_set_cairo_context failed\n", test->name);
      return 1;
    }

  draw_case (test);
  giza_release_cairo_context ();
  cairo_surface_flush (surface);

  if (getenv ("GIZA_TEST_MIRROR_PNG"))
    {
      char pngname[64];
      snprintf (pngname, sizeof (pngname), "test-mirror-%s.png", test->name);
      cairo_surface_write_to_png (surface, pngname);
    }

  if ((test->probes & PROBE_BOTTOM) && !ticks_inward_horizontal (surface, 0))
    {
      fprintf (stderr, "%s: bottom ticks do not point toward plot interior\n",
               test->name);
      failed = 1;
    }

  if ((test->probes & PROBE_TOP) && !ticks_inward_horizontal (surface, 1))
    {
      fprintf (stderr, "%s: top ticks do not point toward plot interior\n",
               test->name);
      failed = 1;
    }

  if ((test->probes & PROBE_LEFT) && !ticks_inward_vertical (surface, 1))
    {
      fprintf (stderr, "%s: left ticks do not point toward plot interior\n",
               test->name);
      failed = 1;
    }

  if ((test->probes & PROBE_AXIS) && !ticks_symmetric_on_axis (surface))
    {
      fprintf (stderr, "%s: internal axis ticks are not symmetric\n",
               test->name);
      failed = 1;
    }

  return failed;
}

int
main (void)
{
  static mirror_case_t const cases[] = {
    /* giza_box y-axis: B=left, C=right, L=log (not "left") */
    { "normal", 0., 1., 0., 1., "BT", "", PROBE_BOTTOM },
    { "mirror_x", 1., 0., 0., 1., "", "BCT", PROBE_LEFT },
    { "mirror_y", 0., 1., 1., 0., "BCT", "", PROBE_TOP },
    { "mirror_xy", 1., 0., 1., 0., "BCT", "BCT", PROBE_TOP | PROBE_LEFT },
    { "axis_symmetric", 0., 1., -1., 1., "AT", "", PROBE_AXIS },
  };
  cairo_surface_t *surface;
  cairo_t *cr;
  size_t i;
  int failed;

  failed = 0;

  if (giza_open_device_size_cairo (SURFACE_SIZE, SURFACE_SIZE,
                                   GIZA_UNITS_PIXELS) <= 0)
    {
      fprintf (stderr, "test-mirrored-window: could not open cairo device\n");
      return EXIT_FAILURE;
    }

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                        SURFACE_SIZE, SURFACE_SIZE);
  cr = cairo_create (surface);

  for (i = 0; i < sizeof (cases) / sizeof (cases[0]); i++)
    failed += run_case (cr, &cases[i]);

  giza_close_device ();
  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  if (failed)
    return EXIT_FAILURE;

  printf ("Mirrored window tick test passed (C)\n");
  return EXIT_SUCCESS;
}
