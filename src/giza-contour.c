/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
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
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

/*
 * The guts of this routine were taken from conrec.c
 * as written by Paul Bourke.
 */

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <stdlib.h> /* for abs() */
#include <math.h>
#include <string.h>


void
giza_contour (int sizex, int sizey, const double* data, int i1,
             int i2, int j1, int j2, int ncont_in, const double* cont,
             const double *affine)
{
#define xsect(p1,p2) (h[p2]*xh[p1]-h[p1]*xh[p2])/(h[p2]-h[p1])
#define ysect(p1,p2) (h[p2]*yh[p1]-h[p1]*yh[p2])/(h[p2]-h[p1])

  if (!_giza_check_device_ready ("giza_contour"))
    return;

  cairo_matrix_t mat;
  int sh[5];
  double h[5];
  double xh[5], yh[5];
  int im[4] = { 0, 1, 1, 0 };
  int jm[4] = { 0, 0, 1, 1 };
  double temp1, temp2, dmin, dmax, x1 = 0., x2 = 0., y1 = 0., y2 = 0.;
  int i, j, k, m;
  int m1, m2, m3, case_value;
  int castab[3][3][3] = {
    {{0, 0, 8}, {0, 2, 5}, {7, 6, 9}},
    {{0, 3, 4}, {1, 3, 1}, {4, 3, 0}},
    {{9, 6, 7}, {5, 2, 0}, {8, 0, 0}}
  };

  /* start buffering */
  int oldBuf;
  giza_get_buffering(&oldBuf);
  giza_begin_buffer ();

  /* Get the affine matrix ready */
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                   affine[4], affine[5]);
  cairo_transform (Dev[id].context, &mat);
  cairo_get_matrix (Dev[id].context, &mat);
  _giza_set_trans (GIZA_TRANS_IDEN);

  /* set up the line style */
  int ls;
  int       curls, newls;
  const int ncont      = abs(ncont_in);
  const int auto_style = (ncont_in > 0);
  giza_get_line_style (&ls);
  curls = ls;
  giza_set_line_style (curls);

  for (j = (j2 - 1); j >= j1; j--)
    {
      for (i = i1; i < i2; i++)
       {
         /* find the minimum value at the corners of a 1x1 box */
         temp1 = MIN (data[j*sizex+i], data[(j + 1)*sizex+i]);
         temp2 = MIN (data[j*sizex+(i + 1)], data[(j + 1)*sizex+(i + 1)]);
         dmin = MIN (temp1, temp2);

         /* find the max value at the corners of the 1x1 box */
         temp1 = MAX (data[j*sizex+i], data[(j + 1)*sizex+i]);
         temp2 = MAX (data[j*sizex+(i + 1)], data[(j + 1)*sizex+(i + 1)]);
         dmax = MAX (temp1, temp2);

         /* Check it is in the range of a contour */
         if (dmax < cont[0] || dmin > cont[ncont - 1])
           continue;

         for (k = 0; k < ncont; ++k)
           {
             if (cont[k] < dmin || cont[k] > dmax)
              continue;
             for (m = 4; m >= 0; m--)
              {
                /* calculate the relative height of the four corners and centre of the square */
                /* (i, j)   (i+1, j) */
                /* (i, j+1) (i+1, j+1) */
                /* as well as their device co-ords */
                if (m > 0)
                  {
                    h[m] = data[(j + jm[m - 1])*sizex+(i + im[m - 1])] - cont[k];
                    xh[m] = i + im[m - 1] + 0.5;
                    yh[m] = j + jm[m - 1] + 0.5;
                  }
                else
                  {
                    h[0] = 0.25 * (h[1] + h[2] + h[3] + h[4]);
                    xh[0] = 0.5 * (i + i + 1) + 0.5;
                    yh[0] = 0.5 * (j + j + 1) + 0.5;
                  }

                if (h[m] > 0.0)
                  sh[m] = 1;
                else if (h[m] < 0.0)
                  sh[m] = -1;
                else
                  sh[m] = 0;
              }

             for (m = 1; m <= 4; ++m)
              {
                m1 = m;
                m2 = 0;
                if (m != 4)
                  m3 = m + 1;
                else
                  m3 = 1;

                if ((case_value =
                     castab[sh[m1] + 1][sh[m2] + 1][sh[m3] + 1]) == 0)
                  continue;

                switch (case_value)
                  {
                  case 1:       /* Line between (i, j) (i + 1, j) */
                    x1 = xh[m1];
                    y1 = yh[m1];
                    x2 = xh[m2];
                    y2 = yh[m2];
                    break;
                  case 2:       /* Line between (i + 1, j) (i + 1, j + 1) */
                    x1 = xh[m2];
                    y1 = yh[m2];
                    x2 = xh[m3];
                    y2 = yh[m3];
                    break;
                  case 3:       /* Line between (i, j) (i + 1, j + 1) */
                    x1 = xh[m3];
                    y1 = yh[m3];
                    x2 = xh[m1];
                    y2 = yh[m1];
                    break;
                  case 4:       /* Line between (i, j) and between (i + 1, j), (i + 1, j + 1) */
                    x1 = xh[m1];
                    y1 = yh[m1];
                    x2 = xsect (m2, m3);
                    y2 = ysect (m2, m3);
                    break;
                  case 5:       /* Line between (i + 1, j) and between (i, j), (i + 1, j + 1) */
                    x1 = xh[m2];
                    y1 = yh[m2];
                    x2 = xsect (m3, m1);
                    y2 = ysect (m3, m1);
                    break;
                  case 6:       /* Line between (i + 1, j + 1) and between (i, j), (i + 1, j + 1)  */
                    x1 = xh[m3];
                    y1 = yh[m3];
                    x2 = xsect (m3, m2);
                    y2 = ysect (m3, m2);
                    break;
                  case 7:       /* Line between sides 1-2 and 2-3  */
                    x1 = xsect (m1, m2);
                    y1 = ysect (m1, m2);
                    x2 = xsect (m2, m3);
                    y2 = ysect (m2, m3);
                    break;
                  case 8:       /* Line between sides 2-3 and 3-1 */
                    x1 = xsect (m2, m3);
                    y1 = ysect (m2, m3);
                    x2 = xsect (m3, m1);
                    y2 = ysect (m3, m1);
                    break;
                  case 9:       /* Line between sides 3-1 and 1-2 */
                    x1 = xsect (m3, m1);
                    y1 = ysect (m3, m1);
                    x2 = xsect (m1, m2);
                    y2 = ysect (m1, m2);
                    break;
                  default:
                    break;
                  }
                  /* compute what the line style should be */
                  newls = (auto_style ? (cont[k]<0 ? GIZA_LS_DOT : GIZA_LS_SOLID) : ls);
                  if( newls!=curls ) {
                      curls = newls;
                      giza_set_line_style (curls);
                  }
                /* draw the line */
                  cairo_matrix_transform_point (&mat, &x1, &y1);
                  cairo_matrix_transform_point (&mat, &x2, &y2);
                  /*printf ("In index: (x1, y1) = (%f, %f)\n", x1, y1); */
                  /*printf ("In coord: (x1, y1) = (%f, %f)\n", x1, y1); */
                  cairo_move_to (Dev[id].context, x1, y1);
                cairo_line_to (Dev[id].context, x2, y2);
                cairo_stroke (Dev[id].context);
              }
           }
       }
    }

  /* restore the transformation */
  _giza_set_trans (oldTrans);

  /* make sure line style returned to previous value */
  giza_set_line_style (ls);

  /* restore buffering and stroke */
  if (!oldBuf)
    giza_end_buffer ();

  giza_flush_device ();
}

