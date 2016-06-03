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

#include "giza-colour-private.h"
#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-drivers-private.h"
#include <stdio.h>
#include <giza.h>
#include <math.h>

static double colourIndex[GIZA_COLOUR_INDEX_MAX - GIZA_COLOUR_INDEX_MIN + 1][4];
static int _giza_ci;

/* these are declared extern in the header file */
int _giza_colour_index_min;
int _giza_colour_index_max;

/**
 * Settings: giza_set_colour_index
 *
 * Synopsis: Sets the colour to that represented by the colour index ci.
 * Represented colours can be changed via giza_set_colour_representation.
 *
 * Input:
 *   -ci :- The new colour index.
 */
void
giza_set_colour_index (int ci)
{
  if (!_giza_check_device_ready ("giza_set_colour_index"))
    return;

  if (ci < GIZA_COLOUR_INDEX_MIN || ci > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_warning ("giza_set_colour_index",
		     "Invalid colour index, colour index not set");
      return;
    }

  _giza_ci = ci;

  cairo_set_source_rgba (Dev[id].context, colourIndex[ci][0],
			 colourIndex[ci][1], colourIndex[ci][2],
			 colourIndex[ci][3]);
}

/**
 * Settings: giza_get_colour_index
 *
 * Synopsis: Queries the current colour index.
 *
 * Input:
 *   -ci :- gets set to the current colour index.
 */
void
giza_get_colour_index (int *ci)
{
  if (!_giza_check_device_ready ("giza_get_colour_index"))
    return;

  *ci = _giza_ci;
}

/**
 * Settings: giza_set_colour_representation
 *
 * Synopsis: Allows the user to set the colour represented by the given colour index.
 *
 * Input:
 *   -ci   :- Which colour index to set.
 *   -red  :- The red component of the colour (between 0 and 1).
 *   -green  :- The green component of the colour (between 0 and 1).
 *   -blue  :- The blue component of the colour (between 0 and 1).
 */
void
giza_set_colour_representation (int ci, double red, double green, double blue)
{
  if (!_giza_check_device_ready ("giza_set_colour_representation"))
    return;

  giza_set_colour_representation_alpha (ci, red, green, blue, 1.);
}

/**
 * Settings: giza_set_colour_representation_float
 *
 * Synopsis: Same functionality as giza_set_colour_representation but takes floats/
 */
void
giza_set_colour_representation_float (int ci, float red, float green, float blue)
{
  giza_set_colour_representation (ci, (double) red, (double) green, (double) blue);
}

/**
 * Settings: giza_set_colour_representation_alpha
 *
 * Synopsis: Allows the user to set the colour represented by the given colour index, aswell
 * as the alpha.
 *
 * Input:
 *   -ci   :- Which colour index to set.
 *   -red  :- The red component of the colour (between 0 and 1).
 *   -green  :- The green component of the colour (between 0 and 1).
 *   -blue  :- The blue component of the colour (between 0 and 1).
 *   -alpha :- The alpha used when drawing with this colour index (between 0 and 1).
 */
void
giza_set_colour_representation_alpha (int ci, double red, double green,
				      double blue, double alpha)
{
  if (!_giza_check_device_ready ("giza_set_colour_representation"))
    return;

  if (ci < GIZA_COLOUR_INDEX_MIN || ci > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_warning ("giza_set_colour_representation",
		     "Invalid colour index, colour representation not set");
      return;
    }

  colourIndex[ci][0] = red;
  colourIndex[ci][1] = green;
  colourIndex[ci][2] = blue;
  colourIndex[ci][3] = alpha;

  /* If we are changing the current colour index
     make the change teke effect immediately
   */
  if (ci == _giza_ci) giza_set_colour_index(ci);
/*  if (ci == 0) {
     printf("in scr ci = %i rgb = %f %f %f %f drawn = %i \n",ci,red,green,blue,alpha,Dev[id].drawn);
     giza_draw_background();
     _giza_stroke();
     giza_flush_device();
  }
*/
}

/**
 * Settings: giza_set_colour_representation_alpha_float
 *
 * Synopsis: Same functionality as giza_set_colour_representation_alpha but takes floats.
 *
 * See Also: giza_set_colour_representation_alpha, giza_set_colour_representation
 */
