/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2013 Daniel Price
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
#include "giza-text-background-private.h"
#include "giza-private.h"
#include <giza.h>

/**
 * Settings: giza_set_text_background
 *
 * Synopsis: Set the colour index for the background of any text to be drawn.
 * Use a negative value for a transparent background.
 *
 * Input:
 *  -colourIndex :- The colour index the background will be.
 */
void
giza_set_text_background (int colourIndex)
{
  if (!_giza_check_device_ready ("giza_set_text_background"))
     return;

  Dev[id].text_background = colourIndex;
}

/**
 * Settings: giza_get_text_background
 *
 * Synopsis: Queries the current text background colour
 *
 * Output:
 *  -colourIndex :- Current setting for the background text colour.
 */
void
giza_get_text_background (int *colourIndex)
{
  if (!_giza_check_device_ready ("giza_get_text_background"))
     return;

  *colourIndex = Dev[id].text_background;
}

void
_giza_init_text_background (void)
{
  Dev[id].text_background = -1;
}
