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

typedef struct
{
  cairo_surface_t *onscreen;
  cairo_t *box;
  cairo_t *restore;
  double lw;
  int ls;
  int maxWidth;
  int maxHeight;
} giza_band_t;

extern giza_band_t Band;

int _giza_init_band (int mode);
void _giza_refresh_band (int mode, int nanc, const int *xanc, const int *yanc, int x2, int y2);
void _giza_destroy_band (int mode);
void _giza_init_band_style (void);
