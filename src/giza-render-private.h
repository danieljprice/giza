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

static void _giza_colour_pixel (unsigned char *array, int pixNum, double pos);
static void _giza_colour_pixel_transparent (unsigned char *array, int pixNum, double pos);
static void _giza_colour_pixel_alpha (unsigned char *array, int pixNum, double pos, double alpha);
void _giza_render (int sizex, int sizey, const double* data, int i1, int i2,
	           int j1, int j2, double valMin, double valMax, const double *affine,
                   int transparent, int extend, const double* datalpha);
void _giza_render_float (int sizex, int sizey, const float* data, int i1,
		   int i2, int j1, int j2, float valMin, float valMax,
		   const float *affine, int transparent, int extend, const float* datalpha);
void _giza_get_extend (int extend, cairo_extend_t *cairoextend);
