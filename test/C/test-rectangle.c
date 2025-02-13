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

#include <giza.h>
#include <stdio.h>

void aux (double x1, double x2, double y1, double y2, int fill, int ci);

int
main ()
{
  giza_open_device ("?", "rect");

  aux (.5, .7, .5, 0.7, 1, 1);

  giza_change_page ();

  aux(0.3, 0.5, 0.1, 0.3, 2, 3);

  giza_close_device ();
}

void
aux (double x1, double x2, double y1, double y2, int fill, int ci)
{
  char tmp[100];
  giza_set_fill (fill);
  giza_set_colour_index (ci);
  giza_rectangle (x1, x2, y1, y2);

  sprintf (tmp, "(x1, y1) = (%.2f, %.2f), (x2, y2) = (%.2f, %.2f), fill: %i", x1, y1, x2, y2, fill);
  giza_ptext (1., 1., 0., 1., tmp);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);

}
