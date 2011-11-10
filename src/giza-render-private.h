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

static void _giza_colour_pixel (unsigned char *array, int pixNum, double pos);
static void _giza_colour_pixel_transparent (unsigned char *array, int pixNum, double pos);
void _giza_render (int sizex, int sizey, const double* data, int i1, int i2,
	           int j1, int j2, double valMin, double valMax, const double *affine, int transparent);
void _giza_render_float (int sizex, int sizey, const float* data, int i1,
		   int i2, int j1, int j2, float valMin, float valMax,
		   const float *affine, int transparent);
