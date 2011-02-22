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

#include "giza-private.h"
#include <giza.h>
int _giza_clip;

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

  _giza_clip = clip;
  if (!clip) 
    {
      cairo_reset_clip(context);
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

  *clip = _giza_clip;

}
