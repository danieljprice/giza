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
#include <giza.h>

/**
 * Drawing: giza_clip
 *
 * Synopsis: Set whether or not to clip at the edge of the viewport
 *
 * Input:
 *  -clip :- Use 0 to disable clipping, 1 to enable clipping
 *
 */
void
giza_set_clipping (int clip)
{
  if (!_giza_check_device_ready ("giza_set_clipping"))
    return;

  Dev[id].clip = clip;
  if (!clip)
    {
      cairo_reset_clip(Dev[id].context);
    }
}

/**
 * Drawing: giza_get_clipping
 *
 * Synopsis: Query whether or not clipping at edge of viewport is enabled or disabled
 *
 * See Also: giza_move
 */
void
giza_get_clipping (int *clip)
{
  if (!_giza_check_device_ready ("giza_get_clipping"))
    return;

  *clip = Dev[id].clip;

}
