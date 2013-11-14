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
#include "giza-io-private.h"
#include <giza.h>
#define GIZA_SMALLEST_LW 1.e-6

/**
 * Settings: giza_set_line_width
 *
 * Synopsis: Sets the line width for all subsequent drawing.
 *
 * Input:
 *  -lw :- The new line width. 1 is 1/4 of a millimetre.
 */
void
giza_set_line_width (double lw)
{
  if (!_giza_check_device_ready ("giza_set_line_width"))
    {
      return;
    }

  if (lw <= GIZA_SMALLEST_LW) { lw = GIZA_SMALLEST_LW; }

  Dev[id].lw = lw;

  /* adjust the line width so it is uniform across devices and 1 is a good size */
  double lwDevice = lw * Dev[id].deviceUnitsPermm * 0.25;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans(GIZA_TRANS_IDEN);
  cairo_set_line_width (Dev[id].context, lwDevice);
  _giza_set_trans (oldTrans);
}

/**
 * Settings: giza_set_line_width_float
 *
 * Synopsis: Same functionality as giza_set_line_width but uses floats
 *
 * See Also: giza_set_line_width
 */
void
giza_set_line_width_float (float lw)
{
  giza_set_line_width ((double) lw);
}

/**
 * Settings: giza_get_line_width
 *
 * Synopsis: Queries the current line width.
 *
 * Input:
 *  -lw :- gets set to the current line width.
 */
void
giza_get_line_width (double *lw)
{
  if (!_giza_check_device_ready ("giza_get_line_width")) {
     *lw = 1;
     return;
  }
  *lw = Dev[id].lw;
}

/**
 * Settings giza_get_line_width_float
 *
 * Synopsis: Same functionality as giza_get_line_width but uses a float.
 *
 * See Also: giza_get_line_width
 */
void
giza_get_line_width_float (float *lw)
{
  if (!_giza_check_device_ready ("giza_get_line_width_float"))
    return;

  double dlw;
  giza_get_line_width(&dlw);
  *lw = (float) dlw;
}
