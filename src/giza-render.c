/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 5a) of the GPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
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
#include "giza-itf.h"
#include "giza-render-private.h"
#include <giza.h>
#include <stdlib.h>
#include <math.h>

/**
 * Drawing: giza_render
 *
 * Synopsis: Renders data to the device.
 *
 * Input:
 *  -sizex         :- The dimensions of data in the x-direction
 *  -sizey         :- The dimensions of data in the y-direction
 *  -data          :- The data to be rendered
 *  -i1            :- The inclusive range of data to render in the x dimension.
 *  -i2            :- The inclusive range of data to render in the x dimension.
 *  -j1            :- The inclusive range of data to render in the y direction
 *  -j2            :- The inclusive range of data to render in the y direction
 *  -valMin        :- The value in data that gets assign the colour corresponding to zero
 *                    on the colour ramp (The ramp is set by giza_set_colour_table)
 *  -valMax        :- The value in data that gets assigned the colour corresponding to one
 *                    on the colour ramp
 *  -extend        :- Option for how to deal with image at edges
 *  -filter        :- Option for how to interpolate between pixels (since v1.5)
 *  -affine        :- The affine transformation matrix that will be applied to the data.
 *
 * Allowed extend settings:
 *  -0 or GIZA_EXTEND_NONE    :- no padding
 *  -1 or GIZA_EXTEND_REPEAT  :- periodic tiling
 *  -2 or GIZA_EXTEND_REFLECT :- reflective boundary
 *  -3 or GIZA_EXTEND_PAD     :- pad by extending last few pixels
 *
 * Allowed filter settings (from cairo_filter_t):
 *  -0 or GIZA_FILTER_DEFAULT  :- default, usually bilinear interpolation
 *  -1 or GIZA_FILTER_FAST     :- a high performance filter with quality similar to GIZA_FILTER_NEAREST
 *  -2 or GIZA_FILTER_GOOD     :- a reasonable-performance filter, with quality similar to GIZA_FILTER_BILINEAR
 *  -3 or GIZA_FILTER_BEST     :- highest quality available, performance may not be suited for interactive use
 *  -4 or GIZA_FILTER_NEAREST  :- Nearest-neighbour filtering
 *  -5 or GIZA_FILTER_BILINEAR :- Linear interpolation in two dimensions
 *
 * See Also: giza_set_colour_table
 */
void
giza_render (int sizex, int sizey, const double* data, int i1, int i2,
             int j1, int j2, double valMin, double valMax, int extend, int filter, const double *affine)
{
   _giza_render (sizex, sizey, data,i1,i2,j1,j2,valMin,valMax,affine,0,extend,filter,data);
}

/**
 * Drawing: giza_render_transparent
 *
 * Synopsis: Same as giza_render, but data < valMin rendered as transparent
 *
 */
void
giza_render_transparent (int sizex, int sizey, const double* data, int i1, int i2,
           int j1, int j2, double valMin, double valMax, int extend, int filter, const double *affine)
{
   _giza_render (sizex, sizey, data,i1,i2,j1,j2,valMin,valMax,affine,1,extend,filter,data);
}

/**
 * Drawing: giza_render_alpha
 *
 * Synopsis: Same as giza_render, but uses additional array specifying transparency of each pixel
 *
 */
void
giza_render_alpha (int sizex, int sizey, const double* data, const double* alpha, int i1, int i2,
                   int j1, int j2, double valMin, double valMax, int extend, int filter, const double *affine)
{
   _giza_render (sizex, sizey, data,i1,i2,j1,j2,valMin,valMax,affine,2,extend,filter,alpha);
}

/**
 *  _giza_render is the internal routine with the most general (and changeable) interface
 *  to which giza_render, giza_render_transparent, etc. are the static API interfaces
 */
