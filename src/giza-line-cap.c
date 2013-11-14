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
#include <giza.h>

/**
 * Settings: giza_set_line_cap
 *
 * Synopsis: Sets the line cap.
 *
 * Input:
 *  -lc :- An integer representing the line cap.
 *
 * Options:
 *    -0 :- butt
 *    -1 :- round
 *    -2 :- square
 */
void
giza_set_line_cap (int lc)
{
  if(!_giza_check_device_ready("giza_set_line_cap")) return;

  switch (lc)
    {
    case 0:
      cairo_set_line_cap (Dev[id].context, CAIRO_LINE_CAP_BUTT);
      break;
    case 1:
      cairo_set_line_cap (Dev[id].context, CAIRO_LINE_CAP_ROUND);
      break;
    case 2:
      cairo_set_line_cap (Dev[id].context, CAIRO_LINE_CAP_SQUARE);
      break;
    default:
      _giza_warning ("giza_set_line_cap", "Invalid line cap, setting to default.");
      cairo_set_line_cap (Dev[id].context, CAIRO_LINE_CAP_BUTT);
      break;
    }
}

/**
 * Settings: giza_get_line_cap
 *
 * Synopsis: Gets the current line cap, as set by giza_set_line_cap.
 *
 * Input:
 *  -lc :- gets set to the current line cap.
 */
void
giza_get_line_cap (int *lc)
{
  if(!_giza_check_device_ready("giza_get_line_cap")) return;

  switch (cairo_get_line_cap (Dev[id].context))
    {
    case CAIRO_LINE_CAP_BUTT:
      *lc = 0;
      break;
    case CAIRO_LINE_CAP_ROUND:
      *lc = 1;
      break;
    case CAIRO_LINE_CAP_SQUARE:
      *lc = 2;
      break;
    default:
      _giza_warning ("giza_get_line_cap", "Unknown line cap, returning default.");
      *lc = 0;
      break;
    }
}
