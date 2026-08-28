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
#include <string.h>

#define SURFACE_SIZE 400
#define INK_PROBE    20
#define INK_THRESH   4

#define PROBE_AXIS 1

typedef struct
{
  const char *name;
  double xmin, xmax, ymin, ymax;
  const char *xopt, *yopt;
  int probes;
} mirror_case_t;

static int count_ink_rect (cairo_surface_t *surface,
                           int x0, int y0, int x1, int y1);
static int option_has (const char *opt, char letter);
static int world_x_to_pixel (double vp_x1, double vp_x2,
                             double win_x1, double win_x2, double world_x);
static int world_y_to_pixel (double vp_y1, double vp_y2,
                             double win_y1, double win_y2, double world_y);
static int probe_horizontal_edge (cairo_surface_t *surface, int frame_y,
                                  int inward_dy, int x0, int x1);
static int probe_vertical_edge (cairo_surface_t *surface, int frame_x,
                                int inward_dx, int y0, int y1);
static int probe_world_y_edge (cairo_surface_t *surface, double world_y,
                               double vp_x1, double vp_x2,
                               double vp_y1, double vp_y2,
                               double win_x1, double win_x2,
                               double win_y1, double win_y2);
static int probe_world_x_edge (cairo_surface_t *surface, double world_x,
                               double vp_x1, double vp_x2,
                               double vp_y1, double vp_y2,
                               double win_x1, double win_x2,
                               double win_y1, double win_y2);
static int ticks_symmetric_on_axis (cairo_surface_t *surface,
                                    double axis_world_y);
static int run_probes (const mirror_case_t *test, cairo_surface_t *surface);
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
option_has (const char *opt, char letter)
{
  if (!opt)
    return 0;

  for (; *opt; opt++)
    if (*opt == letter)
      return 1;

  return 0;
}

static int
world_x_to_pixel (double vp_x1, double vp_x2, double win_x1, double win_x2,
                  double world_x)
{
  double ratio;

  if (win_x2 == win_x1)
    return (int) (0.5 * (vp_x1 + vp_x2));

  ratio = (world_x - win_x1) / (win_x2 - win_x1);
  return (int) (vp_x1 + ratio * (vp_x2 - vp_x1));
}

static int
world_y_to_pixel (double vp_y1, double vp_y2, double win_y1, double win_y2,
                  double world_y)
{
  double ratio;

  if (win_y2 == win_y1)
    return (int) (0.5 * (vp_y1 + vp_y2));

  ratio = (world_y - win_y1) / (win_y2 - win_y1);
  return (int) (vp_y1 + ratio * (vp_y2 - vp_y1));
}

static int
probe_horizontal_edge (cairo_surface_t *surface, int frame_y, int inward_dy,
                       int x0, int x1)
{
  int x, inward_ink, outward_ink;

  for (x = x0; x <= x1; x++)
    {
      inward_ink = count_ink_rect (surface, x - 2,
                                   frame_y + inward_dy,
                                   x + 2,
                                   frame_y + inward_dy * INK_PROBE);
      outward_ink = count_ink_rect (surface, x - 2,
                                    frame_y - inward_dy,
                                    x + 2,
                                    frame_y - inward_dy * INK_PROBE);
      if (inward_ink > outward_ink + INK_THRESH && inward_ink > INK_THRESH)
        return 1;
    }

  return 0;
}

static int
probe_vertical_edge (cairo_surface_t *surface, int frame_x, int inward_dx,
                     int y0, int y1)
{
  int y, inward_ink, outward_ink;

  for (y = y0; y <= y1; y++)
    {
      inward_ink = count_ink_rect (surface, frame_x + inward_dx, y - 2,
                                   frame_x + inward_dx * INK_PROBE, y + 2);
      outward_ink = count_ink_rect (surface, frame_x - inward_dx, y - 2,
                                    frame_x - inward_dx * INK_PROBE, y + 2);
      if (inward_ink > outward_ink + INK_THRESH && inward_ink > INK_THRESH)
        return 1;
    }

  return 0;
}

static int
probe_world_y_edge (cairo_surface_t *surface, double world_y,
                    double vp_x1, double vp_x2, double vp_y1, double vp_y2,
                    double win_x1, double win_x2, double win_y1, double win_y2)
{
  int frame_y, vp_cy, inward_dy, x0, x1;

  frame_y = world_y_to_pixel (vp_y1, vp_y2, win_y1, win_y2, world_y);
  vp_cy = (int) (0.5 * (vp_y1 + vp_y2));
  inward_dy = (frame_y > vp_cy) ? -1 : 1;
  x0 = (int) (vp_x1 + 0.25 * (vp_x2 - vp_x1));
  x1 = (int) (vp_x1 + 0.75 * (vp_x2 - vp_x1));

  return probe_horizontal_edge (surface, frame_y, inward_dy, x0, x1);
}

