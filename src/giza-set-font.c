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

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-set-font-private.h"
#include <giza.h>
#include <stdlib.h>
#include <string.h>

/* Depending on cairo version + capabilities enable FreeType and/or FontConfig support */
#undef GIZA_HAS_FT_FONT
#undef GIZA_HAS_FC_FONT

#include <cairo/cairo.h>
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 8, 0)
#   ifdef CAIRO_HAS_FT_FONT
#       define  GIZA_HAS_FT_FONT
#       include <cairo/cairo-ft.h>
#   endif /* CAIRO has FreeType support */
#   ifdef CAIRO_HAS_FC_FONT
#       define GIZA_HAS_FC_FONT
#       include <fontconfig/fontconfig.h>
#   endif /* CAIRO has FontConfig support */
#endif /* cairo >= 1.8.0 */


/* internal font cache actions */
giza_font_t*  _giza_cache_get_font(const char*, cairo_font_slant_t, cairo_font_weight_t);
void          _giza_cache_release_font(giza_font_t*);


/**
 * Settings: giza_set_font
 *
 * Synopsis: Sets the current font to the family specified in font, otherwise it is unchanged.
 *
 * Input:
 *  -font :- the font family to be used.
 */
void
giza_set_font (const char *font)
{
   if (!_giza_check_device_ready ("giza_set_font"))
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
giza_set_font_bold (const char *font)
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
giza_set_font_italic (const char *font)
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
giza_set_font_bold_italic (const char *font)
{
   if (!_giza_check_device_ready ("giza_set_font_bold_italic"))
    return;

   _giza_set_font(font,CAIRO_FONT_SLANT_ITALIC,CAIRO_FONT_WEIGHT_BOLD);
}


void
_giza_set_font (const char *font, cairo_font_slant_t slant, cairo_font_weight_t weight)
{
   if (!_giza_check_device_ready ("_giza_set_font"))
    return;

   if(!font || strlen(font)==0)
     {
       _giza_warning ("_giza_set_font", "No font string passed, font unchanged.");
       return;
     }

   /* Get a handle for the requested font */
   giza_font_t*       fontptr = _giza_cache_get_font(font, slant, weight);

   if( fontptr==NULL ) {
       _giza_error("_giza_set_font", "Failed to get font for font=%s, slant=%d, weight=%d for Dev[%d] (font unchanged)", font, slant, weight, id);
       return;
   }

   /* store existing font matrix */
   cairo_matrix_t     mat;
   cairo_get_font_matrix (Dev[id].context, &mat);

   /* Release the Dev's current font and install the new one */
   _giza_cache_release_font(Dev[id].Font);
   Dev[id].Font = fontptr;
   Dev[id].Font->refCount++;

   /* And tell cairo to use this'un */
   cairo_set_font_face(Dev[id].context, Dev[id].Font->font);

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
       _giza_warning ("giza_get_font", "Destination string is too short; sais %d characters available", n);
       return;
     }
   if( Dev[id].Font==NULL )
   {
       _giza_warning ("giza_get_font", "Device %d has no Font set?", id);
       font[0] = '\0';
       return;
   }
   /* note: the .family member is always valid */
   strncpy (font,  Dev[id].Font->family, n - 1);
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
  giza_set_font (tmp);
}

/**
 * release the font for the current device
 */
void
_giza_free_font (void)
{
  _giza_cache_release_font( Dev[id].Font );
  Dev[id].Font = NULL;
}


/*
 * Attempt to load the specified font, using FT or FontConfig - 
 * whichever is enabled and works */