void
_giza_render (int sizex, int sizey, const double* data, int i1, int i2,
                   int j1, int j2, double valMin, double valMax, const double *affine,
              int transparent, int extend, int filter, const double* datalpha)
{
  if (!_giza_check_device_ready ("giza_render"))
    return;

  if (sizex < 1 || sizey < 1)
    {
      _giza_warning ("giza_render", "Invalid array size, skipping render.");
      return;
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
  int cimin, cimax;

  cairo_extend_t cairoextendtype;
  _giza_get_extend(extend,&cairoextendtype);

  cairo_filter_t cairofiltertype;
  _giza_get_filter(filter,&cairofiltertype);

  /* apply the transformation */
  int oldCi;
  giza_get_colour_index (&oldCi);
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                   affine[4], affine[5]);
  cairo_transform (Dev[id].context, &mat);

  /* allocate data for the pixmap */
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  stride = cairo_format_stride_for_width (format, width);
#else
  stride = 4*width;
#endif
  pixdata = malloc (stride * height);

  /* colour each pixel in the pixmap */
  int i, j;
  giza_itf_idx_type itf_idx = giza_itf_idx[ Dev[id].itf ];

  giza_get_colour_index_range(&cimin, &cimax);
  pixnum = 0;
  /* transparent if-statement is outside loop as optimisation */
  if (transparent==2) {
    /* render each pixel, using transparent routine */
    for (j = j1; j <= j2; j++)
      {
        for (i = i1; i <= i2; i++)
         {
           _giza_colour_pixel_index_alpha (pixdata, pixnum, itf_idx(data[j*sizex + i], valMin, valMax, cimin, cimax), datalpha[j*sizex + i]);
           pixnum = pixnum + 1;
         }
      }
  } else if (transparent==1) {
    /* render each pixel, using transparent routine */
    int idx;
    for (j = j1; j <= j2; j++)
      {
        for (i = i1; i <= i2; i++)
         {
      idx = itf_idx(data[j*sizex + i], valMin, valMax, cimin, cimax);
           _giza_colour_pixel_index_alpha (pixdata, pixnum, idx, idx==cimin ? 0. : 1.);
           pixnum = pixnum + 1;
         }
      }
  } else {
    /* render each pixel, usual routine */
    for (j = j1; j <= j2; j++)
      {
        for (i = i1; i <= i2; i++)
         {
           _giza_colour_pixel_index_alpha (pixdata, pixnum, itf_idx(data[j*sizex + i], valMin, valMax, cimin,cimax), 1.);
           pixnum = pixnum + 1;
         }
      }
  }

  /* create the cairo surface from the pixmap */
  pixmap = cairo_image_surface_create_for_data (pixdata, format,
                                          width, height, stride);

  /* paint the pixmap to the primary surface */
  cairo_set_source_surface (Dev[id].context, pixmap, 0, 0);
  cairo_pattern_set_extend (cairo_get_source (Dev[id].context), cairoextendtype);

  /* Set the interpolation filter */
  cairo_pattern_set_filter (cairo_get_source(Dev[id].context), cairofiltertype);

  cairo_paint (Dev[id].context);

  /* clean up and restore settings */
  _giza_set_trans (oldTrans);
  giza_set_colour_index (oldCi);
  cairo_surface_destroy (pixmap);
  free (pixdata);

  giza_flush_device ();
}

/**
 * Drawing: giza_render_float
 *
 * Synopsis: Same functionality as giza_render but takes floats.
 *
 * See Also: giza_render
 */
void
giza_render_float (int sizex, int sizey, const float* data, int i1,
                int i2, int j1, int j2, float valMin, float valMax,
                int extend, int filter, const float *affine)
{
  _giza_render_float (sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,affine,0,extend,filter,data);
}

/**
 * Drawing: giza_render_transparent_float
 *
 * Synopsis: Same functionality as giza_render_transparent but takes floats.
 *
 * See Also: giza_render_transparent
 */
void
giza_render_transparent_float (int sizex, int sizey, const float* data, int i1,
                int i2, int j1, int j2, float valMin, float valMax,
                int extend, int filter, const float *affine)
{
  _giza_render_float (sizex,sizey,data,i1,i2,j1,j2,valMin,valMax,affine,1,extend,filter,data);
}

/**
 * Drawing: giza_render_alpha_float
 *
 * Synopsis: Same as giza_render_alpha but takes floats
 *
 * See Also: giza_render_alpha
 */
void
giza_render_alpha_float (int sizex, int sizey, const float* data, const float* alpha, int i1, int i2,
           int j1, int j2, float valMin, float valMax, int extend, int filter, const float *affine)
{
   _giza_render_float (sizex, sizey, data,i1,i2,j1,j2,valMin,valMax,affine,2,extend,filter,alpha);
}
/**
 *  _giza_render_float is the internal routine with the most general (and changeable) interface
 *  c.f. _giza_render
 */
