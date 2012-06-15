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
#include "giza-drivers-private.h"
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

  double xanc[1], yanc[1];
  xanc[0] = 0.;
  yanc[0] = 0.;
  return _giza_get_key_press (0, 0, 1, xanc, yanc, x, y, ch);
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

  double xanc[1], yanc[1];
  xanc[0] = 0.;
  yanc[0] = 0.;
  success = _giza_get_key_press (0, 0, 1, xanc, yanc, &dx, &dy, ch);
  *x = (float) dx;
  *y = (float) dy;

  return success;
}


