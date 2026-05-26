/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2022 Daniel Price
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
#include "giza-drivers-private.h"
#include "giza-driver-xw-private.h"
#include <giza.h>
#include <stddef.h>

/**
 * Drawing: giza_set_window_title
 *
 * Synopsis: Set the device prefix and, for interactive devices, the
 *           window title shown to the user.
 */
void
giza_set_window_title (const char *title)
{
  if (!_giza_check_device_ready ("giza_set_window_title"))
    return;

  if (title == NULL || title[0] == '\0')
    return;

  _giza_set_prefix (title);

#ifdef _GIZA_HAS_XW
  if (Dev[id].type == GIZA_DEVICE_XW)
    _giza_set_window_title_xw (title);
#endif

  return;
}
