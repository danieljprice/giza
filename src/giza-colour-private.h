/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 5a) of the GPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
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
void _giza_init_colour_table (void);
void _giza_free_colour_table (void);
void _giza_hls_to_rgb (double hue, double lightness, double saturation,
                       double *red, double *green, double *blue);

extern int _giza_colour_index_min;
extern int _giza_colour_index_max;
