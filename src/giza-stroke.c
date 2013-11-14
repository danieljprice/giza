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

#include "giza-transforms-private.h"
#include "giza-private.h"
#include "giza-line-style-private.h"
#include "giza-stroke-private.h"
#include <giza.h>

/**
 * An internal function to ensure line width
 * is are honoured for all strokes.
 */
void
_giza_stroke (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  /* set the line style in case there has been a change in character
     height or line width */
  int ls;
  giza_get_line_style (&ls);
  giza_set_line_style (ls);

  cairo_stroke (Dev[id].context);
  _giza_set_trans (oldTrans);
}
