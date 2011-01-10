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
#include "giza-drivers-private.h"
#include <giza.h>

/**
 * Interactive: giza_band
 *
 * Synopsis: Returns the cursor position and character typed by the user
 * relative to an anchor point
 *
 * Input:
 *  -mode     :- selects the mode to draw the band.
 *  -moveCurs :- if 1 the cursor is moved to (x, y),
 *               if 0 the cursor is not moved.
 *  -xanc     :- the x-coord of the anchor point.
 *  -yanc     :- the y-coord of the anchor point.
 *  -x        :- Gets set the position of the cursor.
 *  -y        :- Gets set to the position of the cursor.
 *  -ch       :- Gets set to the character pressed by the user.
 *
 * Return:
 *  -1 :- The device has no cursor
 *  -0 :- The call was successful
 */
int
giza_band (int mode, int moveCurs, double xanc, double yanc, double *x, double *y, char *ch)
{
  if(!_giza_check_device_ready ("giza_band"))
    return 1;

  return _giza_get_key_press (mode, moveCurs, xanc, yanc, x, y, ch);
}

/**
 * Interactive: giza_band_float
 *
 * Synopsis: Same functionality as giza_band, but uses floats.
 *
 * See Also: giza_band
 */
int
giza_band_float (int mode, int moveCurs, float xanc, float yanc, float *x, float *y, char *ch)
{
  int success;
  double dx = (double) *x;
  double dy = (double) *y;

  success = giza_band (mode, moveCurs, (double) xanc, (double) yanc, &dx, &dy, ch);
  *x = (float) dx;
  *y = (float) dy;

  return success;
}


