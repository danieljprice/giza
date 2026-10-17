/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2022 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING.LESSER file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2 of the LGPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the LGPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

#ifndef GIZA_TICK_PRIVATE_H
#define GIZA_TICK_PRIVATE_H

/* one character height in device pixels (cap height of the current font) */
double _giza_character_height_device (void);

void _giza_tick_label_style (double theta_deg, double disp, double angle,
                             double *label_angle, double *justification,
                             double *displacement);

/* draw one tick mark and optional label; GIZA_TRANS_WORLD must be active */
void _giza_draw_tick_mark (double x1, double y1, double x2, double y2,
                           double ratio, double tickl, double tickr,
                           double disp, double angle, const char *label,
                           double theta_deg, double tick_perp_x,
                           double tick_perp_y);

/* append one box tick to the current path; GIZA_TRANS_WORLD must be active */
void _giza_box_draw_tick (double x, double y, double tick_length, int major,
                          int draw_invert, double draw_project,
                          int draw_symmetric, double tick_perp_x,
                          double tick_perp_y, double win_cx, double win_cy);

#endif
