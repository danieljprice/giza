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
 * Settings: giza_start_prompting
 *
 * Synopsis: Turns on prompting for current device, i.e. the user will be 
 * prompted before a page change or a device being closed.
 *
 * See Also: giza_stop_prompting
 */
void
giza_start_prompting (void)
{
  if (!_giza_check_device_ready ("giza_start_prompting"))
    return;

  Dev[id].prompting = 1;
}

/**
 * Settings: giza_stop_prompting
 *
 * Synopsis: Turns off prompting, i.e. the user will not be prompted
 * before a page change or a device being closed.
 *
 * See Also: giza_start_prompting
 */
void
giza_stop_prompting (void)
{
  if (!_giza_check_device_ready ("giza_stop_prompting"))
    return;

  Dev[id].prompting = 0;
}

int
_giza_get_prompting (void)
{
  if (!_giza_check_device_ready ("giza_set_prompting"))
    return 0;

  return Dev[id].prompting;
}
