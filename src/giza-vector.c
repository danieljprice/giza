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

#include "giza-io-private.h"
#include "giza-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <math.h>

/**
 * Drawing: giza_vector
 *
 * Synopsis: Plot of vector data.
 *
 * Input:
 *  -n             :- The dimensions of data in the x-direction
 *  -m             :- The dimensions of data in the y-direction
 *  -horizontal    :- The x-component of the data to be plotted
 *  -vertical      :- The y-component of the data to be plotted
 *  -i1            :- The inclusive range of data to render in the x dimension.
 *  -i2            :- The inclusive range of data to render in the x dimension.
 *  -j1            :- The inclusive range of data to render in the y direction
 *  -j2            :- The inclusive range of data to render in the y direction
 *  -scale         :- scaling factor for arrow lengths (0 = automatic)
 *  -position      :- justification of vector arrow with respect to pixel (0=left, 0.5=centred)
 *  -affine        :- The affine transformation matrix that will be applied to the data.
 *
 * See Also: giza_vector_float, giza_arrow, giza_set_arrow_style
 */
void
giza_vector (int n, int m, const double* horizontal, const double* vertical,
	     int i1, int i2, int j1, int j2, double scale, int position,
	     const double* affine, double blank)
{
  if (!_giza_check_device_ready ("giza_vector"))
    return;

  /* Check ranges */
  if (i1 < 0 || i2 >= n || i1 > i2)
    {
      _giza_error ("giza_vector",
		   "invalid index range for horizontal values");
      return;
    }
  if (j1 < 0 || j2 >= m || j1 > j2)
    {
      _giza_error ("giza_vector", "invalid index range for vertical values");
      return;
    }

  int i, j;
  double x1, x2, y1, y2;
  cairo_matrix_t mat;

  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                     affine[4], affine[5]);

  double dscale = scale;
  /* Find the scaling factor */
  if (fabs (dscale) < GIZA_ZERO_DOUBLE)
    {
      for (j = j1; j <= j2; j++)
	{
	  for (i = i1; i <= i2; i++)
	    {
	      if (!(_giza_equal(horizontal[j*n+i],blank) && _giza_equal(vertical[j*n+i],blank)))
		{
		  double tmp =
		    sqrt (horizontal[j*n+i] * horizontal[j*n+i] +
			  vertical[j*n+i] * vertical[j*n+i]);
		  if (tmp > dscale)
		    dscale = tmp;
		}
	    }
	}
      if (fabs (dscale) < GIZA_ZERO_DOUBLE) { return; }
      double dx2 = affine[0]*affine[0] + affine[1]*affine[1];
      double dy2 = affine[2]*affine[2] + affine[3]*affine[3];
      if (dx2 < dy2)
        {
          dscale = sqrt(dx2)/dscale;
        } else {
          dscale = sqrt(dy2)/dscale;
        }
    }

  int oldBuf;
  giza_get_buffering(&oldBuf);
  giza_begin_buffer ();

 /* Draw the arrows! */
  double x, y;
  for (j = j1; j <= j2; j++)
    {
      for (i = i1; i <= i2; i++)
	{
	  if (!(_giza_equal(horizontal[j*n+i],blank) && _giza_equal(vertical[j*n+i],blank)))
          {
             x = (double) i + 0.5;
             y = (double) j + 0.5;
 	     cairo_matrix_transform_point (&mat, &x, &y);

	     if (position < 0)
	       {
	         x2 = x;
	         y2 = y;
	         x1 = x2 - horizontal[j*n+i] * dscale;
	         y1 = y2 - vertical[j*n+i] * dscale;
	       }
	     else if (_giza_equal(position,0.))
	       {
	         x2 = x + 0.5 * horizontal[j*n+i] * dscale;
	         y2 = y + 0.5 * vertical[j*n+i] * dscale;
	         x1 = x2 - horizontal[j*n+i] * dscale;
	         y1 = y2 - vertical[j*n+i] * dscale;
	       }
	     else
	       {
	         x1 = x;
	         y1 = y;
	         x2 = x1 + horizontal[j*n+i] * dscale;
	         y2 = y1 + vertical[j*n+i] * dscale;
	       }
	     giza_arrow (x1, y1, x2, y2);
	 }
       }
    }

  if (!oldBuf)
    giza_end_buffer ();

  giza_flush_device ();
}

