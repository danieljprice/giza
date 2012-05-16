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

int
main ()
{
  int lc;
  double xpts[2], ypts[2];
  printf ("Expect an error\n");
  giza_set_line_cap (1);
  printf ("Expect an error\n");
  giza_get_line_cap (&lc);

  giza_open_device ("?", "test-line-cap");
  xpts[0] = 0.4;
  xpts[1] = 0.4;
  ypts[0] = 0.3;
  ypts[1] = 0.6;
  
  giza_start_warnings ();
  giza_set_line_width (3.);

  double lw;
  giza_get_line_width (&lw);
  printf("%f\n", lw);
  giza_line (2, xpts, ypts);
  giza_ptext (0.4, 0.63, 90., 0.0, "butt cap");

  xpts[0] = .5;
  xpts[1] = .5;

  giza_set_line_cap (1);

  giza_get_line_width (&lw);
  printf("%f\n", lw);
  giza_line (2, xpts, ypts);
  giza_ptext (0.5, 0.63, 90., 0., "round cap");

  xpts[0] = .6;
  xpts[1] = .6;

  giza_set_line_cap (2);
  giza_get_line_width (&lw);
  printf("%f\n", lw);
  giza_line (2, xpts, ypts);
  giza_ptext (.6, 0.63, 90., 0., "square cap");

  printf ("Expect a warning\n");
  giza_set_line_cap (-1);

  printf ("Expect a warning\n");
  giza_set_line_cap (3);

  giza_close_device ();
  return 0;
}
