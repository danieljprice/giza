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
#include "giza-fill-private.h"
#include <giza.h>

/**
 * Drawing: giza_polygon
 *
 * Synopsis: Draws a polygon, using the current fill set by giza_set_fill.
 *
 * Input:
 *  -n       :- number of vertices
 *  -xpts    :- x positions of vertices
 *  -ypts    :- y positions of vertices
 *
 * See Also: giza_set_fill
 */
void
giza_polygon (int n, double *xpts, double *ypts)
{
  if (!_giza_check_device_ready ("giza_polygon"))
    return;

  // check the line has at least one segment
  if (n < 1)
    {
      _giza_warning ("giza_polygon",
                     "The polygon has less than one segment and will not be plotted.");
      return;
    }

  int fillStyle, oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_move_to (context, xpts[0], ypts[0]);
 
  int i;
  for (i = 1; i < n; i++)
    {
      cairo_line_to(context,xpts[i],ypts[i]);
    }

  cairo_close_path(context);
  _giza_fill ();
  _giza_stroke ();
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
  _giza_set_trans (oldTrans);

}

/**
 * Drawing: giza_polygon_float
 *
 * Synopsis: Same functionality as giza_polygon, but takes floats
 *
 * See Also: giza_polygon
 */
void
giza_polygon_float (int n, float *xpts, float *ypts)
{
  if (!_giza_check_device_ready ("giza_polygon"))
    return;

  // check the line has at least one segment
  if (n < 1)
    {
      _giza_warning ("giza_polygon",
                     "The polygon has less than one segment and will not be plotted.");
      return;
    }

  int fillStyle, oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_move_to (context, (double) xpts[n-1],(double) ypts[n-1]);
 
  int i;
  for (i = 0; i < n; i++)
    {
      cairo_line_to(context,(double) xpts[i],(double) ypts[i]);
    }

  cairo_close_path(context);
  _giza_fill ();
  _giza_stroke ();
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
  _giza_set_trans (oldTrans);
}
