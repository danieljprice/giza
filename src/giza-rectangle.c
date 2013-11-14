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
#include "giza-transforms-private.h"
#include "giza-fill-private.h"
#include <giza.h>
#include <math.h>

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
  if (!_giza_check_device_ready ("giza_rectangle"))
    return;

  int oldTrans = _giza_get_trans ();

  _giza_set_trans (GIZA_TRANS_WORLD);

  cairo_move_to (Dev[id].context, x1, y1);
  cairo_line_to (Dev[id].context, x2, y1);
  cairo_line_to (Dev[id].context, x2, y2);
  cairo_line_to (Dev[id].context, x1, y2);
  cairo_line_to (Dev[id].context, x1, y1);

  _giza_fill ();

  _giza_set_trans (oldTrans);

  giza_flush_device ();
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

/**
 * Drawing: giza_rectangle_rounded
 *
 * Synopsis: Draws a rectangle with rounded corners ((x1, y1) (x2, y1) (x2, y2) (x1, y2)), 
 *  using the current fill set by giza_set_fill.
 *
 * Inputs:
 *   -x1  :- the x-coordinate of two of the points
 *   -x2  :- the x-coordinate of the other two points
 *   -y1  :- the y-coordinate of two of the points
 *   -y2  :- the y-coordinate of the other two points
 *   -radius :- radius of curvature for the corners
 *
 * See Also: giza_rectangle, giza_set_fill, giza_polygon
 */
void
giza_rectangle_rounded (double x1, double x2, double y1, double y2, double radius)
{
  if (!_giza_check_device_ready ("giza_rectangle_rounded"))
    return;
  int oldTrans = _giza_get_trans ();

  _giza_set_trans (GIZA_TRANS_WORLD);

  double width = x2 - x1;
  double height = y2 - y1;
  double x = x1;
  double y = y1;
   
  if (radius > 0.5*height) radius = 0.5*height;
  if (radius > 0.5*width) radius = 0.5*width;
  
  /* Adapted from: http://www.mono-project.com/Mono.Cairo_Cookbook */
  cairo_move_to (Dev[id].context, x, y + radius);
  cairo_arc (Dev[id].context,x + radius, y + radius, radius, M_PI, -0.5*M_PI);
  cairo_line_to (Dev[id].context,x + width - radius, y);
  cairo_arc (Dev[id].context,x + width - radius, y + radius, radius, -0.5*M_PI, 0.);
  cairo_line_to	(Dev[id].context,x + width, y + height - radius);
  cairo_arc (Dev[id].context,x + width - radius, y + height - radius, radius, 0., 0.5*M_PI);
  cairo_line_to (Dev[id].context,x + radius, y + height);
  cairo_arc (Dev[id].context,x + radius, y + height - radius, radius, 0.5*M_PI, M_PI);
  cairo_close_path (Dev[id].context);

  _giza_fill ();

  _giza_set_trans (oldTrans);

  giza_flush_device ();
}

/**
 * Drawing: giza_rectangle_rounded_float
 *
 * Synopsis: Same functionality as giza_rectangle_rounded but takes floats
 *
 * See Also: giza_rectangle_rounded
 */
void
giza_rectangle_rounded_float (float x1, float x2, float y1, float y2, float radius)
{
  giza_rectangle_rounded ((double) x1, (double) x2, (double) y1, (double) y2, (double) radius);
}
