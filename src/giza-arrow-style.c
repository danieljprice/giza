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
#include "giza-arrow-style-private.h"
#include "giza-io-private.h"
#include <giza.h>

#define GIZA_DEFAULT_FILL    1
#define GIZA_DEFAULT_ANGLE   45.0
#define GIZA_DEFAULT_CUTBACK 0.3

/**
 * Settings: giza_set_arrow_style
 *
 * Synopsis: Sets the style of arrow head to be used for arrows drawn with giza_arrow.
 *
 * Input:
 *  -fillStyle :- Sets the fill style. See giza_set_fill
 *  -angle     :- Sets the acute angle of the arrow head. Can
 *                range from 0.0 to 90.
 *  -cutback   :- The fraction of the back of the arrow head that is cut back.
 *                0.0 gives a triangular arrow head, 1.0 gives >.
 */
void
giza_set_arrow_style (int fillStyle, double angle, double cutback)
{
  if (!_giza_check_device_ready ("giza_set_arrow_style"))
    return;

  if (angle < 0. || angle > 90.)
    {
      _giza_warning ("giza_set_arrow_style", "Invalid angle, angle set to default");
      angle = GIZA_DEFAULT_ANGLE;
    }

  if (cutback < 0. || cutback > 1.)
    {
      _giza_warning ("giza_set_arrow_style", "Invalid cutback, cutback set to default");
      cutback = GIZA_DEFAULT_CUTBACK;
    }

  Arrow.fs = fillStyle;
  Arrow.angle = angle;
  Arrow.cutback = cutback;
}

/**
 * Settings: giza_set_arrow_style_float
 *
 * Synopsis: Same functionality as giza_set_arrow_style but takes floats instead of
 * doubles.
 *
 * See Also: giza_set_arrow_style
 */
void
giza_set_arrow_style_float (int fillStyle, float angle, float cutback)
{
  giza_set_arrow_style (fillStyle, (double) angle, (double) cutback);
}

/**
 * Settings: giza_get_arrow_style
 *
 * Synopsis: Queries the current arrow style settings, as set by giza_set_arrow_style.
 *
 * Input:
 *   -fillStyle :- Gets set to the current fillstyle.
 *   -angle     :- Gets set to the current angle.
 *   -cutback   :- Gets set to the current cutback.
 *
 * See Also: giza_set_arrow_style
 */
void
giza_get_arrow_style (int *fillStyle, double *angle, double *cutback)
{
  if (!_giza_check_device_ready ("giza_get_arrow_style"))
    return;

  *fillStyle = Arrow.fs;
  *angle     = Arrow.angle;
  *cutback   = Arrow.cutback;
}

/**
 * Settings: giza_get_arrow_style_float
 *
 * Synopsis: Same functionality as giza_get_arrow_style, but takes floats instead of doubles.
 *
 * See Also: giza_get_arrow_style
 */
void
giza_get_arrow_style_float (int *fillStyle, float *angle, float *cutback)
{
  double dangle, dcutback;
  giza_get_arrow_style (fillStyle, &dangle, &dcutback);
  *angle   = (float) dangle;
  *cutback = (float) dcutback;
}


/**
 * Sets the arrow style to defaults.
 */
void
_giza_init_arrow_style ()
{
  Arrow.fs = GIZA_DEFAULT_FILL;
  Arrow.angle = GIZA_DEFAULT_ANGLE;
  Arrow.cutback = GIZA_DEFAULT_CUTBACK;
}