void
giza_contour_float (int sizex, int sizey, const float* data, int i1,
             int i2, int j1, int j2, int ncont, const float* cont, const float *affine)
{
  double ddata[sizey*sizex];
  double dcont[abs(ncont)];
  double daffine[6];
  int i, j;

  for (j=j1; j<=j2; j++) {
      for (i=i1; i<=i2; i++) {
          ddata[j*sizex+i] = (double) data[j*sizex+i];
      }
  }

  for (i=0; i<abs(ncont); i++) {
      dcont[i] = (double) cont[i];
  }

  for (i=0; i<6; i++) {
     daffine[i] = (double) affine[i];
  }

  giza_contour (sizex, sizey, ddata, i1, i2, j1, j2, ncont, dcont, daffine);

}

/*
 * giza_contour_blanked -- contour with blanking
 *
 * Same as giza_contour but skips cells where any corner value equals blank.
 */
void
giza_contour_blanked (int sizex, int sizey, const double* data, int i1,
             int i2, int j1, int j2, int ncont_in, const double* cont,
             const double *affine, double blank)
{
#define xsect_b(p1,p2) (hb[p2]*xhb[p1]-hb[p1]*xhb[p2])/(hb[p2]-hb[p1])
#define ysect_b(p1,p2) (hb[p2]*yhb[p1]-hb[p1]*yhb[p2])/(hb[p2]-hb[p1])

  if (!_giza_check_device_ready ("giza_contour_blanked"))
    return;

  cairo_matrix_t mat;
  int sh[5];
  double hb[5];
  double xhb[5], yhb[5];
  int im[4] = { 0, 1, 1, 0 };
  int jm[4] = { 0, 0, 1, 1 };
  double temp1, temp2, dmin, dmax, x1 = 0., x2 = 0., y1 = 0., y2 = 0.;
  int i, j, k, m;
  int m1, m2, m3, case_value;
  int castab[3][3][3] = {
    {{0, 0, 8}, {0, 2, 5}, {7, 6, 9}},
    {{0, 3, 4}, {1, 3, 1}, {4, 3, 0}},
    {{9, 6, 7}, {5, 2, 0}, {8, 0, 0}}
  };

  int oldBuf;
  giza_get_buffering(&oldBuf);
  giza_begin_buffer ();

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                   affine[4], affine[5]);
  cairo_transform (Dev[id].context, &mat);
  cairo_get_matrix (Dev[id].context, &mat);
  _giza_set_trans (GIZA_TRANS_IDEN);

  int ls;
  int       curls, newls;
  const int ncont      = abs(ncont_in);
  const int auto_style = (ncont_in > 0);
  giza_get_line_style (&ls);
  curls = ls;
  giza_set_line_style (curls);

  for (j = (j2 - 1); j >= j1; j--)
    {
      for (i = i1; i < i2; i++)
       {
         /* skip cells where any corner is blanked */
         double d00 = data[j*sizex+i];
         double d10 = data[j*sizex+(i+1)];
         double d01 = data[(j+1)*sizex+i];
         double d11 = data[(j+1)*sizex+(i+1)];
         if (d00 == blank || d10 == blank || d01 == blank || d11 == blank)
           continue;

         temp1 = MIN (d00, d01);
         temp2 = MIN (d10, d11);
         dmin = MIN (temp1, temp2);

         temp1 = MAX (d00, d01);
         temp2 = MAX (d10, d11);
         dmax = MAX (temp1, temp2);

         if (dmax < cont[0] || dmin > cont[ncont - 1])
           continue;

         for (k = 0; k < ncont; ++k)
           {
             if (cont[k] < dmin || cont[k] > dmax)
              continue;
             for (m = 4; m >= 0; m--)
              {
                if (m > 0)
                  {
                    hb[m] = data[(j + jm[m - 1])*sizex+(i + im[m - 1])] - cont[k];
                    xhb[m] = i + im[m - 1] + 0.5;
                    yhb[m] = j + jm[m - 1] + 0.5;
                  }
                else
                  {
                    hb[0] = 0.25 * (hb[1] + hb[2] + hb[3] + hb[4]);
                    xhb[0] = 0.5 * (i + i + 1) + 0.5;
                    yhb[0] = 0.5 * (j + j + 1) + 0.5;
                  }

                if (hb[m] > 0.0)
                  sh[m] = 1;
                else if (hb[m] < 0.0)
                  sh[m] = -1;
                else
                  sh[m] = 0;
              }

             for (m = 1; m <= 4; ++m)
              {
                m1 = m;
                m2 = 0;
                if (m != 4)
                  m3 = m + 1;
                else
                  m3 = 1;

                if ((case_value =
                     castab[sh[m1] + 1][sh[m2] + 1][sh[m3] + 1]) == 0)
                  continue;

                switch (case_value)
                  {
                  case 1: x1=xhb[m1]; y1=yhb[m1]; x2=xhb[m2]; y2=yhb[m2]; break;
                  case 2: x1=xhb[m2]; y1=yhb[m2]; x2=xhb[m3]; y2=yhb[m3]; break;
                  case 3: x1=xhb[m3]; y1=yhb[m3]; x2=xhb[m1]; y2=yhb[m1]; break;
                  case 4: x1=xhb[m1]; y1=yhb[m1]; x2=xsect_b(m2,m3); y2=ysect_b(m2,m3); break;
                  case 5: x1=xhb[m2]; y1=yhb[m2]; x2=xsect_b(m3,m1); y2=ysect_b(m3,m1); break;
                  case 6: x1=xhb[m3]; y1=yhb[m3]; x2=xsect_b(m3,m2); y2=ysect_b(m3,m2); break;
                  case 7: x1=xsect_b(m1,m2); y1=ysect_b(m1,m2); x2=xsect_b(m2,m3); y2=ysect_b(m2,m3); break;
                  case 8: x1=xsect_b(m2,m3); y1=ysect_b(m2,m3); x2=xsect_b(m3,m1); y2=ysect_b(m3,m1); break;
                  case 9: x1=xsect_b(m3,m1); y1=ysect_b(m3,m1); x2=xsect_b(m1,m2); y2=ysect_b(m1,m2); break;
                  default: break;
                  }
                  newls = (auto_style ? (cont[k]<0 ? GIZA_LS_DOT : GIZA_LS_SOLID) : ls);
                  if( newls!=curls ) {
                      curls = newls;
                      giza_set_line_style (curls);
                  }
                  cairo_matrix_transform_point (&mat, &x1, &y1);
                  cairo_matrix_transform_point (&mat, &x2, &y2);
                  cairo_move_to (Dev[id].context, x1, y1);
                  cairo_line_to (Dev[id].context, x2, y2);
                  cairo_stroke (Dev[id].context);
              }
           }
       }
    }

  _giza_set_trans (oldTrans);
  giza_set_line_style (ls);
  if (!oldBuf) giza_end_buffer ();
  giza_flush_device ();

