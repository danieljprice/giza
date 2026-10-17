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
 *
 * Contributor(s):
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

#include "giza-io-private.h"
#include "giza-private.h"
#include "giza-stroke-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <math.h>
#include <stdlib.h>

/* occupancy grid size at density = 1 */
#define STREAM_MASK_BASE 30
#define STREAM_MAX_PTS 8192
#define STREAM_MAXLENGTH_FRAC 4.0
#define STREAM_DENSITY_MIN 0.05
#define STREAM_DENSITY_MAX 10.0
#define STREAM_MAXERROR 0.003
#define STREAM_MAXDS_AXES 0.1
#define STREAM_MAX_REJECT 50
/* noisy/null fields can loop inside one occupancy cell; stop after a few steps */
#define STREAM_MAX_SAME_CELL 4

typedef struct
{
  int n, m, i1, i2, j1, j2, nx, ny;
  const double *u;
  const double *v;
  double blank;
  int mask_nx, mask_ny;
  unsigned char *mask;
  unsigned char *cur;
  cairo_matrix_t mat;
} giza_stream_t;

typedef struct
{
  int mx, my;
  double d2;
} giza_seed_t;

static int _giza_stream_blank (const giza_stream_t *s, int i, int j);
static int _giza_stream_sample (const giza_stream_t *s, double x, double y,
                                double *uout, double *vout);
static int _giza_stream_in_mask (const giza_stream_t *s, double x, double y,
                                 int *mx, int *my);
static int _giza_seed_cmp (const void *a, const void *b);
static int _giza_stream_integrate (giza_stream_t *s, double x0, double y0,
                                   int direction, double maxlength,
                                   double *xpts, double *ypts, int *npts,
                                   int *occ, int *nocc, int noccmax);
static int _giza_stream_arrow_index (const double *xw, const double *yw,
                                     int npts, double x0, double y0);
static int _giza_stream_add_line (giza_stream_t *s, const double *xg,
                                  const double *yg, int npts, double *xa,
                                  double *ya, double *xb, double *yb);
static void _giza_streamplot_core (int n, int m, const double *u,
                                   const double *v, int i1, int i2, int j1,
                                   int j2, double density, const double *affine,
                                   double blank);

/**
 * Drawing: giza_streamplot
 *
 * Synopsis: Plot evenly-spaced streamlines of a 2D vector field.
 * Trajectories are integrated with RK2 on the supplied regular grid
 * and spaced using an occupancy mask.
 *
 * Input:
 *  -n        :- The dimensions of data in the x-direction
 *  -m        :- The dimensions of data in the y-direction
 *  -u        :- The x-component of the vector field
 *  -v        :- The y-component of the vector field
 *  -i1, i2   :- Inclusive range of data to use in the x dimension
 *  -j1, j2   :- Inclusive range of data to use in the y dimension
 *  -density  :- Line density (1.0 is about 30 occupancy cells)
 *  -affine   :- Affine transformation from pixel indices to world coordinates
 *  -blank    :- Value of u and v that marks an empty cell (no streamline)
 *
 * See Also: giza_streamplot_float, giza_vector, giza_arrow, giza_line
 */
void
giza_streamplot (int n, int m, const double *u, const double *v,
                 int i1, int i2, int j1, int j2, double density,
                 const double *affine, double blank)
{
  if (!_giza_check_device_ready ("giza_streamplot"))
    return;

  _giza_streamplot_core (n, m, u, v, i1, i2, j1, j2, density, affine, blank);
}

/**
 * Drawing: giza_streamplot_float
 *
 * Synopsis: Same as giza_streamplot but takes floats
 *
 * See Also: giza_streamplot
 */
