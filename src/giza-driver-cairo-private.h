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

int _giza_open_device_cairo (double width, double height, int units);
void _giza_bind_cairo_context (cairo_t *cr);
void _giza_unbind_cairo_context (void);
void _giza_flush_device_cairo (void);
void _giza_change_page_cairo (void);
void _giza_close_device_cairo (void);
int _giza_complete_device_open (int draw_background);
void _giza_init_device_struct (giza_device_t *ptrDev);
void _giza_close_device_unchecked (void);
