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
 * Drawing: giza_draw_background
 *
 * Synopsis: Redraws the background of the currently open device (erase)
 *
 */
void
giza_draw_background (void)
{
  if (!_giza_check_device_ready ("giza_draw_background"))
    return;

  int oldCi;
  giza_get_colour_index (&oldCi);

  cairo_save (Dev[id].context);
  cairo_reset_clip (Dev[id].context);
  giza_set_colour_index (0);
  /*
   * Only paint the background colour if it is not transparent
   * This is to avoid bounding box issues
   * (i.e., eps is tightly cropped)
   */
  double r,g,b,a;
  giza_get_colour_representation_alpha(0,&r,&g,&b,&a);
  if (a > 0.) {
     cairo_paint (Dev[id].context);
  }
  cairo_restore (Dev[id].context);

  giza_set_colour_index (oldCi);
  return;
}