void
giza_streamplot_float (int n, int m, const float *u, const float *v,
                       int i1, int i2, int j1, int j2, float density,
                       const float *affine, float blank)
{
  size_t np, i;
  double *ud, *vd, affd[6];

  if (!_giza_check_device_ready ("giza_streamplot"))
    return;

  if (n <= 0 || m <= 0)
    return;

  np = (size_t) n * (size_t) m;
  ud = malloc (np * sizeof (double));
  vd = malloc (np * sizeof (double));
  if (ud == NULL || vd == NULL)
    {
      _giza_error ("giza_streamplot", "memory allocation failed");
      free (ud);
      free (vd);
      return;
    }

  for (i = 0; i < np; i++)
    {
      ud[i] = (double) u[i];
      vd[i] = (double) v[i];
    }
  for (i = 0; i < 6; i++)
    affd[i] = (double) affine[i];

  _giza_streamplot_core (n, m, ud, vd, i1, i2, j1, j2, (double) density,
                         affd, (double) blank);

  free (ud);
  free (vd);
}

/*
 * Build an occupancy mask, seed streamlines from the centre outward,
 * integrate each candidate in both directions, and draw those that
 * exceed the minimum length.
 */
static void
_giza_streamplot_core (int n, int m, const double *u, const double *v,
                       int i1, int i2, int j1, int j2, double density,
                       const double *affine, double blank)
{
  giza_stream_t s;
  giza_seed_t *seeds;
  int mx, my, nback, nfwd, ntot, i, nseed, iseed, nocc, oldBuf, narrow, oldTrans;
  double x0, y0, minlength, maxlength, length, cx, cy, dx, dy;
  double *xback, *yback, *xfwd, *yfwd, *xline, *yline;
  double *ax1, *ay1, *ax2, *ay2;
  int *occ;
  size_t nbuf;

  if (i1 < 0 || i2 >= n || i1 > i2)
    {
      _giza_error ("giza_streamplot",
                   "invalid index range for horizontal values");
      return;
    }
  if (j1 < 0 || j2 >= m || j1 > j2)
    {
      _giza_error ("giza_streamplot", "invalid index range for vertical values");
      return;
    }

  if (density < STREAM_DENSITY_MIN)
    density = STREAM_DENSITY_MIN;
  if (density > STREAM_DENSITY_MAX)
    density = STREAM_DENSITY_MAX;

  s.n = n;
  s.m = m;
  s.i1 = i1;
  s.i2 = i2;
  s.j1 = j1;
  s.j2 = j2;
  s.nx = i2 - i1 + 1;
  s.ny = j2 - j1 + 1;
  s.u = u;
  s.v = v;
  s.blank = blank;
  s.mask_nx = MAX (1, (int) (STREAM_MASK_BASE * density + 0.5));
  s.mask_ny = MAX (1, (int) (STREAM_MASK_BASE * density + 0.5));
  cairo_matrix_init (&s.mat, affine[0], affine[1], affine[2], affine[3],
                     affine[4], affine[5]);

  s.mask = calloc ((size_t) s.mask_nx * (size_t) s.mask_ny, 1);
  s.cur = calloc ((size_t) s.mask_nx * (size_t) s.mask_ny, 1);
  seeds = malloc ((size_t) s.mask_nx * (size_t) s.mask_ny * sizeof (giza_seed_t));
  if (s.mask == NULL || s.cur == NULL || seeds == NULL)
    {
      _giza_error ("giza_streamplot", "memory allocation failed");
      free (s.mask);
      free (s.cur);
      free (seeds);
      return;
    }

  nbuf = (size_t) STREAM_MAX_PTS;
  xback = malloc (nbuf * sizeof (double));
  yback = malloc (nbuf * sizeof (double));
  xfwd = malloc (nbuf * sizeof (double));
  yfwd = malloc (nbuf * sizeof (double));
  xline = malloc (nbuf * sizeof (double));
  yline = malloc (nbuf * sizeof (double));
  occ = malloc (nbuf * sizeof (int));
  ax1 = malloc ((size_t) s.mask_nx * (size_t) s.mask_ny * sizeof (double));
  ay1 = malloc ((size_t) s.mask_nx * (size_t) s.mask_ny * sizeof (double));
  ax2 = malloc ((size_t) s.mask_nx * (size_t) s.mask_ny * sizeof (double));
  ay2 = malloc ((size_t) s.mask_nx * (size_t) s.mask_ny * sizeof (double));
  if (xback == NULL || yback == NULL || xfwd == NULL || yfwd == NULL
      || xline == NULL || yline == NULL || occ == NULL
      || ax1 == NULL || ay1 == NULL || ax2 == NULL || ay2 == NULL)
    {
      _giza_error ("giza_streamplot", "memory allocation failed");
      free (xback);
      free (yback);
      free (xfwd);
      free (yfwd);
      free (xline);
      free (yline);
      free (occ);
      free (ax1);
      free (ay1);
      free (ax2);
      free (ay2);
      free (s.mask);
      free (s.cur);
      free (seeds);
      return;
    }

  /* keep short closed loops around small islands; reject only tiny stubs */
  minlength = 2.0 * (double) s.nx / (double) s.mask_nx;
  maxlength = STREAM_MAXLENGTH_FRAC * (double) MAX (s.nx, s.ny);

  /* seed from the centre outward so top/bottom (and left/right) are equivalent */
  cx = 0.5 * (double) (s.mask_nx - 1);
  cy = 0.5 * (double) (s.mask_ny - 1);
  nseed = 0;
  for (my = 0; my < s.mask_ny; my++)
    {
      for (mx = 0; mx < s.mask_nx; mx++)
        {
          dx = (double) mx - cx;
          dy = (double) my - cy;
          seeds[nseed].mx = mx;
          seeds[nseed].my = my;
          seeds[nseed].d2 = dx * dx + dy * dy;
          nseed++;
        }
    }
  qsort (seeds, (size_t) nseed, sizeof (giza_seed_t), _giza_seed_cmp);

  /* cairo-xlib pays for every cairo_stroke; append all lines then stroke once.
   * /osx rasterises locally so a per-line giza_line stroke is cheap there. */
  giza_get_buffering (&oldBuf);
  giza_begin_buffer ();
  oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  narrow = 0;

  for (iseed = 0; iseed < nseed; iseed++)
    {
      mx = seeds[iseed].mx;
      my = seeds[iseed].my;
      if (s.mask[my * s.mask_nx + mx])
        continue;

      x0 = (double) i1 + ((mx + 0.5) / (double) s.mask_nx) * (double) s.nx - 0.5;
      y0 = (double) j1 + ((my + 0.5) / (double) s.mask_ny) * (double) s.ny - 0.5;
      if (x0 < i1)
        x0 = (double) i1;
      if (x0 > i2)
        x0 = (double) i2;
      if (y0 < j1)
        y0 = (double) j1;
      if (y0 > j2)
        y0 = (double) j2;

      nocc = 0;
      nback = 0;
      nfwd = 0;
      _giza_stream_integrate (&s, x0, y0, -1, maxlength,
                              xback, yback, &nback, occ, &nocc, STREAM_MAX_PTS);
      _giza_stream_integrate (&s, x0, y0, +1, maxlength,
                              xfwd, yfwd, &nfwd, occ, &nocc, STREAM_MAX_PTS);

      /* stitch backward (reversed) then forward, without duplicating the seed */
      ntot = 0;
      for (i = nback - 1; i >= 1; i--)
        {
          xline[ntot] = xback[i];
          yline[ntot] = yback[i];
          ntot++;
        }
      if (nfwd > 0)
        {
          for (i = 0; i < nfwd && ntot < STREAM_MAX_PTS; i++)
            {
              xline[ntot] = xfwd[i];
              yline[ntot] = yfwd[i];
              ntot++;
            }
        }
      else if (nback > 0)
        {
          xline[ntot] = xback[0];
          yline[ntot] = yback[0];
          ntot++;
        }

      length = 0.;
      for (i = 1; i < ntot; i++)
        {
          length += hypot (xline[i] - xline[i - 1],
                           yline[i] - yline[i - 1]);
        }
      if (ntot >= 2 && length >= minlength)
        {
          /* only occupy the mask once the line is kept, so rejected
           * stubs do not leave holes */
          for (i = 0; i < nocc; i++)
            s.mask[occ[i]] = 1;
          if (_giza_stream_add_line (&s, xline, yline, ntot,
                                     &ax1[narrow], &ay1[narrow],
                                     &ax2[narrow], &ay2[narrow]))
            narrow++;
        }

      /* clear per-trajectory markers for the next seed attempt */
      for (i = 0; i < nocc; i++)
        s.cur[occ[i]] = 0;
    }

  _giza_stroke ();
  _giza_set_trans (oldTrans);
  for (i = 0; i < narrow; i++)
    giza_arrow (ax1[i], ay1[i], ax2[i], ay2[i]);

  if (!oldBuf)
    giza_end_buffer ();

  giza_flush_device ();
  free (xback);
  free (yback);
  free (xfwd);
  free (yfwd);
  free (xline);
  free (yline);
  free (occ);
  free (ax1);
  free (ay1);
  free (ax2);
  free (ay2);
  free (s.mask);
  free (s.cur);
  free (seeds);
}

