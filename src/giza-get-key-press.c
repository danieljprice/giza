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
#include <string.h>
#include <giza.h>

/**
 * Interactive: giza_get_key_press
 *
 * Synopsis: Returns the cursor position and key press after a key press.
 *
 * Input:
 *  -x  :- Gets set to the x world coord of the cursor.
 *  -y  :- Gets set to the y world coord of the cursor.
 *  -ch :- Gets set to the character pressed.
 *
 * Return value:
 *  -0 :- If the call was successful
 *  -1 :- If the device has no cursor
 */
int
giza_get_key_press (double *x, double *y, char *ch)
{
  if (!_giza_check_device_ready ("giza_get_key_press"))
    return 1;

  return _giza_get_key_press (0, 0, 0., 0., x, y, ch);
} 

/**
 * Interactive giza_get_key_press_float
 *
 * Synopsis: Same functionality as giza_get_key_press, but uses floats.
 *
 * See Also: giza_get_key_press
 */
int
giza_get_key_press_float (float *x, float *y,char *ch)
{
  int success;
  double dx;
  double dy;

  success = _giza_get_key_press (0, 0, 0., 0., &dx, &dy, ch);
  *x = (float) dx;
  *y = (float) dy;

  return success;
}


