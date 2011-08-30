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

#define GIZA_CTAB_SAVE_MAX 10
#define GIZA_CTAB_MAXSIZE 256

typedef struct
{
  int n;
  double controlPoints[GIZA_CTAB_MAXSIZE];
  double red[GIZA_CTAB_MAXSIZE];
  double green[GIZA_CTAB_MAXSIZE];
  double blue[GIZA_CTAB_MAXSIZE];
} giza_ctab;

void _giza_set_range_from_colour_table (int cimin, int cimax);
void _giza_init_colour_index (void);
void _giza_init_colour_index_pgplot (void);
void _giza_init_colour_table (void);
void _giza_free_colour_table (void);
void _giza_hls_to_rgb (double hue, double lightness, double saturation, 
                       double *red, double *green, double *blue);

int _giza_colour_index_min;
int _giza_colour_index_max;
