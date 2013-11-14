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
      cairo_identity_matrix (Dev[id].context);
      break;
    case GIZA_TRANS_NORM:
      cairo_set_matrix (Dev[id].context, &(Dev[id].Win.normCoords));
      break;
    case GIZA_TRANS_WORLD:
      cairo_set_matrix (Dev[id].context, &(Dev[id].Win.userCoords));
      break;
    case GIZA_TRANS_FONT:
      cairo_get_font_matrix (Dev[id].context, &mat);
      cairo_set_matrix (Dev[id].context, &mat);
      break;
    default:
      _giza_error ("_giza_set_trans", "Internal Error: Invalid Transformation.");
      return;
      break;
    }
  Dev[id].CurrentTrans = trans;
}

/**
 * Returns the transform of the transform currently applied to the
 * cairo context.
 *
 * Return:
 *  -returns the integer value of the current transform.
 */
int
_giza_get_trans (void)
{
  if (!_giza_check_device_ready ("_giza_get_trans"))
    return -1;

  return Dev[id].CurrentTrans;
}
