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
 * Copyright (C) 2010-2011 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include "giza-colour-private.h"
#include "giza-private.h"
#include <giza.h>

/**
 * Settings: giza_set_colour_palette
 *
 * Synopsis: Choose between various preset colour "palettes" for the first 16
 *           colour indices commonly used for point and line drawing.
 *           This is equivalent to using giza_set_colour_representation 
 *           for each index in turn.
 *
 * Note: 
 *   giza_render is not affected by this setting
 *
 * Input:
 *  -palette  :- choice of colour palette
 *
 * Available palette options:
 *  -0 or GIZA_COLOUR_PALETTE_DEFAULT :- default giza palette
 *  -1 or GIZA_COLOUR_PALETTE_PGPLOT  :- default PGPLOT palette
 *
 * See Also: giza_set_colour_index, giza_set_colour_table
 *
 */
void 
giza_set_colour_palette (int palette)
{
  int nlinecolours;

  switch(palette)
  {
    case GIZA_COLOUR_PALETTE_PGPLOT:
    /* Installs the standard PGPLOT colour table */
       giza_set_colour_representation (0,1.0,1.0,1.0);	/* white */
       giza_set_colour_representation (1,0.0,0.0,0.0);	/* black */
       giza_set_colour_representation (2,1.0,0.0,0.0);
       giza_set_colour_representation (3,0.0,1.0,0.0);
       giza_set_colour_representation (4,0.0,0.0,1.0);
       giza_set_colour_representation (5,0.0,1.0,1.0);
       giza_set_colour_representation (6,1.0,0.0,1.0);
       giza_set_colour_representation (7,1.0,1.0,0.0);
       giza_set_colour_representation (8,1.0,0.5,0.0);
       giza_set_colour_representation (9,0.5,1.0,0.0);
       giza_set_colour_representation (10,0.0,1.0,0.5);
       giza_set_colour_representation (11,0.0,0.5,1.0);
       giza_set_colour_representation (12,0.5,0.0,1.0);
       giza_set_colour_representation (13,1.0,0.0,0.5);
       giza_set_colour_representation (14,0.333,0.333,0.333);
       giza_set_colour_representation (15,0.667,0.667,0.667);
       nlinecolours = 16;
       break;
    default:
       giza_set_colour_representation (0, 1., 1., 1.);	/* white */
       giza_set_colour_representation (1, 0., 0., 0.);	/* black */
       /* red */
       giza_set_colour_representation (2, .933, 0.173, 0.173);
       /* green */
       giza_set_colour_representation (3, 0.18, 0.545, 0.341);
       /* blue */
       giza_set_colour_representation (4, 0., 0., 1.);
       /* Dark slate gray */
       giza_set_colour_representation (5, 0.192, 0.31, 0.31);
       /* Dark violet */
       giza_set_colour_representation (6, 0.58, 0., 0.827);
       /* cyan */
       giza_set_colour_representation (7, 0.0, 0.825, 0.825);
       /* Indian red */
       /*giza_set_colour_representation (7, 0.804, 0.361, 0.361);  */
       /* Gold */
       /*giza_set_colour_representation (7, 1., 0.843, 0.); */
       giza_set_colour_representation (8, 1., 0.31, 0.);
       /* cadet blue */
       giza_set_colour_representation (9, 0.373, 0.62, 0.627);
       /* peach puff */
       giza_set_colour_representation (10, 0.933, 0.796, 0.678);
       giza_set_colour_representation (11, 0.0, 1.0, 0.5);
       giza_set_colour_representation (12, 0.0, 0.5, 1.0);
       giza_set_colour_representation (13, 0.5, 0.0, 0.0);
       giza_set_colour_representation (14, 1.0, 0.0, 0.5);
       giza_set_colour_representation (15, 0.333, 0.333, 0.333);
       giza_set_colour_representation (16, 0.667, 0.667, 0.667);
       nlinecolours = 16;
       break;
  }
  /*
   * allow the rest of the colour indices to be
   * set by giza_set_colour_table
   * (this range can be changed by
   *  calling giza_set_colour_index_range)
   */
  giza_set_colour_index_range(nlinecolours,GIZA_COLOUR_INDEX_MAX);

}

