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
#include "giza-band-private.h"

/**
 * Settings: giza_set_band_style
 *
 * Synopsis: Sets the line style to be used by giza_band
 *
 * Input:
 *  -ls :- the line style for the band
 *  -lw :- the width for the line
 *
 * See Also: giza_band
 */
void
giza_set_band_style (int ls, double lw)
{
  Band.ls = ls;
  Band.lw = lw;
}

/**
 * Initalises the default style for band
 */
void
_giza_init_band_style (void)
{
  Band.ls = 1;
  Band.lw = 1.;
}

