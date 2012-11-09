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

#include "giza-colour-private.h"
#include "giza-private.h"
#include <giza.h>
#include <stdio.h>
#include <stdlib.h>

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
 *  -2 to 4 :- various experimental line palettes
 *  -5 :- colourblind safe palette from http://jfly.iam.u-tokyo.ac.jp/color/
 *  -6 :- optimum palette from http://web.media.mit.edu/~wad/color/palette.html
 *  -7 :- graph-a-licious
 * See Also: giza_set_colour_index, giza_set_colour_table
 *
 */
void 
giza_set_colour_palette (int palette)
{
  int nlinecolours;

  char *text = getenv ("GIZA_PALETTE");
  if(text)
    {
      int mypalette;
      if ( sscanf(text, "%i", &mypalette) == 1 )
         {
            palette = mypalette;
         }
    }

  /* set foreground/background colours independently */
  giza_set_colour_representation (0,1.0,1.0,1.0);  /* white */
  giza_set_colour_representation (1,0.0,0.0,0.0);  /* black */

  switch(palette)
  {
    case GIZA_COLOUR_PALETTE_PGPLOT:
    /* Installs the standard PGPLOT colour table */
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
    case 2: /* Terry, with modifications */
       giza_set_colour_representation (2, 1., 0., 0.);   /* red */
       giza_set_colour_representation (3, 0., 0.25, 1.); /* blue */
       giza_set_colour_representation (4, 0.5, 0., 0.5); /* purple */
       giza_set_colour_representation (5, 0.02,0.93,0.93);  /* cyan */
       giza_set_colour_representation (6, 0., 0.5, 0.);  /* green */
       giza_set_colour_representation (7, 0.5625, 0.554688, 0.011719); /* mustard */
       giza_set_colour_representation (8, 0.3,0.2,0.15);  /* brown */
       giza_set_colour_representation (9,1.0,0.5,0.0); /* orange */
       giza_set_colour_representation_rgb (10, 249, 223, 11); /* desert sun */
       nlinecolours = 11;
       break;
    case 3: /* adapted from palettes cheer up emo kid and grandma's pillow on colorlovers.com */
       giza_set_colour_representation_rgb (2, 85, 98, 112); /* mighty slate */
       giza_set_colour_representation_rgb (3, 78, 205, 196); /* pacifica */
       giza_set_colour_representation_rgb (4, 100, 122, 50);  /* dark apple chic */
       giza_set_colour_representation_rgb (5, 255, 107, 107); /* cheery pink */
       giza_set_colour_representation_rgb (6, 196, 77, 88); /* grandma's pillow */
       giza_set_colour_representation_rgb (7, 192, 157, 93); /* regretful */
       giza_set_colour_representation_rgb (8, 239, 169, 133); /* vintage peach */
       giza_set_colour_representation_rgb (9, 221, 133, 239); /* pinkish */
       nlinecolours = 10;
       break;
    case 4: /* adapted from aboriginal and outback from colourlovers.com */
       giza_set_colour_representation_rgb (2, 196, 14, 14); /* vegas */
       giza_set_colour_representation_rgb (3, 32, 133, 133); /* eucalyptus */
       giza_set_colour_representation_rgb (4, 207, 62, 156);  /* parrot */
       giza_set_colour_representation_rgb (5, 247, 74, 0); /* enjoyable orange */
       giza_set_colour_representation_rgb (6, 38, 51, 17); /* cataclysm */
       giza_set_colour_representation_rgb (7, 5, 79, 129); /* wintersky tango */
       giza_set_colour_representation_rgb (8, 249, 223, 11); /* desert sun */
       nlinecolours = 9;
       break;
    case 5: /* Colourblind-safe palette from http://jfly.iam.u-tokyo.ac.jp/color/ */
       giza_set_colour_representation (2, 0.9, 0.6, 0.); /* orange */
       giza_set_colour_representation (3, 0.35, 0.7, 0.9); /* sky blue */
       giza_set_colour_representation (4, 0., 0.6, 0.5);  /* bluish green */
       giza_set_colour_representation (5, 0.95, 0.9, 0.25); /* yellow */
       giza_set_colour_representation (6, 0., 0.45, 0.7); /* blue */
       giza_set_colour_representation (7, 0.8, 0.4, 0.); /* vermilion */
       giza_set_colour_representation (8, 0.8, 0.6, 0.7); /* reddish purple */
       nlinecolours = 9;
       break;
    case 6: /* modified version of http://web.media.mit.edu/~wad/color/palette.html */
       giza_set_colour_representation_rgb(2, 193, 15, 15); /* red */
       giza_set_colour_representation_rgb(3, 42, 75, 215); /* blue */
       giza_set_colour_representation_rgb(4, 29, 105, 20); /* green */
       giza_set_colour_representation_rgb(5, 129, 74, 25); /* brown */
       giza_set_colour_representation_rgb(6, 129, 38, 192); /* purple */
       giza_set_colour_representation_rgb(7, 160, 160, 160); /* light grey */
       giza_set_colour_representation_rgb(8, 129, 197, 122); /* light green */
       giza_set_colour_representation_rgb(9, 157, 175, 255); /* light blue */
       giza_set_colour_representation_rgb(10, 41, 208, 208); /* cyan */
       giza_set_colour_representation_rgb(11, 255, 146, 51); /* orange */
       giza_set_colour_representation_rgb(12, 255, 238, 51); /* yellow */
       giza_set_colour_representation_rgb(13, 233, 222, 187); /* tan */
       giza_set_colour_representation_rgb(14, 255, 205, 243); /* pink */
       giza_set_colour_representation_rgb(15, 87, 87, 87); /* Dark grey */
       nlinecolours = 16;
       break;
    case 7: /* graph-a-licious from colourlovers.com */
       giza_set_colour_representation_rgb (2, 209, 4, 21); /* red red */
       giza_set_colour_representation_rgb (3, 113, 49, 159); /* poiple */
       giza_set_colour_representation_rgb (4, 12, 34, 100);  /* ink11 */
       giza_set_colour_representation_rgb (5, 113, 145, 38); /* flower power FGR */
       giza_set_colour_representation_rgb (6, 255, 207, 38); /* flower power yel */
       /* added by DJP */
       giza_set_colour_representation_rgb (7, 255, 146, 51); /* orange */
       giza_set_colour_representation_rgb (8, 139, 94, 45); /* brown */
       nlinecolours = 9;
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

