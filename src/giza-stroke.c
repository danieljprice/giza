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

#include "giza-transforms-private.h"
#include "giza-private.h"
#include "giza-line-style-private.h"
#include <giza.h>

/**
 * An internal function to ensure line width
 * is are honoured for all strokes.
 */
void
_giza_stroke ()
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  // set the line style incase there has been a change in character 
  // height/line width
  int ls;
  giza_get_line_style (&ls);
  giza_set_line_style (ls);

  cairo_stroke (context);
  _giza_set_trans (oldTrans);
}
