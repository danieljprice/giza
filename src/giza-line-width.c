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
#include "giza-io-private.h"
#include <giza.h>
#define GIZA_SMALLEST_LW 1.e-6

double _giza_lw;


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

  if (lw <= GIZA_SMALLEST_LW)
    {
      _giza_warning ("giza_set_line_width", "Invalid line width, line width not set ");
      return;
    }

  _giza_lw = lw;

  /* adjust the line width so it is uniform across devices and 1 is a good size */
  double lwDevice = lw * Dev.deviceUnitsPermm * 0.25;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans(GIZA_TRANS_IDEN);
  cairo_set_line_width (context, lwDevice);
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
  if (!_giza_check_device_ready ("giza_get_line_width"))
    return;
  *lw = _giza_lw;
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
  *lw = (float) _giza_lw;
}
