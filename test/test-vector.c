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

#include <giza.h>

int
main ()
{
  giza_open_device ("?", "arrows");
  giza_start_warnings ();
  giza_set_window (0., 10., 0., 10.);
  giza_box ("BCNT", 0., 0, "BCTN", 0., 0);

  int n = 2, m = 2;
  float hori[m][n], vert[m][n];
  float scale = 1.;
  float affine[6];

  hori[0][0] = .5;
  hori[0][1] = .5;
  hori[1][0] = .5;
  hori[1][1] = .5;
  vert[0][0] = .5;
  vert[0][1] = .5;
  vert[1][0] = .5;
  vert[1][1] = .5;
  affine[0] = 2.;
  affine[1] = 0.;
  affine[2] = 0.;
  affine[3] = 2.;
  affine[4] = 0.;
  affine[5] = 0.;

  giza_vector_float (n, m, hori, vert, 0, 1, 0, 1, scale, 0, affine, 1000.);

  giza_close_device ();
} 
