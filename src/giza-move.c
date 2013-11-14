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
  cairo_move_to (Dev[id].context, xpt, ypt);
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

/**
 * Drawing: giza_get_current_point
 *
 * Synopsis: Query current pen position
 *
 * Output:
 *  -xpt :- The world x-coordinates of the point
 *  -ypt :- The world y-coordinates of the point
 *
 * See Also: giza_get_current_point_float
 */
void
giza_get_current_point (double *xpt, double *ypt)
{
  if (!_giza_check_device_ready ("giza_get_current_point"))
    return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_get_current_point (Dev[id].context, xpt, ypt);
  _giza_set_trans (oldTrans);
}

/**
 * Drawing: giza_get_current_point_float
 *
 * Synopsis: The same functionality as giza_get_current_point, but uses floats
 *
 * See Also: giza_get_current_point
 */
void
giza_get_current_point_float (float *xpt, float *ypt)
{
  if (!_giza_check_device_ready ("giza_move"))
    return;

  double xptd, yptd;
  giza_get_current_point(&xptd, &yptd);
  *xpt = (float) xptd;
  *ypt = (float) yptd;
}