/**
 * Internal: _giza_seed_cmp
 *
 * Synopsis: qsort comparator for seed points: sort by squared distance
 * from the mask centre, then by mx and my for deterministic tie-breaking.
 *
 * Input:
 *  -a, b :- Pointers to giza_seed_t records
 *
 * Returns: -1, 0, or 1 for qsort ordering
 */
static int
_giza_seed_cmp (const void *a, const void *b)
{
  const giza_seed_t *pa = (const giza_seed_t *) a;
  const giza_seed_t *pb = (const giza_seed_t *) b;

  if (pa->d2 < pb->d2)
    return -1;
  if (pa->d2 > pb->d2)
    return 1;
  /* same radius: compare |dx| then |dy| so opposite sides stay equivalent */
  if (pa->mx != pb->mx)
    return pa->mx - pb->mx;
  return pa->my - pb->my;
}

/**
 * Internal: _giza_stream_blank
 *
 * Synopsis: Return 1 if grid cell (i,j) is outside the plot range or both
 * components equal the blank sentinel.
 *
 * Input:
 *  -s   :- Streamplot context (grid bounds, u/v arrays, blank value)
 *  -i,j :- Grid indices
 *
 * Returns: 1 if blank/unusable, 0 otherwise
 */
static int
_giza_stream_blank (const giza_stream_t *s, int i, int j)
{
  double ui, vi;

  if (i < s->i1 || i > s->i2 || j < s->j1 || j > s->j2)
    return 1;
  ui = s->u[j * s->n + i];
  vi = s->v[j * s->n + i];
  return (_giza_equal (ui, s->blank) && _giza_equal (vi, s->blank));
}

