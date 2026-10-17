/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2026 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING.LESSER file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2 of the LGPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the LGPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

/*
 * Regression test: rendering an image larger than cairo's 8kB small-image
 * threshold on the /xw device must not raise X errors. cairo builds whose
 * Xlib backend runs over XCB (e.g. XQuartz) upload such images via MIT-SHM,
 * which fails with BadAccess/BadShmSeg on macOS unless the driver disables it.
 *
 * Skipped (exit code 77) when no X display is available.
 */
#include <giza.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

static int x_error_count = 0;

/* count X errors instead of letting giza's handler only print them */
static int count_x_errors (Display *display, XErrorEvent *event)
{
  char text[80];
  XGetErrorText (display, event->error_code, text, (int) sizeof (text));
  fprintf (stderr, "X error: %s (request %d.%d)\n", text,
           event->request_code, event->minor_code);
  x_error_count++;
  return 0;
}

/* render an nx x ny image covering the viewport, then flush */
static void render_image (int nx, int ny)
{
  double *data = malloc (sizeof (double) * (size_t) nx * (size_t) ny);
  double affine[6] = {1., 0., 0., 1., 0., 0.};
  int i;

  for (i = 0; i < nx * ny; i++)
    data[i] = (double) (i % 97);
  giza_set_window (0., (double) nx, 0., (double) ny);
  giza_render (nx, ny, data, 0, nx - 1, 0, ny - 1, 0., 96., 0, 0, affine);
  giza_flush_device ();
  free (data);
}

int main (void)
{
  /* skip if there is no X server to talk to */
  Display *probe = XOpenDisplay (NULL);
  if (!probe)
    {
      printf ("no X display available: skipping\n");
      return 77;
    }
  XCloseDisplay (probe);

  if (giza_open_device ("/xw", "test-xw-render-shm") <= 0)
    return 1;
  giza_stop_prompting ();

  /* replace giza's handler (installed when the device opened) */
  XSetErrorHandler (count_x_errors);

  /* sizes either side of cairo's SHM threshold, plus a large image */
  render_image (32, 32);
  render_image (546, 545);
  render_image (1000, 1000);

  /* closing the display round-trips to the server, delivering any errors */
  giza_close_device ();

  if (x_error_count > 0)
    {
      printf ("FAILED: %d X errors while rendering\n", x_error_count);
      return 1;
    }
  printf ("PASSED\n");
  return 0;
}
