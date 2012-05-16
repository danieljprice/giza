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
#include <stdio.h>

void aux (int symbol, double ch);

int
main ()
{
  giza_open_device ("?", "test-points");

  aux (0, 1.);
  giza_change_page ();

  aux (0, 1.5);
  giza_change_page ();

  aux (1, 1.);
  giza_change_page ();

  aux (2, 1.);
  giza_change_page ();

  aux (3, 1.);
  giza_change_page ();

  aux (38, 1.);
  giza_change_page ();

  aux (38, 1.5);
  giza_change_page ();

  giza_close_device ();
}

void
aux (int symbol, double ch)
{
  double xpts[3], ypts[3];
  char str[100];

  xpts[0] = 0.3;
  xpts[1] = 0.5;
  xpts[2] = 0.7;
  ypts[0] = 0.3;
  ypts[1] = 0.5;
  ypts[2] = 0.7;

  giza_set_character_height (ch);
  giza_points (3, xpts, ypts, symbol);
  sprintf (str, "Symbol: %i, Character height: %f", symbol, ch);
  giza_ptext (1., 0., 0., 1., str);
}
