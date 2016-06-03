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

#include <giza.h>
#include "giza-warnings-private.h"

static int _giza_warnings = 1;

/**
 * Settings: giza_start_warnings
 *
 * Synopsis: Warnings will be printed to stderr.
 *
 * See Also: giza_stop_warnings
 */
void
giza_start_warnings (void)
{
  _giza_warnings = 1;
}

/**
 * Settings: giza_stop_warnings
 *
 * Synopsis: Warnings will not be printed to stderr.
 *
 * See Also: giza_start_warnings
 */
void
giza_stop_warnings (void)
{
  _giza_warnings = 0;
}

/**
 * Returns 0 if warnings are on, returns 0 if warnings are off
 */
int
_giza_get_warnings (void)
{
  return _giza_warnings;
}
