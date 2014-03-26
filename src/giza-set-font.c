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
#include "giza-set-font-private.h"
#include <giza.h>
#include <stdlib.h>
#include <string.h>
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
#ifdef GIZA_HAS_FT_FONT
#ifdef CAIRO_HAS_FT_FONT
#include <cairo/cairo-ft.h>
static FT_Library ft_library;
static FT_Face    ft_face;
static const cairo_user_data_key_t key;
static int got_ftlibrary = 0;
#endif
#endif
#endif
static char *_giza_fontFam;
static cairo_font_face_t *_giza_fontFace;
static int got_ftfont = 0;

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
 * Settings: giza_set_font_bold_italic
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
       return;
     }

   /* store existing font matrix */
   cairo_matrix_t mat;
   cairo_get_font_matrix (Dev[id].context, &mat);

   _giza_fontFam = realloc (_giza_fontFam, (strlen (font) + 1) * sizeof (char));
   strcpy (_giza_fontFam, font);

#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
#ifdef GIZA_HAS_FT_FONT
   if (!got_ftlibrary)
     {
        got_ftlibrary = !FT_Init_FreeType( &ft_library );
     }
   if (!got_ftlibrary)
     {
       _giza_error("giza_set_font","error initialising freetype library");
       got_ftfont = 0;
     }
     else 
     {
       /* destroy any previously created face */
       if (got_ftfont) { FT_Done_Face (ft_face); }

       /* create new freetype font face */
       got_ftfont = !FT_New_Face(ft_library,font,0,&ft_face);
     }

   if (got_ftfont) 
     {
       /* create the font face */
       _giza_fontFace = cairo_ft_font_face_create_for_ft_face(ft_face,0);
       
       /* tie the lifetime of the cairo font object to the FT_Done_Face call
        * (as per the cairo documentation) */
       cairo_font_face_set_user_data (_giza_fontFace, &key,
                           ft_face, (cairo_destroy_func_t) FT_Done_Face);
       
       /* set this as the current font face */
       cairo_set_font_face(Dev[id].context,_giza_fontFace);
     }
#endif
   /* If cairo version is new enough,
    * do the font face creation explicitly rather than
    * using select_font_face. This is so that memory
    * associated with the font face can be explicitly freed
    */
   if (!got_ftfont) 
     {
       _giza_fontFace = cairo_toy_font_face_create(font, slant, weight);
       cairo_set_font_face(Dev[id].context, _giza_fontFace);
     }
#else
   /*
    * For older versions of cairo, we must use
    * cairo_select_font_face
    */
   cairo_select_font_face (Dev[id].context, font, slant, weight);
#endif

   double oldCh;
   giza_get_character_height (&oldCh);
   giza_set_character_height (oldCh);

   /* restore font matrix */
   cairo_set_font_matrix (Dev[id].context, &mat);

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
  char *tmp = getenv ("GIZA_FONT");
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
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
   /* free memory associated with the font face */
  cairo_font_face_destroy (_giza_fontFace);
#ifdef GIZA_HAS_FT_FONT
  if (got_ftfont)
    {
      FT_Done_Face (ft_face);
      got_ftfont = 0;
    }
  if (got_ftlibrary)
    {
      FT_Done_FreeType (ft_library);
      got_ftlibrary = 0;
    }
#endif
#endif
  free (_giza_fontFam);
}
