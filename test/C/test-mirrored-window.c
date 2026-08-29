/* giza - regression test for box ticks on mirrored windows
 *
 * Draws giza_box frames with inverted x and/or y world limits (as used for
 * sky maps) and checks that tick marks extend toward the plot interior on
 * screen, not away from it.
 *
 * Uses a fixed viewport and fixed tick spacing so probe positions do not
 * depend on font metrics; ink checks use relative thresholds so thin
 * monospace strokes are not rejected.
 */

#include <giza.h>
#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SURFACE_SIZE 400
/* probe extent in pixels; large enough for thin strokes across fonts */
#define INK_PROBE    24
/* minimum inward ink pixels when outward ink is negligible */
#define INK_MIN      1
/* world tick positions used with fixed giza_box intervals (see draw_case) */
#define N_TICK_PROBE 3
/* fixed viewport margins (normalised device); avoids font-sized defaults */
#define VP_LEFT      0.15
#define VP_RIGHT     0.85
#define VP_BOTTOM    0.15
#define VP_TOP       0.85
/* skip frame line when comparing tick direction (pixels beyond frame) */
#define FRAME_SKIP   2

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
static void normalize_viewport_pixels (double *vp_x1, double *vp_x2,
                                       double *vp_y1, double *vp_y2);
static int tick_meets_horizontal_frame (cairo_surface_t *surface, int frame_y,
                                        int x);
static int tick_meets_vertical_frame (cairo_surface_t *surface, int frame_x,
                                      int y);
static int probe_horizontal_edge (cairo_surface_t *surface, int frame_y,
                                  int inward_dy, int x);
static int probe_vertical_edge (cairo_surface_t *surface, int frame_x,
                                int inward_dx, int y);
static int probe_horizontal_ticks (cairo_surface_t *surface, int frame_y,
                                   int inward_dy, double vp_x1, double vp_x2,
                                   double vp_y1, double vp_y2,
                                   double win_x1, double win_x2,
                                   double win_y1, double win_y2);
static int probe_vertical_ticks (cairo_surface_t *surface, int frame_x,
                                 int inward_dx, double vp_x1, double vp_x2,
                                 double vp_y1, double vp_y2,
                                 double win_x1, double win_x2,
                                 double win_y1, double win_y2);
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
static int tick_is_symmetric (cairo_surface_t *surface, int axis_pixel,
                              int tick_pixel);
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

/* major tick world positions for giza_box(..., 0.25, 1, ...) on a unit span */
static const double tick_probe_world[N_TICK_PROBE] = { 0.25, 0.50, 0.75 };

static int
tick_points_inward (int inward_ink, int outward_ink)
{
  if (inward_ink <= outward_ink)
    return 0;
  if (outward_ink <= INK_MIN)
    return inward_ink > INK_MIN;
  return inward_ink * 2 > outward_ink * 3;
}

