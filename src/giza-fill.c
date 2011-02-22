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

#include "giza.h"
#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-stroke-private.h"
#include "giza-transforms-private.h"

#define GIZA_FILL_SOLID  1
#define GIZA_FILL_HOLLOW 2
#define GIZA_FILL_HATCH 3
#define GIZA_FILL_CROSSHATCH 4

#define GIZA_MAX_FILL_STYLES 4


int _giza_fill_style;

/**
 * Settings: giza_set_fill
 *
 * Synopsis: Sets the current fill style.
 *
 * Input:
 *  -fs :- the new fill style
 *
 * Fill Styles:
 *  -1 or GIZA_FILL_SOLID       :- solid
 *  -2 or GIZA_FILL_HOLLOW      :- hollow
 *  -3 or GIZA_FILL_HATCH       :- hatch
 *  -4 or GIZA_FILL_CROSSHATCH  :- crosshatch
 */
void
giza_set_fill (int fs)
{
  if (!_giza_check_device_ready ("giza_set_fill"))
    return;
  if (fs < 1 || fs > GIZA_MAX_FILL_STYLES)
    {
      printf(" fill style = %i \n",fs);
      _giza_warning ("giza_set_fill", "Invalid fill style, fill style not set");
      return;
    }

  _giza_fill_style = fs;
}

/**
 * Settings: giza_get_fill
 *
 * Synopsis: Query the current fill style.
 *
 * Input:
 *  -fs :- gets set to the current fill style.
 */
void
giza_get_fill (int *fs)
{
  if (!_giza_check_device_ready ("giza_get_fill"))
    return;

  *fs = _giza_fill_style;
}

void
_giza_init_fill (void)
{
  _giza_fill_style = GIZA_FILL_SOLID;
}

/**
 * Fills the currently drawn path.
 */
void
_giza_fill (void)
{
  if(!_giza_check_device_ready ("_giza_fill"))
     return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  cairo_t *ct;
  cairo_surface_t *hatchsurface;
  cairo_pattern_t *hatchpattern;
  double ri,gi,bi,alphai;
  int ci;
  int hatch_size;
  int lw;

  switch (_giza_fill_style)
    {
    case GIZA_FILL_CROSSHATCH:
    case GIZA_FILL_HATCH:
      /* For hatched pattern we must initialise
       * the pattern every time fill is called,
       * because the hatching needs to be done 
       * with the current colour index
       */
      hatch_size = 7;
      lw = 1.5;
      /* create a temporary bitmap (cairo surface) to hold the hatching pattern */
      hatchsurface = cairo_surface_create_similar(cairo_get_target(context),
                                                  CAIRO_CONTENT_COLOR,hatch_size,hatch_size);
      ct = cairo_create(hatchsurface);
      /* query the current background colour and 
       * set the paint colour equal to this      */
      giza_get_colour_representation_alpha(GIZA_BACKGROUND_COLOUR,&ri,&gi,&bi,&alphai);
      cairo_set_source_rgba(ct, ri, gi, bi, alphai);
      /* paint with this pattern */
      cairo_rectangle(ct,0,0,hatch_size,hatch_size);
      cairo_fill(ct);

      /* draw lines making the pattern with current colour index */
      giza_get_colour_index(&ci);
      giza_get_colour_representation_alpha(ci,&ri,&gi,&bi,&alphai);
      cairo_set_source_rgba(ct, ri, gi, bi, alphai);
      cairo_set_line_width(ct, lw);
      //cairo_set_line_cap(ct, CAIRO_LINE_CAP_BUTT);
      //cairo_set_line_join(ct, CAIRO_LINE_JOIN_BEVEL);
      //cairo_pattern_set_extend(cairo_get_source(ct), CAIRO_EXTEND_PAD);
      if (_giza_fill_style == GIZA_FILL_CROSSHATCH) 
        {
          cairo_move_to(ct, hatch_size, hatch_size);
          cairo_line_to(ct, 0., 0.);
        } else {
          /* the antialiasing causes a dashed look in the hatched pattern
           * that is fixed by turning it off - there should be a better 
           * way but this is a workaround for the fact that cairo
           * renders the pattern pixmap before it is finally
           * rendered to the surface */
          cairo_set_antialias(ct, CAIRO_ANTIALIAS_NONE);    
        }
      cairo_move_to(ct, 0., hatch_size);
      cairo_line_to(ct, hatch_size, 0.);
      cairo_stroke(ct);

      /* finally, make our surface (bitmap) into a pattern object that can be used for fills */
      hatchpattern = cairo_pattern_create_for_surface(hatchsurface);

      /* finished with the temporary surface, so we can get rid of it */
      cairo_surface_destroy(hatchsurface);

      /* finally, set the fill for the actual context to be the pattern we have created */
      cairo_set_source(context, hatchpattern);
      cairo_pattern_set_extend(cairo_get_source(context), CAIRO_EXTEND_REPEAT);

      /* perform the fill */
      cairo_fill(context);
      
      /* destroy the pattern, cleanup */
      cairo_pattern_destroy(hatchpattern);
      break;

    case GIZA_FILL_SOLID:
      cairo_fill (context);
      break;
    case GIZA_FILL_HOLLOW:
      _giza_stroke ();
      break;
    default:
      _giza_warning ("_giza_fill", "Invalid fill style");
      _giza_stroke ();
      break;
    }

  _giza_set_trans (oldTrans);
}