void
_giza_render_float (int sizex, int sizey, const float* data, int i1,
                  int i2, int j1, int j2, float valMin, float valMax,
                  const float *affine, int transparent, int extend, int filter, const float* datalpha)
{
  if (!_giza_check_device_ready ("giza_render_float"))
    return;

  if (sizex < 1 || sizey < 1)
    {
      _giza_warning ("giza_render_float",
                  "Invalid array size, skipping render.");
      return;
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
  int cimin, cimax;

  cairo_extend_t cairoextendtype;
  _giza_get_extend(extend,&cairoextendtype);

  cairo_filter_t cairofiltertype;
  _giza_get_filter(filter,&cairofiltertype);

  int oldCi;
  giza_get_colour_index (&oldCi);
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_matrix_init (&mat, (double) affine[0], (double) affine[1],
                   (double) affine[2], (double) affine[3],
                   (double) affine[4], (double) affine[5]);
  cairo_transform (Dev[id].context, &mat);

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 6, 0)
  stride = cairo_format_stride_for_width (format, width);
#else
  stride = 4*width;
#endif
  pixdata = malloc (stride * height);

  int i, j;
  giza_itf_idx_type_f itf_idx = giza_itf_idx_f[ Dev[id].itf ];

  giza_get_colour_index_range(&cimin, &cimax);
  pixnum = 0;
  /* transparent if-statement is outside loop as optimisation */
  if (transparent==2) {
    /* render each pixel, using transparent routine */
    for (j = j1; j <= j2; j++)
      {
        for (i = i1; i <= i2; i++)
         {
           _giza_colour_pixel_index_alpha (pixdata, pixnum, itf_idx(data[j*sizex + i], valMin, valMax, cimin, cimax), datalpha[j*sizex + i]);
           pixnum = pixnum + 1;
         }
      }
  } else if (transparent==1) {
    /* render each pixel, using transparent routine */
      int idx;
    for (j = j1; j <= j2; j++)
      {
        for (i = i1; i <= i2; i++)
         {
              idx = itf_idx(data[j*sizex + i], valMin, valMax, cimin, cimax);
              _giza_colour_pixel_index_alpha (pixdata, pixnum, idx, idx==cimin ? 0. : 1.);
              pixnum = pixnum + 1;
         }
      }
  } else {
    /* render each pixel, usual routine */
    for (j = j1; j <= j2; j++)
      {
        for (i = i1; i <= i2; i++)
         {
              _giza_colour_pixel_index_alpha (pixdata, pixnum, itf_idx(data[j*sizex + i], valMin, valMax, cimin, cimax), 1);
              pixnum = pixnum + 1;
         }
      }
  }

  pixmap = cairo_image_surface_create_for_data (pixdata, format,
                                          width, height, stride);

  cairo_set_source_surface (Dev[id].context, pixmap, 0, 0);
  cairo_pattern_set_extend (cairo_get_source (Dev[id].context), cairoextendtype);
  cairo_pattern_set_filter (cairo_get_source(Dev[id].context), cairofiltertype);

  cairo_paint (Dev[id].context);

  _giza_set_trans (oldTrans);
  giza_set_colour_index (oldCi);
  cairo_surface_destroy (pixmap);
  free (pixdata);
  giza_flush_device ();
}

/**
 * Drawing: giza_render_gray
 *
 * Synopsis: Same functionality as giza_render but renders in grayscale
 *
 * See Also: giza_render
 */
void
giza_render_gray (int sizex, int sizey, const double* data, int i1,
                int i2, int j1, int j2, double valMin, double valMax,
                int extend, int filter, const double *affine)
{
  giza_save_colour_table();
  giza_set_colour_table_gray ();
  giza_render (sizex, sizey, data, i1, i2, j1, j2, valMin, valMax, extend, filter, affine);
  giza_restore_colour_table();
}

/**
 * Drawing: giza_render_gray_float
 *
 * Synopsis: Same functionality as giza_render_gray but renders in grayscale
 *
 * See Also: giza_render_gray, giza_render
 */
void
giza_render_gray_float (int sizex, int sizey, const float* data, int i1,
                int i2, int j1, int j2, float valMin, float valMax,
                int extend, int filter, const float *affine)
{
  giza_save_colour_table();
  giza_set_colour_table_gray();
  giza_render_float (sizex, sizey, data, i1, i2, j1, j2, valMin, valMax, extend, filter, affine);
  giza_restore_colour_table();
}

/**
 * Sets the rgb for a given pixel, given position in the colour table.
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
  array[pixNum * 4 + 3] = 255.;
  /* set the red, green, and blue */
  array[pixNum * 4 + 2] = (unsigned char) (r * 255.);
  array[pixNum * 4 + 1] = (unsigned char) (g * 255.);
  array[pixNum * 4 + 0] = (unsigned char) (b * 255.);
}

/**
 * As in _giza_colour_pixel, but takes an additional alpha parameter
 */
