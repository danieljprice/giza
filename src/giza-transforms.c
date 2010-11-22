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
#include "giza-window-private.h"

/**
 * Sets the transform currently applied to the cairo context.
 *
 * Input:
 *  -trans :- the integer representation of the transform to be applied.
 */
void
_giza_set_trans (int trans)
{
  if (!_giza_check_device_ready ("_giza_set_trans"))
    return;

  cairo_matrix_t mat;

  switch (trans)
    {
    case GIZA_TRANS_IDEN:
      cairo_identity_matrix (context);
      break;
    case GIZA_TRANS_NORM:
      cairo_set_matrix (context, &(Win.normCoords));
      break;
    case GIZA_TRANS_WORLD:
      cairo_set_matrix (context, &(Win.userCoords));
      break;
    case GIZA_TRANS_FONT:
      cairo_get_font_matrix (context, &mat);
      cairo_set_matrix (context, &mat);
      break;
    default:
      _giza_error ("_giza_set_trans", "Internal Error: Invalid Transformation.");
      return;
      break;
    }
  Dev.CurrentTrans = trans;
}

/**
 * Returns the transform of the transform currently applied to the
 * cairo context.
 *
 * Return:
 *  -returns the integer value of the current transform.
 */
int
_giza_get_trans ()
{
  if (!_giza_check_device_ready ("_giza_get_trans"))
    return -1;

  return Dev.CurrentTrans;
}