#undef xsect_b
#undef ysect_b
}

void
giza_contour_blanked_float (int sizex, int sizey, const float* data, int i1,
             int i2, int j1, int j2, int ncont, const float* cont,
             const float *affine, float blank)
{
  double *ddata = malloc(sizeof(double) * sizex * sizey);
  double *dcont = malloc(sizeof(double) * abs(ncont));
  double daffine[6];
  int i, j;

  if (!ddata || !dcont) {
    _giza_warning("giza_contour_blanked_float", "memory allocation failed");
    free(ddata);
    free(dcont);
    return;
  }

  for (j=j1; j<=j2; j++) {
      for (i=i1; i<=i2; i++) {
          ddata[j*sizex+i] = (double) data[j*sizex+i];
      }
  }

  for (i=0; i<abs(ncont); i++) {
      dcont[i] = (double) cont[i];
  }

  for (i=0; i<6; i++) {
     daffine[i] = (double) affine[i];
  }

  giza_contour_blanked (sizex, sizey, ddata, i1, i2, j1, j2, ncont, dcont, daffine,
                        (double) blank);
  free(ddata);
  free(dcont);
}

/*
 * giza_contour_fill -- fill between two contour levels
 *
 * Uses marching squares to fill the region where c1 <= data <= c2
 * for each grid cell. Linear interpolation along edges.
 */