void
giza_set_colour_representation_alpha_float (int ci, float red, float green,
					    float blue, float alpha)
{
  giza_set_colour_representation_alpha (ci, (double) red, (double) green,
					(double) blue, (double) alpha);
}

/**
 * Settings: giza_set_colour_representation_hls
 *
 * Synopsis: Allows the user to set the colour represented by the given colour index
 *           This routine accepts colours in the Hue, Lightness and Saturation system.
 *
 * Input:
 *   -ci   :- Which colour index to set.
 *   -hue         :- The Hue component of the colour (between 0 and 360 degrees).
 *   -lightness   :- The Lightness component of the colour (between 0 and 1).
 *   -saturation  :- The Saturation component of the colour (between 0 and 1).
 */
void
giza_set_colour_representation_hls (int ci, double hue, double lightness, double saturation)
{
  if (!_giza_check_device_ready ("giza_set_colour_representation"))
    return;

  double red, green, blue;
  _giza_hls_to_rgb(hue,lightness,saturation,&red,&green,&blue);
  giza_set_colour_representation_alpha (ci, red, green, blue, 1.);
}

/**
 * Settings: giza_set_colour_representation_hls_float
 *
 * Synopsis: Same functionality as giza_set_colour_representation_hls but takes floats
 */
void
giza_set_colour_representation_hls_float (int ci, float hue, float lightness, float saturation)
{
  double red, green, blue;
  _giza_hls_to_rgb((double) hue,(double) lightness,(double) saturation,&red,&green,&blue);
  giza_set_colour_representation (ci, red, green, blue);
}

/**
 * Settings: giza_set_colour_representation_rgb
 *
 * Synopsis: Same as giza_set_colour_representation but accepts integer 0->255 instead of double 0->1
 *
 * Input:
 *   -ci   :- Which colour index to set.
 *   -red  :- The red component of the colour (between 0 and 255).
 *   -green  :- The green component of the colour (between 0 and 255).
 *   -blue  :- The blue component of the colour (between 0 and 255).
 *
 * See Also: giza_set_colour_representation, giza_set_colour_representation_rgba
 */
void
giza_set_colour_representation_rgb (int ci, int red, int green, int blue)
{
  if (!_giza_check_device_ready ("giza_set_colour_representation_rgb"))
    return;

  giza_set_colour_representation_rgba (ci, red, green, blue, 1.);
}

/**
 * Settings: giza_set_colour_representation_rgba
 *
 * Synopsis: Same as giza_set_colour_representation_alpha but accepts 0->255 instead of 0->1
 *
 * Input:
 *   -ci   :- Which colour index to set.
 *   -red  :- The red component of the colour (between 0 and 255).
 *   -green  :- The green component of the colour (between 0 and 255).
 *   -blue  :- The blue component of the colour (between 0 and 255).
 *   -alpha :- The alpha component of the colour (between 0 and 1)
 *
 * See Also: giza_set_colour_representation_alpha, giza_set_colour_representation_rgb
 */
void
giza_set_colour_representation_rgba (int ci, int red, int green, int blue, double alpha)
{
  if (!_giza_check_device_ready ("giza_set_colour_representation_rgba"))
    return;

  giza_set_colour_representation_alpha (ci, ((double) red)/255., ((double) green)/255., ((double) blue)/255., alpha);
}

/**
 * Settings: giza_set_colour_representation_rgba_float
 *
 * Synopsis: Same functionality as giza_set_colour_representation_rgba but takes floats
 *
 * See Also: giza_set_colour_representation_rgba, giza_set_colour_representation_alpha
 */
void
giza_set_colour_representation_rgba_float (int ci, int red, int green, int blue, float alpha)
{
  giza_set_colour_representation_rgba (ci, red, green, blue, (double) alpha);
}


/**
 * Settings: giza_get_colour_representation
 *
 * Synopsis: Query the RGB at a given colour index.
 *
 * Input:
 *  -ci    :- The index to enquire about
 *  -red   :- Gets set to the red value at ci (range 0->1)
 *  -green :- Gets set to the green value at ci (range 0->1)
 *  -blue  :- Gets set to the blue value at ci (range 0->1)
 */
