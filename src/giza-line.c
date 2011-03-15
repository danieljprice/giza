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
#include "giza-stroke-private.h"
#include "giza-io-private.h"
#include <giza.h>

/**
 * Drawing: giza_line
 *
 * Synopsis: Plots a line made up of n-1 straight segments.
 *
 * Input:
 *  -n    :- The number of points that define the line.
 *           The line will be made up of n - 1 straight segments.
 *           If n is less than 2 nothing is done.
 *  -xpts :- The world x-coordinates of the points to be joined.
 *  -ypts :- The world y-coordinates of the points to be joined.
 *
 * The arrays xpts and ypts must have at least n elements.
 */
void
giza_line (int n, double *xpts, double *ypts)
{
  if (!_giza_check_device_ready ("giza_line"))
    return;

  /* check the line has at least one segment */
  if (n < 2)
    {
      _giza_warning ("giza_line",
		    "The line has less than two segments and will not be plotted.");
      return;
    }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_move_to (context, xpts[0], ypts[0]);

  int i;
  for (i = 1; i < n; i++)
    {
      cairo_line_to (context, xpts[i], ypts[i]);
    }

  _giza_stroke ();

  _giza_set_trans (oldTrans);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}

/**
 * Drawing: giza_line_float
 *
 * Synopsis: The same functionality as giza_line, except it uses
 * arrays of floats
 *
 * See Also: giza_line
 */
void
giza_line_float (int n, float *xpts, float *ypts)
{
  if (!_giza_check_device_ready ("giza_line_float"))
    return;

  if (n < 2)
    {
      _giza_warning ("giza_line_float",
		    "The line has less than two segments and will not be plotted.");
      return;
    }
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_move_to (context, (double) xpts[0], (double) ypts[0]);

  int i;
  for (i = 1; i < n; i++)
    {
      cairo_line_to (context, (double) xpts[i], (double) ypts[i]);
    }

  _giza_stroke ();
  _giza_set_trans (oldTrans);
  if (!Sets.buf)
    {
      giza_flush_device ();
    }
}