/* Helper: interpolate along an edge between two grid values */
static double _interp_frac(double v1, double v2, double level)
{
  if (fabs(v2 - v1) < 1.0e-30) return 0.5;
  return (level - v1) / (v2 - v1);
}

void
giza_contour_fill (int sizex, int sizey, const double* data, int i1,
             int i2, int j1, int j2, double c1, double c2,
             const double *affine)
{
  if (!_giza_check_device_ready ("giza_contour_fill"))
    return;

  cairo_matrix_t mat;

  int oldBuf;
  giza_get_buffering(&oldBuf);
  giza_begin_buffer ();

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                   affine[4], affine[5]);
  cairo_transform (Dev[id].context, &mat);
  cairo_get_matrix (Dev[id].context, &mat);
  _giza_set_trans (GIZA_TRANS_IDEN);

  int i, j;
  for (j = j1; j < j2; j++)
    {
      for (i = i1; i < i2; i++)
        {
          /* Grid cell corners: (i,j), (i+1,j), (i+1,j+1), (i,j+1) */
          double d[4];
          d[0] = data[j*sizex+i];         /* bottom-left  */
          d[1] = data[j*sizex+(i+1)];     /* bottom-right */
          d[2] = data[(j+1)*sizex+(i+1)]; /* top-right    */
          d[3] = data[(j+1)*sizex+i];     /* top-left     */

          /* Corner positions in grid coordinates (0.5 offset for pixel centers) */
          double cx[4], cy[4];
          cx[0] = i + 0.5;     cy[0] = j + 0.5;
          cx[1] = i + 1 + 0.5; cy[1] = j + 0.5;
          cx[2] = i + 1 + 0.5; cy[2] = j + 1 + 0.5;
          cx[3] = i + 0.5;     cy[3] = j + 1 + 0.5;

          /* Build a polygon representing the region c1 <= data <= c2 in this cell.
           * Walk around the four edges. For each edge (from corner A to corner B):
           *   - If A is inside [c1,c2], emit A
           *   - If crossing c1 or c2 boundary, emit intersection point
           */
          double px[16], py[16];
          int np = 0;

          int e;
          for (e = 0; e < 4; e++)
            {
              int e2 = (e + 1) % 4;
              double va = d[e], vb = d[e2];
              int a_in = (va >= c1 && va <= c2);
              int b_in = (vb >= c1 && vb <= c2);

              if (a_in && np < 16) {
                px[np] = cx[e]; py[np] = cy[e]; np++;
              }

              /* Check crossings of c1 and c2 along this edge */
              /* We need to emit intersections in order from A to B */
              double t_c1 = -1.0, t_c2 = -1.0;

              if ((va < c1 && vb > c1) || (va > c1 && vb < c1))
                t_c1 = _interp_frac(va, vb, c1);
              if ((va < c2 && vb > c2) || (va > c2 && vb < c2))
                t_c2 = _interp_frac(va, vb, c2);

              /* Emit in order of parameter t */
              double t1, t2;
              if (t_c1 >= 0.0 && t_c2 >= 0.0) {
                if (t_c1 < t_c2) { t1=t_c1; t2=t_c2; }
                else              { t1=t_c2; t2=t_c1; }
                if (np < 16) {
                  px[np] = cx[e] + t1*(cx[e2]-cx[e]);
                  py[np] = cy[e] + t1*(cy[e2]-cy[e]);
                  np++;
                }
                if (np < 16) {
                  px[np] = cx[e] + t2*(cx[e2]-cx[e]);
                  py[np] = cy[e] + t2*(cy[e2]-cy[e]);
                  np++;
                }
              } else if (t_c1 >= 0.0 && np < 16) {
                px[np] = cx[e] + t_c1*(cx[e2]-cx[e]);
                py[np] = cy[e] + t_c1*(cy[e2]-cy[e]);
                np++;
              } else if (t_c2 >= 0.0 && np < 16) {
                px[np] = cx[e] + t_c2*(cx[e2]-cx[e]);
                py[np] = cy[e] + t_c2*(cy[e2]-cy[e]);
                np++;
              }
            }

          if (np >= 3)
            {
              /* Transform polygon to device coords and fill */
              int p;
              double tx, ty;
              tx = px[0]; ty = py[0];
              cairo_matrix_transform_point(&mat, &tx, &ty);
              cairo_move_to(Dev[id].context, tx, ty);
              for (p = 1; p < np; p++) {
                tx = px[p]; ty = py[p];
                cairo_matrix_transform_point(&mat, &tx, &ty);
                cairo_line_to(Dev[id].context, tx, ty);
              }
              cairo_close_path(Dev[id].context);
            }
        }
    }

  cairo_fill(Dev[id].context);

  _giza_set_trans (oldTrans);
  if (!oldBuf) giza_end_buffer ();
  giza_flush_device ();
}