/**
 * Internal: _giza_stream_sample
 *
 * Synopsis: Bilinear interpolation of u and v at fractional grid
 * coordinates (x,y).
 *
 * Input:
 *  -s           :- Streamplot context
 *  -x, y        :- Fractional grid coordinates
 *  -uout, vout  :- Output interpolated components
 *
 * Returns: 1 on success, 0 if out of range or any corner is blank
 */
static int
_giza_stream_sample (const giza_stream_t *s, double x, double y,
                     double *uout, double *vout)
{
  int i0, j0, i1, j1;
  double tx, ty, u00, u10, u01, u11, v00, v10, v01, v11;

  if (x < s->i1 || x > s->i2 || y < s->j1 || y > s->j2)
    return 0;

  i0 = (int) floor (x);
  j0 = (int) floor (y);
  if (i0 < s->i1)
    i0 = s->i1;
  if (j0 < s->j1)
    j0 = s->j1;
  if (i0 > s->i2)
    i0 = s->i2;
  if (j0 > s->j2)
    j0 = s->j2;
  i1 = i0 + 1;
  j1 = j0 + 1;
  if (i1 > s->i2)
    i1 = s->i2;
  if (j1 > s->j2)
    j1 = s->j2;

  if (_giza_stream_blank (s, i0, j0) || _giza_stream_blank (s, i1, j0)
      || _giza_stream_blank (s, i0, j1) || _giza_stream_blank (s, i1, j1))
    return 0;

  tx = x - (double) i0;
  ty = y - (double) j0;
  if (tx < 0.)
    tx = 0.;
  if (tx > 1.)
    tx = 1.;
  if (ty < 0.)
    ty = 0.;
  if (ty > 1.)
    ty = 1.;

  u00 = s->u[j0 * s->n + i0];
  u10 = s->u[j0 * s->n + i1];
  u01 = s->u[j1 * s->n + i0];
  u11 = s->u[j1 * s->n + i1];
  v00 = s->v[j0 * s->n + i0];
  v10 = s->v[j0 * s->n + i1];
  v01 = s->v[j1 * s->n + i0];
  v11 = s->v[j1 * s->n + i1];

  *uout = (1. - tx) * (1. - ty) * u00 + tx * (1. - ty) * u10
          + (1. - tx) * ty * u01 + tx * ty * u11;
  *vout = (1. - tx) * (1. - ty) * v00 + tx * (1. - ty) * v10
          + (1. - tx) * ty * v01 + tx * ty * v11;
  return 1;
}