static void
_giza_colour_pixel_alpha (unsigned char *array, int pixNum, double pos, double alpha)
{
  double r, g, b;
  giza_rgb_from_table (pos, &r, &g, &b);
  /* set the alpha */
  array[pixNum * 4 + 3] = (unsigned char) (alpha * 255.);
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
 * Sets the rgb for a given pixel, given the colour index, but allow
 * override of alpha
 *
 * Input:
 *  -array  :- the array in which to store the colour.
 *  -pixnum :- the pixel to be coloured.
 *  -ci     :- the colour index with which to colour the pixel.
 *  -alpha  :- the alpha to use, override what's in the colour table
 */
static void
_giza_colour_pixel_index_alpha (unsigned char *array, int pixNum, int ci, double alpha)
{
  double r, g, b,dummy;
  giza_get_colour_representation_alpha(ci, &r, &g, &b, &dummy);
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
 *  -extend      :- Option for how to deal with image at edges (see giza_render)
 *  -filter      :- Option for how to interpolate between pixels (see giza_render, since v1.5)
 *
 * See Also: giza_render, giza_draw_pixels_float
 */
void
giza_draw_pixels (int sizex, int sizey, const int* idata, int i1, int i2,
           int j1, int j2, double xmin, double xmax, double ymin, double ymax, int extend, int filter)
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

  cairo_extend_t cairoextendtype;
  _giza_get_extend(extend,&cairoextendtype);

  cairo_filter_t cairofiltertype;
  _giza_get_filter(filter,&cairofiltertype);

  /* apply the transformation */
  int oldCi;
  giza_get_colour_index (&oldCi);
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  double dxpix = (xmax - xmin)/((double) width);
  double dypix = (ymax - ymin)/((double) height);
  cairo_matrix_init (&mat, dxpix, 0.,0., dypix,
                   xmin, ymin);
  cairo_transform (Dev[id].context, &mat);

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
         int ci = idata[j*sizex+i];
         _giza_colour_pixel_index (pixdata, pixnum, ci);
         pixnum = pixnum + 1;
       }
    }

  /* create the cairo surface from the pixmap */
  pixmap = cairo_image_surface_create_for_data (pixdata, format,
                                          width, height, stride);

  /* paint the pixmap to the primary surface */
  cairo_set_source_surface (Dev[id].context, pixmap, 0, 0);
  cairo_pattern_set_extend (cairo_get_source (Dev[id].context), cairoextendtype);
  cairo_pattern_set_filter (cairo_get_source (Dev[id].context), cairofiltertype);
  cairo_paint (Dev[id].context);

  /* clean up and restore settings */
  _giza_set_trans (oldTrans);
  giza_set_colour_index (oldCi);
  cairo_surface_destroy (pixmap);
  free (pixdata);

  giza_flush_device ();
}

/**
 * Drawing: giza_draw_pixels_float
 *
 * Synopsis: Same as giza_draw_pixels, but takes floats
 *
 * See Also: giza_draw_pixels
 */
void
giza_draw_pixels_float (int sizex, int sizey, const int* idata, int i1, int i2,
           int j1, int j2, float xmin, float xmax, float ymin, float ymax, int extend, int filter)
{
  if (!_giza_check_device_ready ("giza_render_pixels_float"))
    return;

  giza_draw_pixels (sizex, sizey, idata, i1, i2,
           j1, j2, (double) xmin, (double) xmax, (double) ymin, (double) ymax, extend, filter);

}

/**
 * Internal routine to translate giza's integer value of extend to cairo's cairo_extend_t
 */
void
_giza_get_extend (int extend, cairo_extend_t *cairoextend)
{

  switch (extend)
    {
    case GIZA_EXTEND_PAD:
       *cairoextend = CAIRO_EXTEND_PAD;
       break;
    case GIZA_EXTEND_REFLECT:
       *cairoextend = CAIRO_EXTEND_REFLECT;
       break;
    case GIZA_EXTEND_REPEAT:
       *cairoextend = CAIRO_EXTEND_REPEAT;
       break;
    default:
       *cairoextend = CAIRO_EXTEND_NONE;
       break;
    }
  return;
}

/**
 * Internal routine to translate giza's integer value of filter to cairo's cairo_filter_t
 */
void
_giza_get_filter (int filter, cairo_filter_t *cairofilter)
{

  switch (filter)
    {
    case GIZA_FILTER_FAST:
       *cairofilter = CAIRO_FILTER_FAST;
       break;
    case GIZA_FILTER_GOOD:
       *cairofilter = CAIRO_FILTER_GOOD;
       break;
    case GIZA_FILTER_BEST:
       *cairofilter = CAIRO_FILTER_BEST;
       break;
    case GIZA_FILTER_NEAREST:
       *cairofilter = CAIRO_FILTER_NEAREST;
       break;
    case GIZA_FILTER_BILINEAR:
       *cairofilter = CAIRO_FILTER_BILINEAR;
       break;
    case GIZA_FILTER_GAUSSIAN:
       *cairofilter = CAIRO_FILTER_GAUSSIAN;
       break;
    default:
       *cairofilter = GIZA_FILTER_BEST;
       break;
    }
  return;
}