static void
normalize_viewport_pixels (double *vp_x1, double *vp_x2,
                         double *vp_y1, double *vp_y2)
{
  double tmp;

  if (*vp_x1 > *vp_x2)
    {
      tmp = *vp_x1;
      *vp_x1 = *vp_x2;
      *vp_x2 = tmp;
    }
  if (*vp_y1 < *vp_y2)
    {
      tmp = *vp_y1;
      *vp_y1 = *vp_y2;
      *vp_y2 = tmp;
    }
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
tick_meets_horizontal_frame (cairo_surface_t *surface, int frame_y, int x)
{
  return count_ink_rect (surface, x - 3, frame_y - 1, x + 3, frame_y + 1)
         > INK_MIN;
}

static int
tick_meets_vertical_frame (cairo_surface_t *surface, int frame_x, int y)
{
  return count_ink_rect (surface, frame_x - 1, y - 3, frame_x + 1, y + 3)
         > INK_MIN;
}

static int
probe_horizontal_edge (cairo_surface_t *surface, int frame_y, int inward_dy,
                       int x)
{
  int inward_ink, outward_ink;
  int inward_y0, inward_y1, outward_y0, outward_y1;

  inward_y0 = frame_y + inward_dy * FRAME_SKIP;
  inward_y1 = frame_y + inward_dy * INK_PROBE;
  outward_y0 = frame_y - inward_dy * FRAME_SKIP;
  outward_y1 = frame_y - inward_dy * INK_PROBE;

  inward_ink = count_ink_rect (surface, x - 3, inward_y0, x + 3, inward_y1);
  outward_ink = count_ink_rect (surface, x - 3, outward_y0, x + 3, outward_y1);

  return tick_points_inward (inward_ink, outward_ink);
}

static int
probe_vertical_edge (cairo_surface_t *surface, int frame_x, int inward_dx,
                     int y)
{
  int inward_ink, outward_ink;
  int inward_x0, inward_x1, outward_x0, outward_x1;

  inward_x0 = frame_x + inward_dx * FRAME_SKIP;
  inward_x1 = frame_x + inward_dx * INK_PROBE;
  outward_x0 = frame_x - inward_dx * FRAME_SKIP;
  outward_x1 = frame_x - inward_dx * INK_PROBE;

  inward_ink = count_ink_rect (surface, inward_x0, y - 3,
                                inward_x1, y + 3);
  outward_ink = count_ink_rect (surface, outward_x0, y - 3,
                                 outward_x1, y + 3);

  return tick_points_inward (inward_ink, outward_ink);
}

static int
probe_horizontal_ticks (cairo_surface_t *surface, int frame_y, int inward_dy,
                        double vp_x1, double vp_x2, double vp_y1, double vp_y2,
                        double win_x1, double win_x2,
                        double win_y1, double win_y2)
{
  int i, pass_count, tick_x;

  pass_count = 0;
  for (i = 0; i < N_TICK_PROBE; i++)
    {
      tick_x = world_x_to_pixel (vp_x1, vp_x2, win_x1, win_x2,
                                 tick_probe_world[i]);
      if (tick_meets_horizontal_frame (surface, frame_y, tick_x)
          && probe_horizontal_edge (surface, frame_y, inward_dy, tick_x))
        pass_count++;
    }

  return pass_count >= 2;
}

static int
probe_vertical_ticks (cairo_surface_t *surface, int frame_x, int inward_dx,
                      double vp_x1, double vp_x2, double vp_y1, double vp_y2,
                      double win_x1, double win_x2,
                      double win_y1, double win_y2)
{
  int i, pass_count, tick_y;

  pass_count = 0;
  for (i = 0; i < N_TICK_PROBE; i++)
    {
      tick_y = world_y_to_pixel (vp_y1, vp_y2, win_y1, win_y2,
                                 tick_probe_world[i]);
      if (tick_meets_vertical_frame (surface, frame_x, tick_y)
          && probe_vertical_edge (surface, frame_x, inward_dx, tick_y))
        pass_count++;
    }

  return pass_count >= 2;
}

static int
probe_world_y_edge (cairo_surface_t *surface, double world_y,
                    double vp_x1, double vp_x2, double vp_y1, double vp_y2,
                    double win_x1, double win_x2, double win_y1, double win_y2)
{
  int frame_y, vp_cy, inward_dy;

  frame_y = world_y_to_pixel (vp_y1, vp_y2, win_y1, win_y2, world_y);
  vp_cy = (int) (0.5 * (vp_y1 + vp_y2));
  inward_dy = (frame_y > vp_cy) ? -1 : 1;

  return probe_horizontal_ticks (surface, frame_y, inward_dy,
                                 vp_x1, vp_x2, vp_y1, vp_y2,
                                 win_x1, win_x2, win_y1, win_y2);
}

static int
probe_world_x_edge (cairo_surface_t *surface, double world_x,
                    double vp_x1, double vp_x2, double vp_y1, double vp_y2,
                    double win_x1, double win_x2, double win_y1, double win_y2)
{
  int frame_x, vp_cx, inward_dx;

  frame_x = world_x_to_pixel (vp_x1, vp_x2, win_x1, win_x2, world_x);
  vp_cx = (int) (0.5 * (vp_x1 + vp_x2));
  inward_dx = (frame_x < vp_cx) ? 1 : -1;

  return probe_vertical_ticks (surface, frame_x, inward_dx,
                               vp_x1, vp_x2, vp_y1, vp_y2,
                               win_x1, win_x2, win_y1, win_y2);
}

static int
tick_is_symmetric (cairo_surface_t *surface, int axis_pixel, int tick_pixel)
{
  int above_ink, below_ink, diff;

  above_ink = count_ink_rect (surface, tick_pixel - 3, axis_pixel + FRAME_SKIP,
                              tick_pixel + 3, axis_pixel + INK_PROBE);
  below_ink = count_ink_rect (surface, tick_pixel - 3, axis_pixel - INK_PROBE,
                              tick_pixel + 3, axis_pixel - FRAME_SKIP);
  if (above_ink + below_ink < 2 * INK_MIN)
    return 0;

  diff = above_ink - below_ink;
  if (diff < 0)
    diff = -diff;

  return diff * 3 <= above_ink + below_ink;
}

static int
ticks_symmetric_on_axis (cairo_surface_t *surface, double axis_world_y)
{
  double vp_x1, vp_x2, vp_y1, vp_y2;
  double win_x1, win_x2, win_y1, win_y2;
  int axis_y, pass_count, i, tick_x;

  giza_get_viewport (GIZA_UNITS_PIXELS, &vp_x1, &vp_x2, &vp_y1, &vp_y2);
  giza_get_window (&win_x1, &win_x2, &win_y1, &win_y2);
  normalize_viewport_pixels (&vp_x1, &vp_x2, &vp_y1, &vp_y2);
  axis_y = world_y_to_pixel (vp_y1, vp_y2, win_y1, win_y2, axis_world_y);

  pass_count = 0;
  for (i = 0; i < N_TICK_PROBE; i++)
    {
      tick_x = world_x_to_pixel (vp_x1, vp_x2, win_x1, win_x2,
                                 tick_probe_world[i]);
      if (tick_is_symmetric (surface, axis_y, tick_x))
        pass_count++;
    }

  return pass_count >= 2;
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
  normalize_viewport_pixels (&vp_x1, &vp_x2, &vp_y1, &vp_y2);

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
  giza_set_viewport (VP_LEFT, VP_RIGHT, VP_BOTTOM, VP_TOP);
  giza_set_window (test->xmin, test->xmax, test->ymin, test->ymax);
  giza_set_colour_index (1);
  /* fixed tick spacing so probe positions do not depend on font metrics */
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
    fprintf (stderr, "%s: ticks must meet frame border and point inward\n",
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
