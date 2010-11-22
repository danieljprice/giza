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

struct GIZA_Band
{
  cairo_surface_t *onscreen;
  cairo_t *box;
  cairo_t *restore;
  double lw;
  int ls;
  int maxWidth;
  int maxHeight;
} Band;

int _giza_init_band (int mode);
void _giza_refresh_band (int mode, int x1, int y1, int x2, int y2);
void _giza_destroy_band (int mode);
void _giza_init_band_style ();
