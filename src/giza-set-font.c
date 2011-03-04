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

#include "giza-private.h"
#include "giza-io-private.h"
#include <giza.h>
#include <stdlib.h>
#include <string.h>

char *_giza_fontFam;


/**
 * Settings: giza_set_font
 *
 * Synopsis: Sets the current font to the family specified in font, otherwise it is unchanged.
 *
 * Input:
 *  -font :- the font family to be used.
 */
void
giza_set_font (char *font)
{
   if (!_giza_check_device_ready ("giza_set_font_bold"))
    return;

   _giza_set_font(font,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);

}

/**
 * Settings: giza_set_font_bold
 *
 * Synopsis: Sets the current font to the family specified in font, otherwise it is unchanged.
 *
 * Input:
 *  -font :- the font family to be used.
 */
void
giza_set_font_bold (char *font)
{
   if (!_giza_check_device_ready ("giza_set_font_bold"))
    return;

   _giza_set_font(font,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
}

/**
 * Settings: giza_set_font_italic
 *
 * Synopsis: Sets the current font to the family specified in font, otherwise it is unchanged.
 *
 * Input:
 *  -font :- the font family to be used.
 */
void
giza_set_font_italic (char *font)
{
   if (!_giza_check_device_ready ("giza_set_font_italic"))
    return;

   _giza_set_font(font,CAIRO_FONT_SLANT_ITALIC,CAIRO_FONT_WEIGHT_NORMAL);
}

/**
 * Settings: giza_set_font_italic
 *
 * Synopsis: Sets the current font to the family specified in font, otherwise it is unchanged.
 *
 * Input:
 *  -font :- the font family to be used.
 */
void
giza_set_font_bold_italic (char *font)
{
   if (!_giza_check_device_ready ("giza_set_font_bold_italic"))
    return;

   _giza_set_font(font,CAIRO_FONT_SLANT_ITALIC,CAIRO_FONT_WEIGHT_BOLD);
}


void
_giza_set_font (char *font, cairo_font_slant_t slant, cairo_font_weight_t weight)
{
   if (!_giza_check_device_ready ("giza_set_font"))
    return;

   if(!font)
     {
       _giza_warning ("giza_set_font", "No font string passed, font unchanged.");
     }

   _giza_fontFam = realloc (_giza_fontFam, (strlen (font) + 1) * sizeof (char));
   strcpy (_giza_fontFam, font);

   cairo_select_font_face (context, font, slant, weight);
   double oldCh;
   giza_get_character_height (&oldCh);
   giza_set_character_height (oldCh);
}

/**
 * Settings: giza_get_font
 *
 * Sets font to a string representing the font family.
 *
 * Input:
 *  -font :- Gets set to the current font family
 *  -n    :- The length of memory allocated to font
 *
 * See Also: giza_set_font
 */
void
giza_get_font (char *font, int n)
{
   if (!_giza_check_device_ready ("giza_get_font"))
    return;

   if (n < 1)
     {
       _giza_warning ("giza_get_font", "String is too short, returning null");
       font = NULL;
       return;
     }
   
   strncpy (font, _giza_fontFam, n - 1);
}

/**
 * Sets the default font and character height.
 *
 * If the environment variable GIZA_FONT is set
 * it's value will be used to set the font family,
 * otherwise the system specific default is used.
 */
void
_giza_init_font (void)
{
  char *tmp = getenv("GIZA_FONT");
  if(!tmp)
    {
      tmp = "Times";
    }
  
  _giza_fontFam = malloc ((strlen (tmp) + 1) * sizeof (char));
  strcpy (_giza_fontFam, tmp);

  giza_set_font (tmp);
}

/**
 * Frees the memory associated with fontFam
 */
void
_giza_free_font (void)
{
  free (_giza_fontFam);
}
