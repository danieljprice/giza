/* giza - a scientific plotting layer built on cairo
 *
 * This file is (or was) part of GIZA, a scientific plotting
 * layer built on cairo.
 * GIZA comes with ABSOLUTELY NO WARRANTY.
 * This is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * Copyright (C) 2010-2011 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include "giza-io-private.h"
#include "giza-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <stdlib.h>
#include <math.h>

static void _giza_colour_pixel (unsigned char *array, int pixNum, double pos);

/**
 * Drawing: giza_render
 *
 * Synopsis: Renders data to the device.
 *
 * Input:
 *  -sizex         :- The dimensions of data in the x-direction
 *  -sizey         :- The dimensions of data in the y-direction
 *  -data          :- The data to be render
 *  -i1            :- The inclusive range of data to render in the x dimension.
 *  -i2            :- The inclusive range of data to render in the x dimension.
 *  -j1            :- The inclusive range of data to render in the y direction
 *  -j2            :- The inclusive range of data to render in the y direction
 *  -valMin        :- The value in data that gets assign the colour corresponding to zero
 *                    on the colour ramp (The ramp is set by giza_set_colour_ramp
 *  -valMax        :- The value in data that gets assigned the colour corresponding to one
 *                    on the colour ramp
 *  -affine        :- The affine transformation matrix that will be applied to the data.
 */
void
giza_render (int sizex, int sizey, const double data[sizey][sizex], int i1, int i2,
	    int j1, int j2, double valMin, double valMax, double *affine)
{
  if (!_giza_check_device_ready ("giza_render"))
    return;

  if (sizex < 1 || sizey < 1)
    {
      _giza_warning ("giza_render", "Invalid array size, skipping render.");
    }
  if (i1 < 0 || i2 < i1 || j1 < 0 || j2 < j1)
    {
      _giza_warning ("giza_render", "Invalid index range, skipping render.");
      return;
    }

  unsigned char *pixdata;
  cairo_format_t format = CAIRO_FORMAT_ARGB32;
  cairo_surface_t *pixmap;
  cairo_matrix_t mat;
  int stride, pixnum, width = i2 - i1 + 1, height = j2 - j1 + 1;
  double pos;

  /* apply the transformation */
  int oldCi;
  giza_get_colour_index (&oldCi);
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
		     affine[4], affine[5]);
  cairo_transform (context, &mat);

  /* allocate data for the pixmap */
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  stride = cairo_format_stride_for_width (format, width);
#else
  stride = 4*width;
#endif
  pixdata = malloc (stride * height);

  /* colour each pixel in the pixmap */
  int i, j;
  pixnum = 0;
  for (j = j1; j <= j2; j++)
    {
      for (i = i1; i <= i2; i++)
	{
	  pos = (data[j][i] - valMin) / (valMax - valMin);
	  _giza_colour_pixel (pixdata, pixnum, pos);
	  pixnum = pixnum + 1;
	}
    }

  /* create the cairo surface from the pixmap */
  pixmap = cairo_image_surface_create_for_data (pixdata, format,
						width, height, stride);

  /* paint the pixmap to the primary surface */
  cairo_set_source_surface (context, pixmap, 0, 0);
  cairo_pattern_set_extend (cairo_get_source (context), CAIRO_EXTEND_REPEAT);
  cairo_paint (context);

  /* clean up and restore settings */
  _giza_set_trans (oldTrans);
  giza_set_colour_index (oldCi);
  cairo_surface_destroy (pixmap);
  free (pixdata);

  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}

/**
 * Drawing: giza_render
 *
 * Synopsis: Same functionality as giza_render but takes floats.
 *
 * See Also: giza_render
 */
