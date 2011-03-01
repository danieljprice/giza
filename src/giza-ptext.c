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
#include "giza-text-private.h"
#include "giza-text-background-private.h"
#include "giza-transforms-private.h"
#include "giza-stroke-private.h"
#include "giza-drivers-private.h"
#include <giza.h>
#include <math.h>

/**
 * Text: giza_ptext
 *
 * Synopsis: Draws text at a given position in world coords at a given angle with a given justification.
 *
 * Input:
 *  -x     :- The x world coord.
 *  -y     :- The y world coord.
 *  -angle :- The angle to be drawn at.
 *  -just  :- The justification.
 *  -text  :- The text to be drawn.
 */
void
giza_ptext (double x, double y, double angle, double just, char *text)
{
  if (!_giza_check_device_ready ("giza_ptext"))
    {
      return;
    }

  // save the character height (can be changed during superscript/subscripting)
  double ch;
  giza_get_character_height (&ch);

  cairo_save (context);

  _giza_expand_clipping ();

  // change x and y to device coords
  //cairo_user_to_device (context, &x, &y);

  double xbox[4], ybox[4];
  giza_qtext (x, y, angle, just, text, xbox, ybox);

  // Draw the bounding box
  if (_giza_text_background >= 0)
  {
    int oldCi;
    giza_get_colour_index (&oldCi);
    giza_set_colour_index (_giza_text_background);
    giza_polygon (4, xbox, ybox);
    giza_set_colour_index (oldCi);
  }

  // change the current trans to world coords
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_move_to (context, xbox[0], ybox[0]);

  // Set the rotation matrix
  double theta = -angle * GIZA_DEG_TO_RAD;
  Sets.fontAngle = theta;
  cairo_matrix_t mat;
  cairo_get_font_matrix (context, &mat);
  cairo_matrix_rotate (&mat, theta);
  cairo_set_font_matrix (context, &mat);

  _giza_parse_string (text, xbox, ybox, _giza_action_print);

  cairo_restore (context);

  _giza_stroke ();

  if (!Sets.buf)
    {
      giza_flush_device ();
    }

  // restore the original character height (and font matrix)
  giza_set_character_height (ch);
}

/**
 * Text: giza_ptext_float
 *
 * Synopsis: Same functionality as giza_ptext but uses floats.
 *
 * See Also: giza_ptext
 */
void
giza_ptext_float (float x, float y, float angle, float just, char *text)
{
  giza_ptext ((double) x, (double) y, (double) angle, (double) just, text);
}

/**
 * Text: giza_text
 *
 * Synopsis: Draws text at the position (x, y).
 *
 * Input:
 *  -x    :- The x-coordinate of the bottom left corner of the text.
 *  -y    :- The y-coordinate of the bottom left corner of the text.
 *  -text :- The text to be drawn.
 */
void
giza_text (double x, double y, char *text)
{
  giza_ptext (x, y, 0.0, 0.0, text);
}

/**
 * Text: giza_text_float
 *
 * Synopsis: Same functionality as giza_text but takes floats.
 *
 * See Also: giza_text
 */
void
giza_text_float (float x, float y, char *text)
{
  giza_ptext ((double) x, (double) y, 0.0, 0.0, text);
}
