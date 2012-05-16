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
#include "giza-io-private.h"
#include "giza-band-private.h"
#include <giza.h>

/**
 * Settings: giza_set_band_style
 *
 * Synopsis: Sets the line style to be used by giza_band
 *
 * Input:
 *  -ls :- the line style for the band
 *  -lw :- the width for the line
 *
 * See Also: giza_band, giza_get_band_style
 */
void
giza_set_band_style (int ls, double lw)
{
  Band.ls = ls;
  Band.lw = lw;
}

/**
 * Settings: giza_get_band_style
 *
 * Synopsis: Queries the current band style settings to be used by giza_band
 *
 * Output:
 *  -ls :- the line style for the band
 *  -lw :- the width for the line
 *
 * See Also: giza_band, giza_set_band_style
 */
void
giza_get_band_style (int *ls, double *lw)
{
  *ls = Band.ls;
  *lw = Band.lw;
}

/**
 * Initalises the default style for band
 */
void
_giza_init_band_style (void)
{
  Band.ls = 1;
  Band.lw = 0.9;
}