static int
probe_world_x_edge (cairo_surface_t *surface, double world_x,
                    double vp_x1, double vp_x2, double vp_y1, double vp_y2,
                    double win_x1, double win_x2, double win_y1, double win_y2)
{
  int frame_x, vp_cx, inward_dx, y0, y1;

  frame_x = world_x_to_pixel (vp_x1, vp_x2, win_x1, win_x2, world_x);
  vp_cx = (int) (0.5 * (vp_x1 + vp_x2));
  inward_dx = (frame_x < vp_cx) ? 1 : -1;
  y0 = (int) (vp_y2 + 0.25 * (vp_y1 - vp_y2));
  y1 = (int) (vp_y2 + 0.75 * (vp_y1 - vp_y2));

  return probe_vertical_edge (surface, frame_x, inward_dx, y0, y1);
}

static int
ticks_symmetric_on_axis (cairo_surface_t *surface, double axis_world_y)
{
  double vp_x1, vp_x2, vp_y1, vp_y2;
  double win_x1, win_x2, win_y1, win_y2;
  int width, axis_y, tick_x, x, x0, x1, above_ink, below_ink, diff;

  giza_get_viewport (GIZA_UNITS_PIXELS, &vp_x1, &vp_x2, &vp_y1, &vp_y2);
  giza_get_window (&win_x1, &win_x2, &win_y1, &win_y2);
  axis_y = world_y_to_pixel (vp_y1, vp_y2, win_y1, win_y2, axis_world_y);

  width = cairo_image_surface_get_width (surface);
  x0 = width / 4;
  x1 = 3 * width / 4;
  tick_x = -1;

  for (x = x0; x <= x1; x++)
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

static int
run_probes (const mirror_case_t *test, cairo_surface_t *surface)
{
  double vp_x1, vp_x2, vp_y1, vp_y2;
  double win_x1, win_x2, win_y1, win_y2;
  int failed;

  failed = 0;
  giza_get_viewport (GIZA_UNITS_PIXELS, &vp_x1, &vp_x2, &vp_y1, &vp_y2);
  giza_get_window (&win_x1, &win_x2, &win_y1, &win_y2);

  if (vp_x1 > vp_x2)
    {
      double tmp = vp_x1;
      vp_x1 = vp_x2;
      vp_x2 = tmp;
    }
  if (vp_y1 < vp_y2)
    {
      double tmp = vp_y1;
      vp_y1 = vp_y2;
      vp_y2 = tmp;
    }

  /* xopt B/C and yopt B/C attach ticks to world edges (pWin limits), which
   * may lie on either screen side when the window is mirrored. */
  if (option_has (test->xopt, 'B')
      && !probe_world_y_edge (surface, win_y1, vp_x1, vp_x2, vp_y1, vp_y2,
                              win_x1, win_x2, win_y1, win_y2))
    failed = 1;

  if (option_has (test->xopt, 'C')
      && !probe_world_y_edge (surface, win_y2, vp_x1, vp_x2, vp_y1, vp_y2,
                              win_x1, win_x2, win_y1, win_y2))
    failed = 1;

  if (option_has (test->yopt, 'B')
      && !probe_world_x_edge (surface, win_x1, vp_x1, vp_x2, vp_y1, vp_y2,
                              win_x1, win_x2, win_y1, win_y2))
    failed = 1;

  if (option_has (test->yopt, 'C')
      && !probe_world_x_edge (surface, win_x2, vp_x1, vp_x2, vp_y1, vp_y2,
                              win_x1, win_x2, win_y1, win_y2))
    failed = 1;

  if ((test->probes & PROBE_AXIS)
      && !ticks_symmetric_on_axis (surface, 0.))
    failed = 1;

  return failed;
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
  cairo_surface_flush (surface);
  failed = run_probes (test, surface);
  giza_release_cairo_context ();
  cairo_surface_flush (surface);

  if (getenv ("GIZA_TEST_MIRROR_PNG"))
    {
      char pngname[64];
      snprintf (pngname, sizeof (pngname), "test-mirror-%s.png", test->name);
      cairo_surface_write_to_png (surface, pngname);
    }

  if (failed)
    fprintf (stderr, "%s: tick marks do not point toward plot interior\n",
             test->name);

  return failed;
}

int
main (void)
{
  static mirror_case_t const cases[] = {
    /* giza_box y-axis: B=left, C=right, L=log (not "left") */
    { "normal", 0., 1., 0., 1., "BT", "", 0 },
    { "mirror_x", 1., 0., 0., 1., "", "BCT", 0 },
    { "mirror_y", 0., 1., 1., 0., "BCT", "", 0 },
    { "mirror_xy", 1., 0., 1., 0., "BCT", "BCT", 0 },
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
