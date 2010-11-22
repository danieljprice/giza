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
#include "giza-io-private.h"
#include "giza-stroke-private.h"
#include "giza-transforms-private.h"

#define GIZA_FILL_SOLID  1
#define GIZA_FILL_HOLLOW 2


int _giza_fill_style;

/**
 * Settings: giza_set_fill
 *
 * Synopsis: Sets the current fill style.
 *
 * Input:
 *  -fs :- the new fill style
 *
 * Fill Styles:
 *  -1 :- solid
 *  -2 :- hollow
 */
void
giza_set_fill (int fs)
{
  if (!_giza_check_device_ready ("giza_set_fill"))
    return;
  if (fs < 1 || fs > 2)
    {
      _giza_warning ("giza_set_fill", "Invalid fill style, fill style not set");
      return;
    }

  _giza_fill_style = fs;
}

/**
 * Settings: giza_get_fill
 *
 * Synopsis: Query the current fill style.
 *
 * Input:
 *  -fs :- gets set to the current fill style.
 */
void
giza_get_fill (int *fs)
{
  if (!_giza_check_device_ready ("giza_get_fill"))
    return;

  *fs = _giza_fill_style;
}

void
_giza_init_fill ()
{
  _giza_fill_style = 1;
}

/**
 * Fills the currently drawn path.
 */
void
_giza_fill ()
{
  if(!_giza_check_device_ready ("_giza_fill"))
     return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  switch (_giza_fill_style)
    {
    case GIZA_FILL_SOLID:
      cairo_fill (context);
      break;
    case GIZA_FILL_HOLLOW:
      _giza_stroke ();
      break;
    default:
      _giza_warning ("_giza_fill", "Invalid fill style");
      _giza_stroke ();
      break;
    }

  _giza_set_trans (oldTrans);
}
