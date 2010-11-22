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

/**
 * Drawing: giza_rectangle
 *
 * Synopsis: Draws a rectangle with corners ((x1, y1) (x2, y1) (x2, y2) (x1, y2)), using the current fill
 * set by giza_set_fill.
 *
 * Inputs:
 *   -x1  :- the x-coordinate of two of the points
 *   -x2  :- the x-coordinate of the other two points
 *   -y1  :- the y-coordinate of two of the points
 *   -y2  :- the y-coordinate of the other two points
 *
 * See Also: giza_set_fill, giza_polygon
 */
void
giza_rectangle (double x1, double x2, double y1, double y2)
{
  int oldTrans = _giza_get_trans ();
  double width, height;

  _giza_set_trans (GIZA_TRANS_WORLD);

  cairo_move_to (context, x1, y1);
  cairo_line_to (context, x2, y1);
  cairo_line_to (context, x2, y2);
  cairo_line_to (context, x1, y2);
  cairo_line_to (context, x1, y1);

  _giza_fill ();

  _giza_set_trans (oldTrans);

  if (!Sets.buf) giza_flush_device ();
}

/**
 * Drawing: giza_rectangle_float
 *
 * Synopsis: Same functionality as giza_rectangle but takes floats
 *
 * See Also: giza_rectangle
 */
void
giza_rectangle_float (float x1, float x2, float y1, float y2)
{
  giza_rectangle ((double) x1, (double) x2, (double) y1, (double) y2);
}
