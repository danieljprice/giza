/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010-2026 Daniel Price
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
 */

#include <giza.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#define NPIX 80

static int
file_exists (const char *name)
{
  return access (name, F_OK) != -1;
}

int
main (void)
{
  const int n = NPIX, m = NPIX;
  double u[NPIX][NPIX], v[NPIX][NPIX];
  double affine[6];
  int i, j, id;
  double x, y, xmin, xmax, ymin, ymax, dx, dy;

  xmin = -1.;
  xmax = 1.;
  ymin = -1.;
  ymax = 1.;
  dx = (xmax - xmin) / (double) n;
  dy = (ymax - ymin) / (double) m;
  /* PGPLOT-style affine: world = A * (i+0.5, j+0.5) */
  affine[0] = dx;
  affine[1] = 0.;
  affine[2] = 0.;
  affine[3] = dy;
  affine[4] = xmin;
  affine[5] = ymin;

  id = giza_open_device ("/png", "test-streamplot");
  if (id <= 0)
    {
      fprintf (stderr, "failed to open png device\n");
      return 1;
    }

  /* circular field: (-y, x) */
  for (j = 0; j < m; j++)
    {
      for (i = 0; i < n; i++)
        {
          x = xmin + (i + 0.5) * dx;
          y = ymin + (j + 0.5) * dy;
          u[j][i] = -y;
          v[j][i] = x;
        }
    }
  giza_set_environment (xmin, xmax, ymin, ymax, 1, 0);
  giza_label ("x", "y", "circular field");
  giza_streamplot (n, m, (double *) u, (double *) v, 0, n - 1, 0, m - 1,
                   1.0, affine, 9999.);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);

  /* saddle field: (x, -y) */
  for (j = 0; j < m; j++)
    {
      for (i = 0; i < n; i++)
        {
          x = xmin + (i + 0.5) * dx;
          y = ymin + (j + 0.5) * dy;
          u[j][i] = x;
          v[j][i] = -y;
        }
    }
  giza_set_environment (xmin, xmax, ymin, ymax, 1, 0);
  giza_label ("x", "y", "saddle field");
  giza_streamplot (n, m, (double *) u, (double *) v, 0, n - 1, 0, m - 1,
                   1.0, affine, 9999.);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);

  /* uniform field, plus a blanked region */
  for (j = 0; j < m; j++)
    {
      for (i = 0; i < n; i++)
        {
          x = xmin + (i + 0.5) * dx;
          y = ymin + (j + 0.5) * dy;
          if (x > 0. && y > 0.)
            {
              u[j][i] = 9999.;
              v[j][i] = 9999.;
            }
          else
            {
              u[j][i] = 1.;
              v[j][i] = 0.3;
            }
        }
    }
  giza_set_environment (xmin, xmax, ymin, ymax, 1, 0);
  giza_label ("x", "y", "uniform field with blanks");
  giza_streamplot (n, m, (double *) u, (double *) v, 0, n - 1, 0, m - 1,
                   1.0, affine, 9999.);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);

  /* Orszag-Tang vortex B field on [0,1] x [0,1]:
   *   Bx = -sin(2 pi y),  By = sin(4 pi x)
   * This is a standard MHD test with nested magnetic islands. */
  xmin = 0.;
  xmax = 1.;
  ymin = 0.;
  ymax = 1.;
  dx = (xmax - xmin) / (double) n;
  dy = (ymax - ymin) / (double) m;
  affine[0] = dx;
  affine[1] = 0.;
  affine[2] = 0.;
  affine[3] = dy;
  affine[4] = xmin;
  affine[5] = ymin;
  for (j = 0; j < m; j++)
    {
      for (i = 0; i < n; i++)
        {
          x = xmin + (i + 0.5) * dx;
          y = ymin + (j + 0.5) * dy;
          u[j][i] = -sin (2. * M_PI * y);
          v[j][i] = sin (4. * M_PI * x);
        }
    }
  giza_set_environment (xmin, xmax, ymin, ymax, 1, 0);
  giza_label ("x", "y", "Orszag-Tang B field");
  giza_streamplot (n, m, (double *) u, (double *) v, 0, n - 1, 0, m - 1,
                   1.0, affine, 9999.);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);

  giza_close_device ();

  if (!file_exists ("test-streamplot_0000.png")
      || !file_exists ("test-streamplot_0001.png")
      || !file_exists ("test-streamplot_0002.png")
      || !file_exists ("test-streamplot_0003.png"))
    {
      fprintf (stderr, "Error: expected streamplot png files were not created\n");
      return 1;
    }

  return 0;
}
