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
 * Copyright (C) 2010 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include "giza-private.h"
#include "giza-transforms-private.h"
#include "giza-stroke-private.h"
#include "giza-io-private.h"
#include <giza.h>

/**
 * Drawing: giza_move
 *
 * Synopsis: Move current pen position to a point
 *
 * Input:
 *  -xpt :- The world x-coordinates of the point
 *  -ypt :- The world y-coordinates of the point
 *
 */
void
giza_move (double xpt, double ypt)
{
  if (!_giza_check_device_ready ("giza_move"))
    return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_move_to (context, xpt, ypt);
  _giza_set_trans (oldTrans);
}

/**
 * Drawing: giza_move_float
 *
 * Synopsis: The same functionality as giza_move, except it uses floats
 *
 * See Also: giza_move
 */
void
giza_move_float (float xpt, float ypt)
{
  if (!_giza_check_device_ready ("giza_move"))
    return;

  giza_move((double) xpt, (double) ypt);

}