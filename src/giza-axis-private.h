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

#ifndef GIZA_AXIS_PRIVATE_H
#define GIZA_AXIS_PRIVATE_H

/* axis tick geometry: screen direction and perpendicular unit vector
 * in world coordinates (GIZA_TRANS_WORLD must be active).  Returns 1 on
 * success, 0 if the axis segment has zero length in device space. */
int _giza_axis_tick_vectors (double x1, double y1, double x2, double y2,
                             double *theta_deg,
                             double *tick_perp_x, double *tick_perp_y);

#endif