/**
 * Internal: _giza_stream_in_mask
 *
 * Synopsis: Map fractional grid coordinates to the occupancy mask cell
 * indices (mx, my), clamped to the mask bounds.
 *
 * Input:
 *  -s        :- Streamplot context
 *  -x, y     :- Fractional grid coordinates
 *  -mx, my   :- Output mask cell indices
 *
 * Returns: 1 (always succeeds after clamping)
 */
static int
_giza_stream_in_mask (const giza_stream_t *s, double x, double y,
                      int *mx, int *my)
{
  *mx = (int) floor ((x - (double) s->i1 + 0.5) / (double) s->nx * s->mask_nx);
  *my = (int) floor ((y - (double) s->j1 + 0.5) / (double) s->ny * s->mask_ny);
  if (*mx < 0)
    *mx = 0;
  if (*my < 0)
    *my = 0;
  if (*mx >= s->mask_nx)
    *mx = s->mask_nx - 1;
  if (*my >= s->mask_ny)
    *my = s->mask_ny - 1;
  return 1;
}

/**
 * Internal: _giza_stream_integrate
 *
 * Synopsis: Integrate one streamline half with adaptive RK2 (Heun) steps
 * along the given direction, tracking occupancy cells in occ[] and cur[].
 *
 * Input:
 *  -s          :- Streamplot context
 *  -x0, y0     :- Starting position in grid coordinates
 *  -direction  :- +1 forward or -1 backward along the field
 *  -maxlength  :- Maximum arc length in grid units
 *  -xpts, ypts :- Output trajectory coordinates
 *  -npts       :- Number of points written (updated)
 *  -occ, nocc  :- Mask cells visited on this trajectory
 *  -noccmax    :- Maximum occ[] entries
 *
 * Returns: final npts value
 */