void
giza_render_float (int sizex, int sizey, const float data[sizey][sizex], int i1,
		  int i2, int j1, int j2, float valMin, float valMax,
		  float *affine)
{
  if (!_giza_check_device_ready ("giza_render_float"))
    return;

  if (sizex < 1 || sizey < 1)
    {
      _giza_warning ("giza_render_float",
		    "Invalid array size, skipping render.");
    }
  if (i1 < 0 || i2 < i1 || j1 < 0 || j2 < j1)
    {
      _giza_warning ("giza_render_float",
		    "Invalid index range, skipping render.");
      return;
    }

  unsigned char *pixdata;
  cairo_format_t format = CAIRO_FORMAT_ARGB32;
  cairo_surface_t *pixmap;
  cairo_matrix_t mat;
  int stride, pixnum, width = i2 - i1 + 1, height = j2 - j1 + 1;
  double pos;

  int oldCi;
  giza_get_colour_index (&oldCi);
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_matrix_init (&mat, (double) affine[0], (double) affine[1],
		     (double) affine[2], (double) affine[3],
		     (double) affine[4], (double) affine[5]);
  cairo_transform (context, &mat);

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  stride = cairo_format_stride_for_width (format, width);
#else
  stride = 4*width;
#endif
  pixdata = malloc (stride * height);

  int i, j;
  pixnum = 0;
  for (j = j1; j <= j2; j++)
    {
      for (i = i1; i <= i2; i++)
	{
          pos = (double) (data[j][i] - valMin) / (valMax - valMin);
	  _giza_colour_pixel (pixdata, pixnum, pos);
	  pixnum = pixnum + 1;
	}
    }

  pixmap = cairo_image_surface_create_for_data (pixdata, format,
						width, height, stride);

  cairo_set_source_surface (context, pixmap, 0, 0);
  cairo_pattern_set_extend (cairo_get_source (context), CAIRO_EXTEND_REPEAT);
  cairo_paint (context);

  _giza_set_trans (oldTrans);
  giza_set_colour_index (oldCi);
  cairo_surface_destroy (pixmap);
  free (pixdata);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}

/**
 * Drawing: giza_render_grey
 *
 * Synopsis: Same functionality as giza_render but renders in greyscale
 *
 * See Also: giza_render
 */
void
giza_render_gray (int sizex, int sizey, const double data[sizey][sizex], int i1,
		  int i2, int j1, int j2, double valMin, double valMax,
		  double *affine)
{
  giza_save_colour_table();
  giza_set_colour_table_gray ();  
  giza_render (sizex, sizey, data, i1, i2, j1, j2, valMin, valMax, affine);
  giza_restore_colour_table();
}

/**
 * Drawing: giza_render_grey_float
 *
 * Synopsis: Same functionality as giza_render_grey but renders in greyscale
 *
 * See Also: giza_render_grey, giza_render
 */
void
giza_render_gray_float (int sizex, int sizey, const float data[sizey][sizex], int i1,
		  int i2, int j1, int j2, float valMin, float valMax,
		  float *affine)
{
  giza_save_colour_table();
  giza_set_colour_table_gray();
  giza_render_float (sizex, sizey, data, i1, i2, j1, j2, valMin, valMax, affine);
  giza_restore_colour_table();
}

/**
 * Sets the rgb for a given pixel, given position in the colour table.
 * NOTE: This function needs to be changed to operate on different endians.
 *
 * Input:
 *  -array  :- the array in which to store the colour.
 *  -pixnum :- the pixel to be coloured.
 *  -pos    :- the fraction along the colour table to get the colour from
 */
static void
_giza_colour_pixel (unsigned char *array, int pixNum, double pos)
{
  double r, g, b;
  giza_rgb_from_table (pos, &r, &g, &b);
  /* set the alpha */
  array[pixNum * 4 + 3] = 255;
  /* set the red, green, and blue */
  array[pixNum * 4 + 2] = (unsigned char) (r * 255.);
  array[pixNum * 4 + 1] = (unsigned char) (g * 255.);
  array[pixNum * 4 + 0] = (unsigned char) (b * 255.);
}

/**
 * Sets the rgb for a given pixel, given the colour index
 *
 * Input:
 *  -array  :- the array in which to store the colour.
 *  -pixnum :- the pixel to be coloured.
 *  -ci     :- the colour index with which to colour the pixel.
 */
static void
_giza_colour_pixel_index (unsigned char *array, int pixNum, int ci)
{
  double r, g, b,alpha;
  giza_get_colour_representation_alpha(ci, &r, &g, &b, &alpha);
  /* set the alpha */
  array[pixNum * 4 + 3] = (unsigned char) (alpha * 255.);
  /* set the red, green, and blue */
  array[pixNum * 4 + 2] = (unsigned char) (r * 255.);
  array[pixNum * 4 + 1] = (unsigned char) (g * 255.);
  array[pixNum * 4 + 0] = (unsigned char) (b * 255.);
}

