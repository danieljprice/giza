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

void aux (double lw);

int
main ()
{
  giza_open_device ("?", "lines");
  
  aux (5.);

  giza_change_page ();

  aux (1.);

  giza_change_page ();

  aux (4.);

  giza_close_device ();
  /*
  printf ("Expect an error\n");
  giza_set_line_width (1);

  int ierr;
  ierr = giza_open_device ("?", "test-set-line-width");

  giza_start_warnings ();

  double xpts[2], ypts[2];
  xpts[0] = 0;
  xpts[1] = 1;
  ypts[0] = 0.9;
  ypts[1] = 0.9;
  giza_line (2, xpts, ypts);
  double lw;
  giza_get_line_width (&lw);
  printf("%f\n", lw);
  giza_ptext (1., 0.92, 0., 1., "Line Width = 1.0");

  giza_set_line_width (1.5);
  ypts[0] = 0.8;
  ypts[1] = 0.8;
  giza_line (2, xpts, ypts);
  giza_get_line_width (&lw);
  printf("%f\n", lw);
  giza_ptext (1., 0.82, 0., 1., "Line Width = 1.5");

  giza_set_line_width (2);
  ypts[0] = 0.7;
  ypts[1] = 0.7;
  giza_line (2, xpts, ypts);
  giza_get_line_width (&lw);
  printf("%f\n", lw);
  giza_ptext (1., 0.72, 0., 1., "Line Width = 2.0");

  giza_set_line_width (2.5);
  ypts[0] = 0.6;
  ypts[1] = 0.6;
  giza_get_line_width (&lw);
  printf("%f\n", lw);
  giza_line (2, xpts, ypts);
  giza_ptext (1., 0.62, 0., 1., "Line Width = 2.5");

  printf ("Expect a warning\n");
  giza_set_line_width (0.0);

  printf ("Expect a warning\n");
  giza_set_line_width (-0.0);

  giza_close_device ();
  */
}

void
aux (double lw)
{
  double xpts[2], ypts[2];
  char str[100];
  xpts[0] = 0.3;
  xpts[1] = .7;
  ypts[0] = .1;
  ypts[1] = .3;

  giza_set_line_width (lw);
  giza_set_line_cap (0);

  giza_line (2, xpts, ypts);

  giza_set_line_cap (1);
  ypts[0] = 0.4;
  ypts[1] = 0.6;

  giza_line (2, xpts, ypts);

  giza_set_line_cap (2);
  ypts[0] = 0.7;
  ypts[1] = 0.9;

  giza_line (2, xpts, ypts);

  sprintf (str, "line width: %.2f", lw);
  giza_ptext (1., 0., 0., 1., str);
}