void
giza_contour_fill_float (int sizex, int sizey, const float* data, int i1,
             int i2, int j1, int j2, float c1, float c2, const float *affine)
{
  double *ddata = malloc(sizeof(double) * sizex * sizey);
  double daffine[6];
  int i, j;

  if (!ddata) {
    _giza_warning("giza_contour_fill_float", "memory allocation failed");
    return;
  }

  for (j=j1; j<=j2; j++) {
      for (i=i1; i<=i2; i++) {
          ddata[j*sizex+i] = (double) data[j*sizex+i];
      }
  }

  for (i=0; i<6; i++) {
     daffine[i] = (double) affine[i];
  }

  giza_contour_fill (sizex, sizey, ddata, i1, i2, j1, j2,
                     (double) c1, (double) c2, daffine);
  free(ddata);
}

/*
 * giza_contour_labelled -- contour with labels
 *
 * Traces a single contour level and places text labels along the contour.
 * intval: spacing in grid cells between labels
 * minint: minimum grid cells between labels
 * label: text string to draw
 */

/* Segment storage for contour tracing */
typedef struct {
  double x1, y1, x2, y2;
} _giza_seg;

void
giza_contour_labelled (int sizex, int sizey, const double* data, int i1,
             int i2, int j1, int j2, double c,
             const double *affine, const char *label,
             int intval, int minint)
{
#define xsect_l(p1,p2) (hl[p2]*xhl[p1]-hl[p1]*xhl[p2])/(hl[p2]-hl[p1])
#define ysect_l(p1,p2) (hl[p2]*yhl[p1]-hl[p1]*yhl[p2])/(hl[p2]-hl[p1])

  if (!_giza_check_device_ready ("giza_contour_labelled"))
    return;

  if (intval <= 0) intval = 20;
  if (minint <= 0) minint = 10;

  cairo_matrix_t mat;
  int sh[5];
  double hl[5];
  double xhl[5], yhl[5];
  int im[4] = { 0, 1, 1, 0 };
  int jm[4] = { 0, 0, 1, 1 };
  double temp1, temp2, dmin, dmax, x1 = 0., x2 = 0., y1 = 0., y2 = 0.;
  int i, j, m;
  int m1, m2, m3, case_value;
  int castab[3][3][3] = {
    {{0, 0, 8}, {0, 2, 5}, {7, 6, 9}},
    {{0, 3, 4}, {1, 3, 1}, {4, 3, 0}},
    {{9, 6, 7}, {5, 2, 0}, {8, 0, 0}}
  };

  int oldBuf;
  giza_get_buffering(&oldBuf);
  giza_begin_buffer ();

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                   affine[4], affine[5]);
  cairo_transform (Dev[id].context, &mat);
  cairo_get_matrix (Dev[id].context, &mat);
  _giza_set_trans (GIZA_TRANS_IDEN);

  /* Collect all contour segments for this level */
  int nseg = 0;
  int maxseg = (i2 - i1) * (j2 - j1) * 4;
  if (maxseg < 64) maxseg = 64;
  _giza_seg *segs = (_giza_seg *) malloc(maxseg * sizeof(_giza_seg));
  if (!segs) {
    _giza_set_trans(oldTrans);
    if (!oldBuf) giza_end_buffer();
    return;
  }

  for (j = (j2 - 1); j >= j1; j--)
    {
      for (i = i1; i < i2; i++)
       {
         temp1 = MIN (data[j*sizex+i], data[(j + 1)*sizex+i]);
         temp2 = MIN (data[j*sizex+(i + 1)], data[(j + 1)*sizex+(i + 1)]);
         dmin = MIN (temp1, temp2);

         temp1 = MAX (data[j*sizex+i], data[(j + 1)*sizex+i]);
         temp2 = MAX (data[j*sizex+(i + 1)], data[(j + 1)*sizex+(i + 1)]);
         dmax = MAX (temp1, temp2);

         if (dmax < c || dmin > c)
           continue;

         for (m = 4; m >= 0; m--)
           {
             if (m > 0)
               {
                 hl[m] = data[(j + jm[m - 1])*sizex+(i + im[m - 1])] - c;
                 xhl[m] = i + im[m - 1] + 0.5;
                 yhl[m] = j + jm[m - 1] + 0.5;
               }
             else
               {
                 hl[0] = 0.25 * (hl[1] + hl[2] + hl[3] + hl[4]);
                 xhl[0] = 0.5 * (i + i + 1) + 0.5;
                 yhl[0] = 0.5 * (j + j + 1) + 0.5;
               }
             if (hl[m] > 0.0) sh[m] = 1;
             else if (hl[m] < 0.0) sh[m] = -1;
             else sh[m] = 0;
           }

         for (m = 1; m <= 4; ++m)
           {
             m1 = m; m2 = 0;
             m3 = (m != 4) ? m + 1 : 1;

             if ((case_value = castab[sh[m1]+1][sh[m2]+1][sh[m3]+1]) == 0)
               continue;

             switch (case_value)
               {
               case 1: x1=xhl[m1]; y1=yhl[m1]; x2=xhl[m2]; y2=yhl[m2]; break;
               case 2: x1=xhl[m2]; y1=yhl[m2]; x2=xhl[m3]; y2=yhl[m3]; break;
               case 3: x1=xhl[m3]; y1=yhl[m3]; x2=xhl[m1]; y2=yhl[m1]; break;
               case 4: x1=xhl[m1]; y1=yhl[m1]; x2=xsect_l(m2,m3); y2=ysect_l(m2,m3); break;
               case 5: x1=xhl[m2]; y1=yhl[m2]; x2=xsect_l(m3,m1); y2=ysect_l(m3,m1); break;
               case 6: x1=xhl[m3]; y1=yhl[m3]; x2=xsect_l(m3,m2); y2=ysect_l(m3,m2); break;
               case 7: x1=xsect_l(m1,m2); y1=ysect_l(m1,m2); x2=xsect_l(m2,m3); y2=ysect_l(m2,m3); break;
               case 8: x1=xsect_l(m2,m3); y1=ysect_l(m2,m3); x2=xsect_l(m3,m1); y2=ysect_l(m3,m1); break;
               case 9: x1=xsect_l(m3,m1); y1=ysect_l(m3,m1); x2=xsect_l(m1,m2); y2=ysect_l(m1,m2); break;
               default: break;
               }

             if (nseg < maxseg) {
               segs[nseg].x1 = x1; segs[nseg].y1 = y1;
               segs[nseg].x2 = x2; segs[nseg].y2 = y2;
               nseg++;
             }
           }
       }
    }

  /* Draw segments and place labels.
   * Walk segments, accumulating distance. Every intval cells, place a label.
   * Minimum gap between labels is minint cells. */
  double dist_since_label = 0.0;
  double total_dist = 0.0;

  /* First compute total contour length for initial offset */
  for (i = 0; i < nseg; i++) {
    double dx = segs[i].x2 - segs[i].x1;
    double dy = segs[i].y2 - segs[i].y1;
    total_dist += sqrt(dx*dx + dy*dy);
  }

  /* Start labeling after half the interval so labels are nicely distributed */
  double next_label = intval * 0.5;
  double accum = 0.0;

  for (i = 0; i < nseg; i++) {
    double sx1 = segs[i].x1, sy1 = segs[i].y1;
    double sx2 = segs[i].x2, sy2 = segs[i].y2;
    double dx = sx2 - sx1, dy = sy2 - sy1;
    double seglen = sqrt(dx*dx + dy*dy);

    int drew_label = 0;

    /* Check if label should be placed along this segment */
    if (label && label[0] && seglen > 0.0 && accum + seglen >= next_label) {
      double t = (next_label - accum) / seglen;
      double lx = sx1 + t*dx;
      double ly = sy1 + t*dy;
      double angle_rad = atan2(dy, dx);
      double angle_deg = angle_rad * 180.0 / M_PI;

      /* Transform label position */
      double tlx = lx, tly = ly;
      cairo_matrix_transform_point(&mat, &tlx, &tly);

      /* Get character height for gap sizing */
      double xch, ych;
      giza_get_character_size(GIZA_UNITS_WORLD, &xch, &ych);

      /* Compute label width in grid units (approximate) */
      int llen = strlen(label);
      double label_half_w = 0.5 * llen * xch * 0.6; /* approximate */

      /* Draw the segment in two parts with a gap for the label */
      /* First part: from segment start to gap start */
      double gap_t1 = t - label_half_w / (seglen > 0 ? seglen : 1.0);
      double gap_t2 = t + label_half_w / (seglen > 0 ? seglen : 1.0);
      if (gap_t1 < 0.0) gap_t1 = 0.0;
      if (gap_t2 > 1.0) gap_t2 = 1.0;

      if (gap_t1 > 0.01) {
        double tx1 = sx1, ty1 = sy1;
        double tx2 = sx1 + gap_t1*dx, ty2 = sy1 + gap_t1*dy;
        cairo_matrix_transform_point(&mat, &tx1, &ty1);
        cairo_matrix_transform_point(&mat, &tx2, &ty2);
        cairo_move_to(Dev[id].context, tx1, ty1);
        cairo_line_to(Dev[id].context, tx2, ty2);
        cairo_stroke(Dev[id].context);
      }
      if (gap_t2 < 0.99) {
        double tx1 = sx1 + gap_t2*dx, ty1 = sy1 + gap_t2*dy;
        double tx2 = sx2, ty2 = sy2;
        cairo_matrix_transform_point(&mat, &tx1, &ty1);
        cairo_matrix_transform_point(&mat, &tx2, &ty2);
        cairo_move_to(Dev[id].context, tx1, ty1);
        cairo_line_to(Dev[id].context, tx2, ty2);
        cairo_stroke(Dev[id].context);
      }

      /* Draw the label.
       * The affine maps grid -> world: world_x = affine[4] + affine[0]*lx + affine[2]*ly
       * lx, ly are grid coords; giza_ptext expects world coords. */
      _giza_set_trans(GIZA_TRANS_WORLD);
      double wx = affine[4] + affine[0]*lx + affine[2]*ly;
      double wy = affine[5] + affine[1]*lx + affine[3]*ly;
      giza_ptext(wx, wy, angle_deg, 0.5, label);
      _giza_set_trans(GIZA_TRANS_IDEN);

      next_label += (double)intval;
      if (next_label - (accum + seglen) < (double)minint)
        next_label = accum + seglen + (double)minint;
      drew_label = 1;
    }

    if (!drew_label) {
      /* Draw the full segment */
      double tx1 = sx1, ty1 = sy1;
      double tx2 = sx2, ty2 = sy2;
      cairo_matrix_transform_point(&mat, &tx1, &ty1);
      cairo_matrix_transform_point(&mat, &tx2, &ty2);
      cairo_move_to(Dev[id].context, tx1, ty1);
      cairo_line_to(Dev[id].context, tx2, ty2);
      cairo_stroke(Dev[id].context);
    }

    accum += seglen;
  }

  free(segs);

  _giza_set_trans (oldTrans);
  if (!oldBuf) giza_end_buffer ();
  giza_flush_device ();

#undef xsect_l
#undef ysect_l
}

void
giza_contour_labelled_float (int sizex, int sizey, const float* data, int i1,
             int i2, int j1, int j2, float c,
             const float *affine, const char *label,
             int intval, int minint)
{
  double *ddata = malloc(sizeof(double) * sizex * sizey);
  double daffine[6];
  int i, j;

  if (!ddata) {
    _giza_warning("giza_contour_labelled_float", "memory allocation failed");
    return;
  }

  for (j=j1; j<=j2; j++) {
      for (i=i1; i<=i2; i++) {
          ddata[j*sizex+i] = (double) data[j*sizex+i];
      }
  }

  for (i=0; i<6; i++) {
     daffine[i] = (double) affine[i];
  }

  giza_contour_labelled (sizex, sizey, ddata, i1, i2, j1, j2,
                         (double) c, daffine, label, intval, minint);
  free(ddata);
}
