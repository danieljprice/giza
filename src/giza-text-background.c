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
#include "giza-text-background-private.h"

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
  _giza_text_background = colourIndex;
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
  *colourIndex = _giza_text_background;
}
