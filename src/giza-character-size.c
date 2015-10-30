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

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-transforms-private.h"
#include <giza.h>

/**
 * Settings: giza_set_character_height
 *
 * Synopsis: Sets the font size in units of character height.
 *
 * Input:
 *  -ch :- the new character height
 */
void
giza_set_character_height (double ch)
{
  if (!_giza_check_device_ready ("giza_set_character_height"))
    return;
  if (ch < 0)
    {
      _giza_warning ("giza_set_character_height",
		    "Invalid character height, character height not set");
      return;
    }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  double chDevice = ch * Dev[id].panelheight * .027;
  /*double chDevice = ch * Dev[id].height * .027;*/
  cairo_set_font_size (Dev[id].context, chDevice);
  /* query font extents and store this */
  cairo_font_extents (Dev[id].context, &Dev[id].fontExtents);
  Dev[id].ch = ch;

  _giza_set_trans (oldTrans);

}

/**
 * Settings: giza_set_character_height_float
 *
 * Synopsis: Same functionality as giza_set_character_height but takes a float
 */
void
giza_set_character_height_float (float ch)
{
  giza_set_character_height ((double) ch);
}

/**
 * Settings: giza_get_character_height
 *
 * Synopsis: Query the character height
 *
 * Input:
 *  -ch :- gets set to the character height.
 */
void
giza_get_character_height (double *ch)
{
  if (!_giza_check_device_ready ("giza_get_character_height"))
    {
      *ch = 1.;
      return;
    }

  *ch = Dev[id].ch;
}

/**
 * Settings: giza_get_character_height_float
 *
 * Synopsis: Same functionality as giza_get_character_height but takes a float.
 *
 * See Also: giza_get_character_height
 */
void
giza_get_character_height_float (float *ch)
{
  double dch;
  giza_get_character_height(&dch);
  *ch = (float) dch;
}

/**
 * Settings: giza_get_character_size
 *
 * Synopsis: Returns the character size in a variety of units.
 *
 * Input:
 *  -units  :- select the units the result will be returned in.
 *  -width  :- will be set to the width of a character.
 *  -height :- will be set to the height of a character.
 *
 * Units:
 *  -GIZA_UNITS_NORMALIZED :- normalised device coords.
 *  -GIZA_UNITS_WORLD      :- world coords.
 *  -GIZA_UNITS_PIXELS     :- pixels
 *  -GIZA_UNITS_DEVICE     :- device coords.
 *  -GIZA_UNITS_MM         :- mm
 *  -GIZA_UNITS_INCHES     :- inches
 *  Other values cause an error message and are treated as GIZA_UNITS_NORMALIZED
 */
void
giza_get_character_size (int units, double *heightx, double *heighty)
{
  if (!_giza_check_device_ready ("giza_get_character_height"))
    return;

  int oldTrans = _giza_get_trans ();

  *heighty = Dev[id].fontExtents.ascent;
  *heightx = *heighty;

  switch (units)
    {
    case GIZA_UNITS_NORMALIZED:
      _giza_set_trans (GIZA_TRANS_NORM);
      cairo_device_to_user_distance (Dev[id].context, heightx, heighty);
      *heighty = -*heighty;
      break;
    case GIZA_UNITS_MM:
    case GIZA_UNITS_INCHES:
    case GIZA_UNITS_PIXELS:
    case GIZA_UNITS_DEVICE:
      _giza_set_trans (GIZA_TRANS_IDEN);
      cairo_device_to_user_distance (Dev[id].context, heightx, heighty);
      break;
    case GIZA_UNITS_WORLD:
      _giza_set_trans (GIZA_TRANS_WORLD);
      cairo_device_to_user_distance (Dev[id].context, heightx, heighty);
      *heighty = -*heighty;
      break;
    default:
      _giza_warning ("giza_get_character_size", "Invalid units, using normalised device units.");
      _giza_set_trans (GIZA_TRANS_NORM);
      cairo_device_to_user_distance (Dev[id].context, heightx, heighty);
      *heighty = -*heighty;
      break;
    }

   /* Now convert to mm/inches */
   switch (units)
   {
   case GIZA_UNITS_PIXELS:
      *heightx = *heightx * Dev[id].deviceUnitsPerPixel;
      *heighty = *heighty * Dev[id].deviceUnitsPerPixel;
      break;
   case GIZA_UNITS_MM:
      *heightx = *heightx * Dev[id].deviceUnitsPermm;
      *heighty = *heighty * Dev[id].deviceUnitsPermm;
      break;
   case GIZA_UNITS_INCHES:
      *heightx = *heightx * Dev[id].deviceUnitsPermm/25.4;
      *heighty = *heighty * Dev[id].deviceUnitsPermm/25.4;
      break;
   default:
     break;
   }

  _giza_set_trans (oldTrans);
}

/**
 * Settings: giza_get_character_size_float
 *
 * Synopsis: Same functionality as giza_get_character_size, but returns the values as floats
 */
void
giza_get_character_size_float (int units, float *xch, float *ych)
{
  double dxch, dych;
  giza_get_character_size (units, &dxch, &dych);
  *xch = (float) dxch;
  *ych = (float) dych;
}

/**
 * Gives character of 1 a size relative to the current device size.
 * WARNING: will cause the character height to be set to one
 */
void
_giza_init_character_height (void)
{
  /* Note: do not call giza_set_character_height here
     This is done from giza_init_font */
  Dev[id].ch = 1.;
}

/**
 * Sets the character_size size relative to it's current size
 * this is slightly different to set_character_height
 * as it preserves the current font matrix
 */
void
_giza_scale_character_size (double scalefac)
{
  if (!_giza_check_device_ready ("_giza_scale_character_size"))
    return;
  if (scalefac < 0)
    {
      _giza_warning ("_giza_scale_character_size",
		    "Invalid character size, character size not set");
      return;
    }

  /* Sets the character size as a ratio of the current character size */
  /* so that the font matrix is not destroyed */
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  cairo_matrix_t mat;
  cairo_get_font_matrix (Dev[id].context, &mat);

/* Dev[id].ch = scalefac * Dev[id].ch; */

  cairo_matrix_scale (&mat, scalefac, scalefac);
  cairo_set_font_matrix (Dev[id].context, &mat);
  cairo_font_extents (Dev[id].context, &Dev[id].fontExtents);

  _giza_set_trans (oldTrans);

}
