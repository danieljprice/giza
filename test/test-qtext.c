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
  giza_open_device ("?", "qtext");
  giza_start_warnings ();

  //giza_set_fill (2);
  //giza_set_window (0., 1., 0., 1.);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);

  double x, y, angle, just, xbox[4], ybox[4];
  char text[100];

  giza_qtext (0., 0., 0., 0., "HELLO", xbox, ybox);
  giza_ptext (0., 0., 0., 0., "HELLO");

  printf ("x = 0., y = 0., angle = 0., just = 0., text = HELLO\n");
  printf ("XBOX = %.4f, %.4f, %.4f, %.4f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("YBOX = %.4f, %.4f, %.4f, %.4f\n\n", ybox[0], ybox[1], ybox[2], ybox[3]);
 
  giza_qtext (0.5, 0., 90., 0., "HELLO", xbox, ybox);
  giza_ptext (0.5, 0., 90., 0., "HELLO");

  printf ("x = 0.5, y = 0., angle = 90., just = 0., text = HELLO\n");
  printf ("XBOX = %.4f, %.4f, %.4f, %.4f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("YBOX = %.4f, %.4f, %.4f, %.4f\n\n", ybox[0], ybox[1], ybox[2], ybox[3]);
 
  giza_qtext (0., 0.5, 35., 0., "HELLO", xbox, ybox);
  giza_ptext (0., 0.5, 35., 0., "HELLO");

  printf ("x = 0., y = 0.5, angle = 35., just = 0., text = HELLO\n");
  printf ("XBOX = %.4f, %.4f, %.4f, %.4f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("YBOX = %.4f, %.4f, %.4f, %.4f\n\n", ybox[0], ybox[1], ybox[2], ybox[3]);
  
  giza_change_page ();

  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);

  giza_qtext (0., 0., 0., 0.5, "HELLO", xbox, ybox);
  giza_ptext (0., 0., 0., 0.5, "HELLO");

  printf ("x = 0., y = 0., angle = 0., just = 0.5, text = HELLO\n");
  printf ("XBOX = %.4f, %.4f, %.4f, %.4f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("YBOX = %.4f, %.4f, %.4f, %.4f\n\n", ybox[0], ybox[1], ybox[2], ybox[3]);
  
  giza_qtext (0.5, 0., 90., 0.5, "HELLO", xbox, ybox);
  giza_ptext (0.5, 0., 90., 0.5, "HELLO");

  printf ("x = 0.5, y = 0., angle = 90., just = .5, text = HELLO\n");
  printf ("XBOX = %.4f, %.4f, %.4f, %.4f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("YBOX = %.4f, %.4f, %.4f, %.4f\n\n", ybox[0], ybox[1], ybox[2], ybox[3]);
 
  giza_qtext (0., 0.5, 35., 0.5, "HELLO", xbox, ybox);
  giza_ptext (0., 0.5, 35., 0.5, "HELLO");

  printf ("x = 0., y = 0.5, angle = 35., just = 0.5, text = HELLO\n");
  printf ("XBOX = %.4f, %.4f, %.4f, %.4f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("YBOX = %.4f, %.4f, %.4f, %.4f\n\n", ybox[0], ybox[1], ybox[2], ybox[3]);
  
  giza_change_page ();
 
  x = 0.;
  y = 0.;
  angle = 90.;
  just = .0;
  strcpy (text, "HELLO^2!");
  //giza_ptext (x, y, angle, just, text);
  giza_qtext (x, y, angle, just, text, xbox, ybox);
  //giza_rectangle (xbox[0], xbox[2], ybox[0], ybox[2]);
  printf ("X[0] = %f, x[1] = %f, X[2] = %f, X[3] = %f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("Y[0] = %f, Y[1] = %f, Y[2] = %f, Y[3] = %f\n", ybox[0], ybox[1], ybox[2], ybox[3]);

  giza_change_page ();

  angle = 90.;
  //giza_ptext (x, y, angle, just, text);
  giza_qtext (x, y, angle, just, text, xbox, ybox);
  //giza_rectangle (xbox[0], xbox[2], ybox[0], ybox[2]);
  printf ("X[0] = %f, x[1] = %f, X[2] = %f, X[3] = %f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("Y[0] = %f, Y[1] = %f, Y[2] = %f, Y[3] = %f\n", ybox[0], ybox[1], ybox[2], ybox[3]);
 
  giza_change_page ();

  angle = 0.;
  just = 0.5;
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_qtext (x, y, angle, just, text, xbox, ybox);
  giza_ptext (x, y, angle, just, text);
  giza_rectangle (xbox[0], xbox[2], ybox[0], ybox[2]);
  printf ("X[0] = %f, x[1] = %f, X[2] = %f, X[3] = %f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("Y[0] = %f, Y[1] = %f, Y[2] = %f, Y[3] = %f\n", ybox[0], ybox[1], ybox[2], ybox[3]);

  giza_change_page ();

  just = 0.0;
  angle = 30.;
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_qtext (x, y, angle, just, text, xbox, ybox);
  giza_ptext (x, y, angle, just, text);
  giza_rectangle (xbox[0], xbox[2], ybox[0], ybox[2]);
  printf ("X[0] = %f, x[1] = %f, X[2] = %f, X[3] = %f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("Y[0] = %f, Y[1] = %f, Y[2] = %f, Y[3] = %f\n", ybox[0], ybox[1], ybox[2], ybox[3]);

 giza_change_page ();

  giza_set_window (-10., 10., -10., 10.);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_qtext (x, y, angle, just, text, xbox, ybox);
  giza_ptext (x, y, angle, just, text);
  printf ("X[0] = %f, x[1] = %f, X[2] = %f, X[3] = %f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("Y[0] = %f, Y[1] = %f, Y[2] = %f, Y[3] = %f\n", ybox[0], ybox[1], ybox[2], ybox[3]);

  giza_change_page ();

  giza_set_window (0., 1000., 0., 1.);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_qtext (x, y, angle, just, text, xbox, ybox);
  giza_ptext (x, y, angle, just, text);
  printf ("X[0] = %f, x[1] = %f, X[2] = %f, X[3] = %f\n", xbox[0], xbox[1], xbox[2], xbox[3]);
  printf ("Y[0] = %f, Y[1] = %f, Y[2] = %f, Y[3] = %f\n", ybox[0], ybox[1], ybox[2], ybox[3]);

  giza_close_device ();
}
 
