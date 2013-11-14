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
#include "giza-io-private.h"
#include "giza-stroke-private.h"
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
giza_polygon (int n, const double *xpts, const double *ypts)
{
  if (!_giza_check_device_ready ("giza_polygon"))
    return;

  /* check the line has at least one segment */
  if (n < 1) return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_move_to (Dev[id].context, xpts[0], ypts[0]);

  int i;
  for (i = 1; i < n; i++)
    {
      cairo_line_to(Dev[id].context,xpts[i],ypts[i]);
    }

  cairo_close_path(Dev[id].context);
  _giza_fill ();
  _giza_stroke ();
  giza_flush_device ();
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
giza_polygon_float (int n, const float *xpts, const float *ypts)
{
  if (!_giza_check_device_ready ("giza_polygon"))
    return;

  /* check the line has at least one segment */
  if (n < 1) return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_move_to (Dev[id].context, (double) xpts[n-1],(double) ypts[n-1]);

  int i;
  for (i = 0; i < n; i++)
    {
      cairo_line_to(Dev[id].context,(double) xpts[i],(double) ypts[i]);
    }

  cairo_close_path(Dev[id].context);
  _giza_fill ();
  _giza_stroke ();
  giza_flush_device ();
  _giza_set_trans (oldTrans);
}