static int
_giza_stream_integrate (giza_stream_t *s, double x0, double y0, int direction,
                        double maxlength, double *xpts, double *ypts,
                        int *npts, int *occ, int *nocc, int noccmax)
{
  double x, y, u, v, speed, k1x, k1y, k2x, k2y, length;
  double ds, maxds, dsmin, error, dx1, dy1, dx2, dy2, nx, ny;
  int mx, my, k, mxprev, myprev, nreject, nsame;

  /* cap the step at one occupancy cell so trajectories cannot skip mask cells */
  nx = (double) s->nx;
  ny = (double) s->ny;
  maxds = nx / (double) s->mask_nx;
  if (ny / (double) s->mask_ny < maxds)
    maxds = ny / (double) s->mask_ny;
  if (maxds > STREAM_MAXDS_AXES * MIN (nx, ny))
    maxds = STREAM_MAXDS_AXES * MIN (nx, ny);
  if (maxds < 1.0e-6)
    maxds = 1.0e-6;
  /* keep steps a reasonable fraction of an occupancy cell; tiny adaptive
   * steps on noisy fields pack thousands of segments into one cell */
  dsmin = 0.25 * maxds;
  if (dsmin < 1.0e-6)
    dsmin = 1.0e-6;
  ds = maxds;

  x = x0;
  y = y0;
  length = 0.;
  mxprev = -1;
  myprev = -1;
  nreject = 0;
  nsame = 0;

  while (*npts < STREAM_MAX_PTS && length < maxlength)
    {
      if (!_giza_stream_sample (s, x, y, &u, &v))
        break;
      speed = hypot (u, v);
      if (speed < GIZA_ZERO_DOUBLE)
        break;

      _giza_stream_in_mask (s, x, y, &mx, &my);
      k = my * s->mask_nx + mx;
      if (mx != mxprev || my != myprev)
        {
          /* stop on a cell already used by this trajectory or a previous
           * line, otherwise closed orbits retrace themselves until maxlength */
          if (*npts > 0 && (s->mask[k] || s->cur[k]))
            break;
          if (!s->cur[k] && *nocc < noccmax)
            {
              s->cur[k] = 1;
              occ[(*nocc)++] = k;
            }
          mxprev = mx;
          myprev = my;
          nsame = 0;
        }
      else if (nreject == 0)
        {
          /* count accepted steps that stay in this cell, not rejected retries */
          nsame++;
          if (nsame > STREAM_MAX_SAME_CELL)
            break;
        }

      /* record this position once; rejected steps retry from the same point */
      if (nreject == 0)
        {
          xpts[*npts] = x;
          ypts[*npts] = y;
          (*npts)++;
        }

      k1x = direction * ds * u / speed;
      k1y = direction * ds * v / speed;
      if (!_giza_stream_sample (s, x + k1x, y + k1y, &u, &v))
        break;
      speed = hypot (u, v);
      if (speed < GIZA_ZERO_DOUBLE)
        break;
      k2x = direction * ds * u / speed;
      k2y = direction * ds * v / speed;

      dx1 = k1x;
      dy1 = k1y;
      dx2 = 0.5 * (k1x + k2x);
      dy2 = 0.5 * (k1y + k2y);
      /* Euler vs Heun disagreement, as a fraction of the data grid */
      error = hypot ((dx2 - dx1) / nx, (dy2 - dy1) / ny);

      if (error < STREAM_MAXERROR)
        {
          x += dx2;
          y += dy2;
          length += ds;
          nreject = 0;
          if (error < GIZA_ZERO_DOUBLE)
            ds = maxds;
          else
            {
              ds = 0.85 * ds * sqrt (STREAM_MAXERROR / error);
              if (ds > maxds)
                ds = maxds;
            }
        }
      else
        {
          nreject++;
          ds = 0.85 * ds * sqrt (STREAM_MAXERROR / error);
          if (ds > maxds)
            ds = maxds;
          if (ds < dsmin || nreject > STREAM_MAX_REJECT)
            break;
        }
    }

  return *npts;
}

/**
 * Internal: _giza_stream_arrow_index
 *
 * Synopsis: Choose the segment index for the direction arrow using
 * arc length along the drawn line, with a per-seed phase hash so nested
 * orbits do not align their arrows radially.
 *
 * Input:
 *  -xw, yw :- World-coordinate polyline
 *  -npts   :- Number of vertices
 *  -x0, y0 :- Seed position in grid coordinates (for phase hash)
 *
 * Returns: segment index imid with 1 <= imid < npts
 */