/**
 * Drawing: giza_draw_pixels
 *
 * Synopsis: Renders an array of pixels according to a colour index defined for each pixel
 *
 * Input:
 *  -sizex         :- The dimensions of data in the x-direction
 *  -sizey         :- The dimensions of data in the y-direction
 *  -idata          :- The data to be rendered (colour index on each pixel)
 *  -i1            :- The inclusive range of data to render in the x dimension.
 *  -i2            :- The inclusive range of data to render in the x dimension.
 *  -j1            :- The inclusive range of data to render in the y direction
 *  -j2            :- The inclusive range of data to render in the y direction
 *  -xmin        :- world coordinate corresponding to left of pixel array
 *  -xmax        :- world coordinate corresponding to right of pixel array
 *  -ymin        :- world coordinate corresponding to bottom of pixel array
 *  -ymax        :- world coordinate corresponding to top of pixel array
 */
void
giza_draw_pixels (int sizex, int sizey, const int idata[sizey][sizex], int i1, int i2,
	    int j1, int j2, double xmin, double xmax, double ymin, double ymax)
{
  if (!_giza_check_device_ready ("giza_render_pixels"))
    return;

  if (sizex < 1 || sizey < 1)
    {
      _giza_warning ("giza_render_pixels", "Invalid array size, skipping render.");
    }
  if (i1 < 0 || i2 < i1 || j1 < 0 || j2 < j1)
    {
      _giza_warning ("giza_render_pixels", "Invalid index range, skipping render.");
      return;
    }

  unsigned char *pixdata;
  cairo_format_t format = CAIRO_FORMAT_ARGB32;
  cairo_surface_t *pixmap;
  cairo_matrix_t mat;
  int stride, pixnum, width = i2 - i1 + 1, height = j2 - j1 + 1;

  /* apply the transformation */
  int oldCi;
  giza_get_colour_index (&oldCi);
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  
  double dxpix = (xmax - xmin)/((double) width);
  double dypix = (ymax - ymin)/((double) height);
  cairo_matrix_init (&mat, dxpix, 0.,0., dypix,
		     xmin, ymin);
  cairo_transform (context, &mat);

  /* allocate data for the pixmap */
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  stride = cairo_format_stride_for_width (format, width);
#else
  stride = 4*width;
#endif
  pixdata = malloc (stride * height);

  /* colour each pixel in the pixmap */
  int i, j;
  pixnum = 0;
  for (j = j1; j <= j2; j++)
    {
      for (i = i1; i <= i2; i++)
	{
	  int ci = idata[j][i];
	  _giza_colour_pixel_index (pixdata, pixnum, ci);
	  pixnum = pixnum + 1;
	}
    }

  /* create the cairo surface from the pixmap */
  pixmap = cairo_image_surface_create_for_data (pixdata, format,
						width, height, stride);

  /* paint the pixmap to the primary surface */
  cairo_set_source_surface (context, pixmap, 0, 0);
  cairo_pattern_set_extend (cairo_get_source (context), CAIRO_EXTEND_PAD);
  cairo_paint (context);

  /* clean up and restore settings */
  _giza_set_trans (oldTrans);
  giza_set_colour_index (oldCi);
  cairo_surface_destroy (pixmap);
  free (pixdata);

  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}

/**
 * Drawing: giza_draw_pixels_float
 *
 * Synopsis: Same as giza_draw_pixels, but takes floats
 *
 * See Also: giza_draw_pixels
 */

void
giza_draw_pixels_float (int sizex, int sizey, const int idata[sizey][sizex], int i1, int i2,
	    int j1, int j2, float xmin, float xmax, float ymin, float ymax)
{
  if (!_giza_check_device_ready ("giza_render_pixels_float"))
    return;
  
  giza_draw_pixels (sizex, sizey, idata, i1, i2,
	    j1, j2, (double) xmin, (double) xmax, (double) ymin, (double) ymax);

}
