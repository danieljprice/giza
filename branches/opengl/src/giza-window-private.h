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

#define GIZA_DEFAULT_WINDOW_X1 0.0
#define GIZA_DEFAULT_WINDOW_X2 1.0
#define GIZA_DEFAULT_WINDOW_Y1 0.0
#define GIZA_DEFAULT_WINDOW_Y2 1.0

struct GIZA_Window
{
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  cairo_matrix_t userCoords;
  cairo_matrix_t normCoords;
} Win;

void _giza_init_window (void);