cairo_font_face_t*
_giza_resolve_font(const char* font, cairo_font_slant_t slant, cairo_font_weight_t weight)
{
    /* for error reporting, keep track of which mechanisms were tried to get the font*/
    char               tried[128] = {0,};
    /* The new font, if any */
    cairo_font_face_t* newFont = NULL;

#ifdef GIZA_HAS_FT_FONT
    /* Attempt one: try FreeType font.
     * We must remember (1) did we try to initalize the FT library and
     * (2) what was the result of that */
    static int                         didFtInit = 0, ftInitOk = 0;
    static FT_Library                  ft_library;
    static const cairo_user_data_key_t key;

    if( !didFtInit ) {
       ftInitOk  = !FT_Init_FreeType( &ft_library ); 
       didFtInit = 1;
       if( !ftInitOk )
           _giza_warning("_giza_resolve_font","error initialising freetype library");
    }

    /* If FT Library was initialized we may ask it to get the font */
    if( ftInitOk ) {
        FT_Face newFace;
        if( FT_New_Face(ft_library, font, 0, &newFace)==0 ) {
            /* Can we make a font out of that one? */
            if( (newFont=cairo_ft_font_face_create_for_ft_face(newFace, 0)) != NULL ) {
                /* tie the lifetime of the FT_Face object to the
                 * cairo font object as per cairo documentation */
                cairo_font_face_set_user_data(newFont, &key, newFace, (cairo_destroy_func_t) FT_Done_Face);
            } else {
                /* We got a FT_Face but not a cairo font */
                _giza_error("giza_set_font", "FT_Face succeeded for font %s but cairo_font_face_create_for_ft failed", font);
                /* The FT_Face can go now */
                FT_Done_Face(newFace);
            }
        }
        /* We did try FreeType */
        strcat(tried, "FreeType ");
    } /* FreeType lib initialized Ok */

#endif /* GIZA_HAS_FT_FONT */

#ifdef GIZA_HAS_FC_FONT
    /* Try fontconfig if we seem to has it and no new font yet.
     * Also need to keep track of wether the library was initialised
     * and if so what the result of that was */
    static FcBool    didFcInit = FcFalse, fcInitOk = FcFalse;
    static FcConfig* configPtr = NULL;

    if( didFcInit==FcFalse ) {
        fcInitOk  = FcInit();
        configPtr = (fcInitOk==FcTrue ? FcInitLoadConfigAndFonts() : NULL);
        fcInitOk  = (fcInitOk && configPtr!=NULL) ? FcTrue : FcFalse;
        didFcInit = FcTrue;
        if( fcInitOk==FcFalse )
           _giza_warning("giza_resolve_font","error initialising fontconfig library");
    }

    if( fcInitOk && newFont==NULL ) {
        /* See if we can load the requested font through FontConfig */
        FcPattern*  search = FcPatternBuild(0, FC_FAMILY, FcTypeString , font,
                                               FC_SLANT,  FcTypeInteger, (slant==CAIRO_FONT_SLANT_ITALIC ? FC_SLANT_ITALIC : FC_SLANT_ROMAN),
                                               FC_WEIGHT, FcTypeInteger, (weight==CAIRO_FONT_WEIGHT_BOLD ? FC_WEIGHT_BOLD  : FC_WEIGHT_NORMAL),
                                               (char *)0 /* sentinel to stop parsing fields */
                                            );
        if( search ) {
            /* FontConfig yielded a result. Great. */
            if( (newFont=cairo_ft_font_face_create_for_pattern(search))==NULL )
                _giza_error("giza_resolve_font", "FontConfig search succeeded for font %s but cairo_ft_font_face_create_for_pattern didn't", font);
            /* Whichever the result was, we don't need the search result anymore */
            FcPatternDestroy( search );
        } else {
            _giza_warning("giza_resolve_font", "Could not create FontConfig search pattern for font %s", font);
        }
        /* We tried FontConfig */
        strcat(tried, "FontConfig ");
    } /* FontConfig OK and font not yet found */

#endif /* GIZA_HAS_FC_FONT */

    /* If still no font, try toy_font_face_create .... */
    if( newFont==NULL && (newFont=cairo_toy_font_face_create(font, slant, weight))==NULL )
        strcat(tried, "cairo_toy_font ");
   
    if( newFont==NULL ) 
        _giza_warning("_giza_resolve_font","No cairo_font_t for font=%s, slant=%d, weight=%d [tried: %s]", font, slant, weight, tried);
    return newFont;
}


/*************************************************************
 *
 *  The internal font cache
 *
 *************************************************************/

typedef struct _giza_font_cache_element_t {
    struct _giza_font_cache_element_t* next;
    struct _giza_font_cache_element_t* previous;
    giza_font_t                        font;
} giza_font_cache_element_t;

static giza_font_cache_element_t*   _giza_font_cache = NULL;

/* Check if an entry for the font already present in the cache.
 * - Yes? Just return the giza_font_t in that entry
 * - No?  Attempt to resolve the font (FreeType/FontConfig/Toy)
 *        and add a new entry to the cache with refcount == 1
 *
 * Parameters:
 * -family: name of the font, assume already tested for non-NULL-ness
 * -slant:  the requested slant of the font
 * -weigth: the requested weight of the font
 */
