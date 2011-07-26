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
#include "giza-text-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <math.h>
#include <stdio.h>

void _giza_action_get_size (const char *text, double *width, double *height)
{
  if (strlen(text) <= 0) return;
  double newHeight, dummy;
  cairo_get_current_point (context, &dummy, &newHeight);
  cairo_text_extents_t extents;
  cairo_text_extents (context, text, &extents);

  dummy = newHeight;
  *width += extents.x_advance;
  newHeight = -newHeight - extents.y_bearing;
  if (newHeight > *height) *height = newHeight;
}

void _giza_action_print (const char *text, double *width, double *height)
{
  if (strlen(text) > 0) {
     cairo_show_text (context, text);
  }
}

/**
 * Moves the current position to continue writing in superscript.
 */
void
_giza_start_super (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  double x , y, height = Sets.fontExtents.ascent * GIZA_RAISE_SUPERS;

  x = height * sin (Sets.fontAngle);
  y = -height * cos (Sets.fontAngle);
  
  cairo_rel_move_to (context, x, y);

  /* DJP: we scale the character height relative to it's current size
   *    rather than setting the absolute height -- this preserves
   *    the font matrix and hence any rotation etc. that has been set
   */
  _giza_scale_character_size (GIZA_SCALE_SUPERS);

  _giza_set_trans (oldTrans);
}

/**
 * If currently writing superscript returns to normal position.
 */
void
_giza_stop_super (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  _giza_scale_character_size (1. / GIZA_SCALE_SUPERS);

  double x , y, height = Sets.fontExtents.ascent * GIZA_RAISE_SUPERS;

  x = -height * sin (Sets.fontAngle);
  y = height * cos (Sets.fontAngle);
  
  cairo_rel_move_to (context, x, y);
  _giza_set_trans (oldTrans);
}

/**
 * Moves the current position to continue writing in subscript.
 *
 * Must be in GIZA_TRANS_IDEN.
 */
void
_giza_start_sub (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  double x , y, height = -Sets.fontExtents.ascent * GIZA_LOWER_SUBS;

  x = height * sin (Sets.fontAngle);
  y = -height * cos (Sets.fontAngle);
  
  cairo_rel_move_to (context, x, y);

  _giza_scale_character_size (GIZA_SCALE_SUBS);
  _giza_set_trans (oldTrans);
}

/**
 * If currently writing subscript returns to normal position.
 *
 * Must be in GIZA_TRANS_IDEN.
 */
void
_giza_stop_sub (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  _giza_scale_character_size (1. / GIZA_SCALE_SUBS);

  double x , y, height = -Sets.fontExtents.ascent * GIZA_LOWER_SUBS;

  x = -height * sin (Sets.fontAngle);
  y = height * cos (Sets.fontAngle);
  
  cairo_rel_move_to (context, x, y);
  _giza_set_trans (oldTrans);
}

/**
 * Switches font family during text write
 *
 */
void
_giza_switch_font (int fonttype)
{
  switch(fonttype)
    {
    case 4:
      giza_set_font("cursive");
      break;
    case 3:
      giza_set_font_italic("times");
      break;
    case 2:
      giza_set_font("times");
      break;
    case 1:
      giza_set_font("arial");
      break;
    }
}