/**
 * Drawing: giza_vector_float
 *
 * Synopsis: Same as giza_vector but takes floats
 *
 * See Also: giza_vector, giza_arrow, giza_set_arrow_style
 *
 */
void
giza_vector_float (int n, int m, const float* horizontal, const float* vertical,
	     int i1, int i2, int j1, int j2, float scale, int position,
	     const float* affine, float blank)
{
  if (!_giza_check_device_ready ("giza_vector"))
    return;

  /* Check ranges */
  if (i1 < 0 || i2 >= n || i1 > i2)
    {
      _giza_error ("giza_vector",
		   "invalid index range for horizontal values");
      return;
    }
  if (j1 < 0 || j2 >= m || j1 > j2)
    {
      _giza_error ("giza_vector", "invalid index range for vertical values");
      return;
    }

  int i, j;
  double x1, x2, y1, y2;
  cairo_matrix_t mat;

  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                     affine[4], affine[5]);

 /* Find the scaling factor */
  double dscale = (double) scale;
  if (fabs (scale) < GIZA_ZERO_FLOAT)
    {
      for (j = j1; j <= j2; j++)
	{
	  for (i = i1; i <= i2; i++)
	    {
	      if (!(_giza_equal(horizontal[j*n+i],blank) && _giza_equal(vertical[j*n+i],blank)))
		{
		  double tmp =
		    sqrt (horizontal[j*n+i] * horizontal[j*n+i] +
			  vertical[j*n+i] * vertical[j*n+i]);
		  if (tmp > dscale)
		    dscale = tmp;
		}
	    }
	}
      if (fabs (dscale) < GIZA_ZERO_DOUBLE) { return; }
      float dx2 = affine[0]*affine[0] + affine[1]*affine[1];
      float dy2 = affine[2]*affine[2] + affine[3]*affine[3];
      if (dx2 < dy2)
        {
          dscale = sqrt(dx2)/dscale;
        } else {
          dscale = sqrt(dy2)/dscale;
        }
    }

  int oldBuf;
  giza_get_buffering (&oldBuf);
  giza_begin_buffer ();

    /* Draw the arrows! */
  double x, y;
  for (j = j1; j <= j2; j++)
    {
      for (i = i1; i <= i2; i++)
	{
	  if (!(_giza_equal(horizontal[j*n+i],blank) && _giza_equal(vertical[j*n+i],blank)))
          {
             x = (double) i + 0.5;
             y = (double) j + 0.5;
 	     cairo_matrix_transform_point (&mat, &x, &y);

	     if (position < 0)
	       {
	         x2 = x;
	         y2 = y;
	         x1 = x2 - horizontal[j*n+i] * dscale;
	         y1 = y2 - vertical[j*n+i] * dscale;
	       }
	     else if (_giza_equal(position,0.))
	       {
	         x2 = x + 0.5 * horizontal[j*n+i] * dscale;
	         y2 = y + 0.5 * vertical[j*n+i] * dscale;
	         x1 = x2 - horizontal[j*n+i] * dscale;
	         y1 = y2 - vertical[j*n+i] * dscale;
	       }
	     else
	       {
	         x1 = x;
	         y1 = y;
	         x2 = x1 + horizontal[j*n+i] * dscale;
	         y2 = y1 + vertical[j*n+i] * dscale;
	       }
  	       giza_arrow (x1, y1, x2, y2);
          }
	}
    }

  if (!oldBuf)
    giza_end_buffer ();

  giza_flush_device ();
}