static int
_giza_stream_arrow_index (const double *xw, const double *yw, int npts,
                          double x0, double y0)
{
  double total, target, acc, t;
  int i, imid;

  if (npts < 2)
    return 1;

  total = 0.;
  for (i = 1; i < npts; i++)
    total += hypot (xw[i] - xw[i - 1], yw[i] - yw[i - 1]);
  if (total < GIZA_ZERO_DOUBLE)
    return 1;

  /* scatter the arrow along the line so nested orbits do not share a phase */
  t = x0 * 0.7548776662466927 + y0 * 0.5698402909980532;
  t -= floor (t);
  if (t < 0.)
    t += 1.;
  target = (0.15 + 0.70 * t) * total;

  acc = 0.;
  imid = 1;
  for (i = 1; i < npts; i++)
    {
      acc += hypot (xw[i] - xw[i - 1], yw[i] - yw[i - 1]);
      imid = i;
      if (acc >= target)
        break;
    }
  return imid;
}

/**
 * Internal: _giza_stream_add_line
 *
 * Synopsis: Transform a grid-coordinate polyline to world space, append it
 * as a cairo subpath (not stroked), and optionally return a direction arrow.
 * Callers should stroke once after all lines are appended: cairo-xlib is
 * dominated by cairo_stroke, not by giza's XFlush buffering.
 *
 * Input:
 *  -s          :- Streamplot context (affine matrix)
 *  -xg, yg     :- Trajectory in grid coordinates
 *  -npts       :- Number of vertices
 *  -xa,ya,xb,yb:- Output arrow endpoints in world coordinates
 *
 * Returns: 1 if an arrow should be drawn, 0 otherwise
 */
static int
_giza_stream_add_line (giza_stream_t *s, const double *xg, const double *yg,
                       int npts, double *xa, double *ya, double *xb, double *yb)
{
  double *xw, *yw;
  double dx, dy, len, alen, ux, uy;
  int i, imid;

  if (npts < 2)
    return 0;

  xw = malloc ((size_t) npts * sizeof (double));
  yw = malloc ((size_t) npts * sizeof (double));
  if (xw == NULL || yw == NULL)
    {
      _giza_error ("giza_streamplot", "memory allocation failed");
      free (xw);
      free (yw);
      return 0;
    }

  for (i = 0; i < npts; i++)
    {
      xw[i] = xg[i] + 0.5;
      yw[i] = yg[i] + 0.5;
      cairo_matrix_transform_point (&s->mat, &xw[i], &yw[i]);
    }

  cairo_move_to (Dev[id].context, xw[0], yw[0]);
  for (i = 1; i < npts; i++)
    cairo_line_to (Dev[id].context, xw[i], yw[i]);

  imid = _giza_stream_arrow_index (xw, yw, npts, xg[0], yg[0]);
  dx = xw[imid] - xw[imid - 1];
  dy = yw[imid] - yw[imid - 1];
  len = hypot (dx, dy);
  if (len < GIZA_ZERO_DOUBLE)
    {
      free (xw);
      free (yw);
      return 0;
    }
  ux = dx / len;
  uy = dy / len;
  /* arrow length of about 0.8 data-cell widths in world coordinates */
  alen = 0.8 * hypot (s->mat.xx, s->mat.yx);
  if (alen < GIZA_ZERO_DOUBLE)
    alen = len;
  *xa = xw[imid] - 0.5 * alen * ux;
  *ya = yw[imid] - 0.5 * alen * uy;
  *xb = xw[imid] + 0.5 * alen * ux;
  *yb = yw[imid] + 0.5 * alen * uy;

  free (xw);
  free (yw);
  return 1;
}