void
giza_get_colour_representation (int ci, double *red, double *green,
				double *blue)
{
  if (!_giza_check_device_ready ("giza_get_colour_representation"))
    {
      *red = 0.;
      *green = 0.;
      *blue = 0.;
      return;
    }

  if (ci < GIZA_COLOUR_INDEX_MIN || ci > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_warning ("giza_get_colour_representation",
		     "Invalid colour index");
      return;
    }

  *red = colourIndex[ci][0];
  *green = colourIndex[ci][1];
  *blue = colourIndex[ci][2];
}

/**
 * Settings giza_set_colour_representation_float
 *
 * Synopsis: Same functionality as giza_get_colour_representation but takes floats.
 */
void
giza_get_colour_representation_float (int ci, float *red, float *green,
				      float *blue)
{
  if (!_giza_check_device_ready ("giza_get_colour_representation_float"))
    return;

  if (ci < GIZA_COLOUR_INDEX_MIN || ci > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_error ("giza_get_colour_representation_float",
		   "Invalid colour index");
      return;
    }

  double dred, dgreen, dblue;
  giza_get_colour_representation (ci, &dred, &dgreen, &dblue);
  *red = (float) dred;
  *green = (float) dgreen;
  *blue = (float) dblue;
}

/**
 * Settings: giza_get_colour_representation_alpha
 *
 * Synopsis: Query the RGB and alpha at a given colour index.
 *
 * Input:
 *  -ci    :- The index to enquire about
 *  -red   :- Gets set to the red value at ci
 *  -green :- Gets set to the green value at ci
 *  -blue  :- Gets set to the blue value at ci
 *  -alpha :- Gets set to the alpha at ci
 */
void
giza_get_colour_representation_alpha (int ci, double *red, double *green,
				      double *blue, double *alpha)
{
  if (!_giza_check_device_ready ("giza_get_colour_representation"))
    {
      *red = 0.;
      *green = 0.;
      *blue = 0.;
      *alpha = 0.;
      return;
    }

  if (ci < GIZA_COLOUR_INDEX_MIN || ci > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_warning ("giza_get_colour_representation",
		     "Invalid colour index");
      return;
    }

  *red = colourIndex[ci][0];
  *green = colourIndex[ci][1];
  *blue = colourIndex[ci][2];
  *alpha = colourIndex[ci][3];
}

/**
 * Settings giza_set_colour_representation_alpha_float
 *
 * Synopsis: Same functionality as giza_get_colour_representation_alpha but takes floats.
 *
 * See Also: giza_get_colour_representation_alpha
 */
void
giza_get_colour_representation_alpha_float (int ci, float *red, float *green,
					    float *blue, float *alpha)
{
  if (!_giza_check_device_ready ("giza_get_colour_representation_float"))
    return;

  if (ci < GIZA_COLOUR_INDEX_MIN || ci > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_error ("giza_get_colour_representation_float",
		   "Invalid colour index");
      return;
    }

  double dred, dgreen, dblue, dalpha;
  giza_get_colour_representation_alpha (ci, &dred, &dgreen, &dblue, &dalpha);
  *red = (float) dred;
  *green = (float) dgreen;
  *blue = (float) dblue;
  *alpha = (float) dalpha;
}

void
_giza_init_colour_index (void)
{
  giza_set_colour_palette(GIZA_COLOUR_PALETTE_DEFAULT);

  /* All alpha default to 1 */
  colourIndex[0][3] = Dev[id].defaultBackgroundAlpha;
  int i;
  for (i = GIZA_COLOUR_INDEX_MIN + 1; i <= GIZA_COLOUR_INDEX_MAX; ++i)
    {
      colourIndex[i][3] = 1.;
    }

  giza_set_colour_index (1);
}

/**
 * Settings: giza_set_colour_index_range
 *
 * Synopsis: Set the range of colour indices that are affected by giza_set_colour_table
 *
 * Note: giza_render is not affected by this setting
 *
 * Input:
 *  -cimin  :- lowest colour index in range
 *  -cimax  :- highest colour index in range
 */
