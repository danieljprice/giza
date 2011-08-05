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
 
#include <giza.h>
#include "giza-warnings-private.h"

int _giza_warnings = 1;

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
