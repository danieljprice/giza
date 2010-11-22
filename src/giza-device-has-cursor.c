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
 * D.Price (5/3/10): changed function name to device_has_cursor
 */

#include "giza-private.h"

/**
 * Device: giza_device_has_cursor
 *
 * Synopsis: Query the interactivity of the device.
 *
 * Return:
 *   -0 :- If the device is not interactive.
 *   -1 :- If the device is interactive.
 */
int
giza_device_has_cursor ()
{
  return Dev.isInteractive;
}
