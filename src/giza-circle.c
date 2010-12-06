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
#include "giza-transforms-private.h"
#include "giza-fill-private.h"
#include <giza.h>
#include <math.h>

/**
 * Drawing: giza_circle
 *
 * Synopsis: Draws a circle at x, y with radius r (in world coords), using the current fill set by giza_set_fill.
 *
 * Input:
 *  -x    :- the world x-coord of the centre of the circle
 *  -y    :- the world y-coord of the centre of the circle
 *  -r    :- the radius of the circle in world coords
 */
void
giza_circle (double x, double y, double r)
{
  int oldTrans = _giza_get_trans ();

  _giza_set_trans (GIZA_TRANS_WORLD);

  cairo_arc (context, x, y, r, 0., 2 * M_PI);

  _giza_fill ();

  _giza_set_trans (oldTrans);
  
  if (!Sets.buf) giza_flush_device ();
}

/**
 * Drawing: giza_circle_float 
 *
 * Synopsis: Same functionality as giza_circle, but takes floats
 *
 * See Also: giza_circle
 */
void
giza_circle_float (float x, float y, float r)
{
  giza_circle ((double) x, (double) y, (double) r);

  if (!Sets.buf) giza_flush_device ();
}
