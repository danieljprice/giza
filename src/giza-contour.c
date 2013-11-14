/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
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

/*
 * The guts of this routine were taken from conrec.c
 * as written by Paul Bourke.
 */

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-transforms-private.h"
#include <giza.h>


void
giza_contour (int sizex, int sizey, const double* data, int i1,
	      int i2, int j1, int j2, int ncont, const double* cont,
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
  giza_get_line_style (&ls);
  giza_set_line_style (ls);


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
		    case 1:	/* Line between (i, j) (i + 1, j) */
		      x1 = xh[m1];
		      y1 = yh[m1];
		      x2 = xh[m2];
		      y2 = yh[m2];
		      break;
		    case 2:	/* Line between (i + 1, j) (i + 1, j + 1) */
		      x1 = xh[m2];
		      y1 = yh[m2];
		      x2 = xh[m3];
		      y2 = yh[m3];
		      break;
		    case 3:	/* Line between (i, j) (i + 1, j + 1) */
		      x1 = xh[m3];
		      y1 = yh[m3];
		      x2 = xh[m1];
		      y2 = yh[m1];
		      break;
		    case 4:	/* Line between (i, j) and between (i + 1, j), (i + 1, j + 1) */
		      x1 = xh[m1];
		      y1 = yh[m1];
		      x2 = xsect (m2, m3);
		      y2 = ysect (m2, m3);
		      break;
		    case 5:	/* Line between (i + 1, j) and between (i, j), (i + 1, j + 1) */
		      x1 = xh[m2];
		      y1 = yh[m2];
		      x2 = xsect (m3, m1);
		      y2 = ysect (m3, m1);
		      break;
		    case 6:	/* Line between (i + 1, j + 1) and between (i, j), (i + 1, j + 1)  */
		      x1 = xh[m3];
		      y1 = yh[m3];
		      x2 = xsect (m3, m2);
		      y2 = ysect (m3, m2);
		      break;
		    case 7:	/* Line between sides 1-2 and 2-3  */
		      x1 = xsect (m1, m2);
		      y1 = ysect (m1, m2);
		      x2 = xsect (m2, m3);
		      y2 = ysect (m2, m3);
		      break;
		    case 8:	/* Line between sides 2-3 and 3-1 */
		      x1 = xsect (m2, m3);
		      y1 = ysect (m2, m3);
		      x2 = xsect (m3, m1);
		      y2 = ysect (m3, m1);
		      break;
		    case 9:	/* Line between sides 3-1 and 1-2 */
		      x1 = xsect (m3, m1);
		      y1 = ysect (m3, m1);
		      x2 = xsect (m1, m2);
		      y2 = ysect (m1, m2);
		      break;
		    default:
		      break;
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
  double dcont[ncont];
  double daffine[6];
  int i, j;

  for (j=j1; j<=j2; j++) {
      for (i=i1; i<=i2; i++) {
          ddata[j*sizex+i] = (double) data[j*sizex+i];
      }
  }

  for (i=0; i<ncont; i++) {
      dcont[i] = (double) cont[i];
  }

  for (i=0; i<6; i++) {
     daffine[i] = (double) affine[i];
  }

  giza_contour (sizex, sizey, ddata, i1, i2, j1, j2, ncont, dcont, daffine);

}
