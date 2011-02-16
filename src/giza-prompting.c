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

/*
 * The initial value of _giza_prompting is set by a call
 * either to giza_start_prompting or giza_stop_prompting
 * in giza_open_device
 */
int _giza_prompting;

/**
 * Settings: giza_start_prompting
 *
 * Synopsis: Turns on prompting, i.e. the user will be prompted before
 * a page change or a device being changed.
 *
 * See Also: giza_stop_prompting
 */
void
giza_start_prompting (void)
{
  if (!_giza_check_device_ready ("giza_get_colour_representation"))
    return;
  _giza_prompting = 1;
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
  if (!_giza_check_device_ready ("giza_get_colour_representation"))
    return;
  _giza_prompting = 0;
}

int
_giza_get_prompting (void)
{
  return _giza_prompting;
}
