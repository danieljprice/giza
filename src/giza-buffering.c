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
 * Settings: giza_begin_buffer
 *
 * Synopsis: Begins buffering
 *
 * See Also: giza_end_buffer, giza_flush_buffer
 */
void
giza_begin_buffer (void)
{
  if (!_giza_check_device_ready ("giza_begin_buffer")) return;
  Dev[id].buf = 1;
}

/**
 * Settings: giza_end_buffer
 *
 * Synopsis: Ends buffering
 *
 * See Also: giza_begin_buffer, giza_flush_buffer
 */
void
giza_end_buffer (void)
{
  if(!_giza_check_device_ready ("giza_end_buffer")) return;
  Dev[id].buf = 0;
  giza_flush_device ();
}

/**
 * Settings: giza_get_buffering
 *
 * Synopsis: returns whether output is currently buffered on current device
 *
 * See Also: giza_begin_buffer, giza_flush_buffer, giza_end_buffer
 */
void
giza_get_buffering (int *buf)
{
  *buf = 0;
  if(!_giza_check_device_ready ("giza_end_buffer")) return;
  *buf = Dev[id].buf;
}
/**
 * Settings: giza_flush_buffer
 *
 * Synopsis: Updates graphics display
 *           Can be used to flush the graphics buffer
 *           manually between calls to giza_begin_buffer
 *           and giza_end_buffer
 *
 * See Also: giza_begin_buffer, giza_end_buffer
 */
void
giza_flush_buffer (void)
{
  if(!_giza_check_device_ready ("giza_flush_buffer")) return;
  giza_end_buffer();
  giza_begin_buffer();
}