giza_font_t*
_giza_cache_get_font(const char* family, cairo_font_slant_t slant, cairo_font_weight_t weight)
{
    giza_font_cache_element_t*  cur;

    /* Check if we already have this one */
    for(cur = _giza_font_cache; cur!=NULL; cur = cur->next)
        /* case insensitive font family check */
        if( strcasecmp(cur->font.family, family)==0 && cur->font.slant==slant && cur->font.weight==weight )
            break;
    if( cur!=NULL )
        return &cur->font;

    /* If we didn't find it, let's add one.
       Let cairo create a new font face */
    cairo_font_face_t*  newFont = _giza_resolve_font(family, slant, weight);
    if( newFont==NULL ) {
        _giza_error("_giza_cache_get_font", "Failed to load font (family=%s, slant=%d, weight=%d)", family, slant, weight);
        return NULL;
    }
    /* Create new entry */
    giza_font_cache_element_t*  newEntry = (giza_font_cache_element_t*)calloc( 1, sizeof(giza_font_cache_element_t) );
    if( newEntry==NULL ) {
        _giza_error("_giza_cache_get_font", "Out of memory allocating giza_font_cache entry");
        /* Release the cairo font */
        cairo_font_face_destroy( newFont );
        return NULL;
    }
    /* Fill in the new cache entry.
     * Memory allocation story: we initialize the refCount to '1' here.
     * This effectively means the font will remain in memory for the
     * lifetime of the library - devices opening, closing, changing font,
     * will never make the refcount drop to zero (*).
     *
     * Reason: otherwise each font would be unloaded _at every font change_
     *         which would be a tad inefficient, see this scenario:
     *      
     *      open device -> sets default font, refcount == 1
     *      application sets cursive font:
     *          - decrease ref on current font (default) => goes to 0 =>
     *            delete font
     *          - allocate cursive font, set refcount to 1
     *          - change back to default font:
     *              - decrease refcount on current font (cursive): 1 -> 0
     *                  delete cursive font
     *              - get entry for default font: oh no! we just deleted it,
     *                must re-create it ...
     *      etc etc
     *
     * The downside is that at program shutdown (PGEND), extra code is added
     * to force-unload the font cache.
     *
     * By the way, this is also the reason why we don't use the
     *   cairo_font_face_reference() counting mechanism because it would suffer from
     *   the same problem. If there's only one device with one current font,
     *   the refcount would toggle between 0,1,0 on every font change, making
     *   cairo delete+allocate+delete+allocate etc.
     * By the 2nd way: investigating cairo_font_face_reference_count() 
     *   seems to be out of sync with what WE do. When manually doing
     *   while( cairo_font_face_reference_count(face) )
     *     cairo_font_face_destroy(face);
     *   it immediately SIGSEGVs or bombs out with double delete.
     *   So relying on cairo's internal font refcounting seems not a good
     *   idea at this point.
     *
     * (*) Not /entirely/ true; in order to ensure _proper_ memory management,
     *     code was added to force-unload the font cache in case no open
     *     device(s) remain after PGCLOS. Some applications don't call PGEND.
     *     This means that the cache may be dropped/rebuilt over the
     *     lifetime of the library, e.g. if an application closes all
     *     devices and then starts opening new one(s) again.
     *     Still, this a significantly less overhead compared to
     *     loading/unloading fonts at every font change.
     */
    newEntry->font.family   = strdup(family); /* No worries, success will be checked below. */
    newEntry->font.slant    = slant;
    newEntry->font.weight   = weight;
    newEntry->font.refCount = 1;              /* This will keep the fonts in memory 'for ever' */
    newEntry->font.font     = newFont;

    if( newEntry->font.family==NULL ) {
        _giza_error("_giza_cache_get_font", "Out of memory copying font family into cache");
        /* Release the cairo font */
        cairo_font_face_destroy( newFont );
        /* And the memory for the new entry */
        free( newEntry );
        return NULL;
    }
    /* Now we're all set to add this one to the cache.
     * Easiest is at the head of the list */
    if( _giza_font_cache )
        _giza_font_cache->previous = newEntry;
    newEntry->next   = _giza_font_cache;
    _giza_font_cache = newEntry;

    return &newEntry->font;
}


/* If a device is closed or another font is set, release one instance of the
 * current font */
void
_giza_cache_release_font(giza_font_t* fontptr)
{
    giza_font_cache_element_t*  cur;

    /* Check which cache element has this font */
    for(cur = _giza_font_cache; cur!=NULL; cur = cur->next)
        if( &cur->font==fontptr )
            break;
    if( cur==NULL ) {
        /* this is an error if fontptr != NULL */
        if( fontptr!=NULL )
            _giza_error("_giza_cache_release_font", "Internal error: no cache element has font pointer == %p", (void*)fontptr);
        return;
    }
    /* Decrease refcount. If still > 0 we're quickly done */
    if( --cur->font.refCount > 0 )
        return;

    /* No more references to this font */
    cairo_font_face_destroy( cur->font.font );
    free( cur->font.family );
    /* remove it from the cache */
    if( cur->previous )
        cur->previous->next = cur->next;
    if( cur->next )
        cur->next->previous = cur->previous;
    /* Did we remove the head of the list? */
    if( cur==_giza_font_cache )
        _giza_font_cache = cur->next;
    /* and release the memory for the cache element */
    free( cur );
}

/* At program end (giza_close_devices) the whole font cache will be unloaded */
void
_giza_unload_font_cache (void)
{
  /* The simplest approach ... */
  while( _giza_font_cache )
    _giza_cache_release_font( &_giza_font_cache->font );
}

