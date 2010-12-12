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
#include <giza.h>

/**
 * Settings: giza_begin_buffer
 *
 * Synopsis: Begins buffering
 *
 * See Also: giza_end_buffer
 */
void
giza_begin_buffer (void)
{
  if (!_giza_check_device_ready ("giza_begin_buffer")) return;
  Sets.buf = 1;
}

/**
 * Settings: giza_end_buffer
 *
 * Synopsis: Ends buffering
 *
 * See Also: giza_begin_buffer
 */
void
giza_end_buffer (void)
{
  if(!_giza_check_device_ready ("giza_end_buffer")) return;
  giza_flush_device ();
  Sets.buf = 0;
}
