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
 * Copyright (C) 2010 James Wetter. All rights reserved.
 * Contact: wetter.j@gmail.com
 *
 */

#include "giza-colour-index-private.h"
#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-drivers-private.h"
#include <stdio.h>
#include <giza.h>

double colourIndex[GIZA_COLOUR_INDEX_MAX - GIZA_COLOUR_INDEX_MIN + 1][4];
int _giza_ci;

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

  cairo_set_source_rgba (context, colourIndex[ci][0],
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

  if (ci < GIZA_COLOUR_INDEX_MIN || ci > GIZA_COLOUR_INDEX_MAX)
    {
      _giza_warning ("giza_set_colour_representation",
		     "Invalid colour index, colour representation not set");
      return;
    }

  colourIndex[ci][0] = red;
  colourIndex[ci][1] = green;
  colourIndex[ci][2] = blue;
  colourIndex[ci][3] = 1.;

  /* If we are changing the current colour index
     make the change teke effect immediately 
   */
  if (ci == _giza_ci) 
     {
       cairo_set_source_rgba (context, colourIndex[ci][0],
			 colourIndex[ci][1], colourIndex[ci][2],
			 colourIndex[ci][3]);
     }

/*  if (ci == 0)
    {
      giza_draw_background ();
    }
*/
}

/**
 * Settings: giza_set_colour_representation_float
 *
 * Synopsis: Same functionality as giza_set_colour_representation but takes floats/
 */
void
giza_set_colour_representation_float (int ci, float red, float green,
				      float blue)
{
  giza_set_colour_representation (ci, (double) red, (double) green,
				  (double) blue);
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
  if (ci == _giza_ci) 
     {
       cairo_set_source_rgba (context, colourIndex[ci][0],
			 colourIndex[ci][1], colourIndex[ci][2],
			 colourIndex[ci][3]);
     }

/*  if (ci == 0)
    {
      giza_draw_background ();
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
 * Settings: giza_get_colour_representation
 *
 * Synopsis: Query the RGB at a given colour index.
 *
 * Input:
 *  -ci    :- The index to enquire about
 *  -red   :- Gets set to the red value at ci
 *  -green :- Gets set to the green value at ci
 *  -blue  :- Gets set to the blue value at ci
 */
void
giza_get_colour_representation (int ci, double *red, double *green,
				double *blue)
{
  if (!_giza_check_device_ready ("giza_get_colour_representation"))
    return;

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
    return;

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
  giza_set_colour_representation (0, 1., 1., 1.);	// white
  giza_set_colour_representation (1, 0., 0., 0.);	// black

  // red
  //giza_set_colour_representation (2, 1., 0., 0.);
  giza_set_colour_representation (2, .933, 0.173, 0.173);

  // green
  //giza_set_colour_representation (3, 0., 1., 0.);
  giza_set_colour_representation (3, 0.18, 0.545, 0.341);

  // blue
  giza_set_colour_representation (4, 0., 0., 1.);

  // Dark slate gray
  giza_set_colour_representation (5, 0.192, 0.31, 0.31);

  // Dark violet
  giza_set_colour_representation (6, 0.58, 0., 0.827);


  // cyan
  giza_set_colour_representation (7, 0.0, 0.825, 0.825);

  // Indian red
  //giza_set_colour_representation (7, 0.804, 0.361, 0.361); 

  // Gold
  //giza_set_colour_representation (7, 1., 0.843, 0.);
  giza_set_colour_representation (8, 1., 0.31, 0.);

  // cadet blue
  giza_set_colour_representation (9, 0.373, 0.62, 0.627);

  // peach puff
  giza_set_colour_representation (10, 0.933, 0.796, 0.678);

  giza_set_colour_representation (11, 0.0, 1.0, 0.5);
  giza_set_colour_representation (12, 0.0, 0.5, 1.0);
  giza_set_colour_representation (13, 0.5, 0.0, 0.0);
  giza_set_colour_representation (14, 1.0, 0.0, 0.5);
  giza_set_colour_representation (15, 0.333, 0.333, 0.333);
  giza_set_colour_representation (16, 0.667, 0.667, 0.667);

  // All alpha default to 1
  colourIndex[0][3] = Dev.defaultBackgroundAlpha;
  int i;
  for (i = GIZA_COLOUR_INDEX_MIN + 1; i <= GIZA_COLOUR_INDEX_MAX; ++i)
    {
      colourIndex[i][3] = 1.;
    }

  giza_set_colour_index (1);
}