void
giza_set_colour_index_range (int cimin, int cimax)
{
  if (cimin < GIZA_COLOUR_INDEX_MIN)
    {
      _giza_colour_index_min = GIZA_COLOUR_INDEX_MIN;
    }
   else if (cimin > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_colour_index_min = GIZA_COLOUR_INDEX_MAX;
    }
   else
    {
      _giza_colour_index_min = cimin;
    }

  if (cimax < GIZA_COLOUR_INDEX_MIN)
    {
      _giza_colour_index_max = GIZA_COLOUR_INDEX_MIN;
    }
   else if (cimax > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_colour_index_max = GIZA_COLOUR_INDEX_MAX;
    }
   else
    {
      _giza_colour_index_max = cimax;
    }
}

/**
 * Settings: giza_get_colour_index_range
 *
 * Synopsis: Queries the current range of colour indices affected by giza_set_colour_table,
 *           as set by giza_set_colour_index_range
 *
 * Output:
 *  -cimin  :- lowest colour index in range
 *  -cimax  :- highest colour index in range
 */
void
giza_get_colour_index_range (int *cimin, int *cimax)
{
  *cimin = _giza_colour_index_min;
  *cimax = _giza_colour_index_max;
}

/**
 * Settings: giza_set_range_as_colour_table
 *
 * Synopsis: Can be used in place of giza_set_colour_table to install
 *           the colour table from a predefined set of colour indices
 *
 * See Also: giza_set_colour_table
 */
/*void
giza_set_range_as_colour_table (int *cimin, int *cimax)
{
  *cimin = _giza_colour_index_min;
  *cimax = _giza_colour_index_max;
}
*/

/**
 * Converts hls to rgb colour representation
 * See: http://en.wikipedia.org/wiki/HSL_and_HSV#From_HSL
 */
void _giza_hls_to_rgb (double hue, double lightness, double saturation,
                       double *red, double *green, double *blue)
{
  hue = fmod(hue,360.); /* wrap to 0->360 degrees */

  double Chroma,Hdash,X,R1,G1,B1,m;
  Chroma = (1. - fabs(2.*lightness - 1.))*saturation;
  Hdash = hue/60.;
  X = Chroma*(1. - fabs( fmod(Hdash,2.) - 1.));

  if (Hdash >= 0. && Hdash < 1.)
    {
      R1 = Chroma;
      G1 = X;
      B1 = 0.;
    }
  else if (Hdash < 2.)
    {
      R1 = X;
      G1 = Chroma;
      B1 = 0.;
    }
  else if (Hdash < 3.)
    {
      R1 = 0.;
      G1 = Chroma;
      B1 = X;
    }
  else if (Hdash < 4.)
    {
      R1 = 0.;
      G1 = X;
      B1 = Chroma;
    }
  else if (Hdash < 5.)
    {
      R1 = X;
      G1 = 0.;
      B1 = Chroma;
    }
  else if (Hdash < 6.)
    {
      R1 = Chroma;
      G1 = 0.;
      B1 = X;
    }
  else /* H undefined */
    {
      R1 = 0.;
      G1 = 0.;
      B1 = 0.;
    }

  /* Now get RGB by matching lightness */
  m = lightness - 0.5*Chroma;

  *red   = _giza_set_in_range(R1 + m,0.,1.);
  *green = _giza_set_in_range(G1 + m,0.,1.);
  *blue  = _giza_set_in_range(B1 + m,0.,1.);
}

/**
 * Ensures value of the input variable falls in the range
 * given by val1 and val2
 */
double _giza_set_in_range (double val, double val1, double val2)
{
  double valmin, valmax;
  if (val1 < val2)
    {
      valmin = val1;
      valmax = val2;
    }
  else
    {
      valmin = val2;
      valmax = val1;
    }

  if (val < valmin)
    return valmin;
  else if (val > valmax)
    return valmax;
  else
    return val;
}

/**
 * Internal routine to temporarily set alpha to a particular value
 * Note that this only affects the current colour index
 * and should be RESET to 1.0 after finishing
 */
void _giza_set_alpha (double alpha)
{
  int ci;
  double r,g,b,a;
  giza_get_colour_index(&ci);
  giza_get_colour_representation_alpha(ci,&r,&g,&b,&a);
  giza_set_colour_representation_alpha(ci,r,g,b,alpha);
}

void _giza_get_alpha (double *alpha)
{
  int ci;
  double r,g,b;
  giza_get_colour_index(&ci);
  giza_get_colour_representation_alpha(ci,&r,&g,&b,alpha);
}
