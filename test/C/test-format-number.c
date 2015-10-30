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
#include <math.h>

int
main ()
{

  int m, p;
  double num;
  char   string[40];

  m = 20000;
  p = -1;
  num = ((double) m) * pow (10, p);
  giza_format_number (m, p, 0, string);
  printf ("%e: %s\n", num, string);

  m = 20000;
  p = -1;
  num = ((double) m) * pow (10, p);
  giza_format_number (m, p, 0, string);
  printf ("%e: %s\n", num, string);
/*
 // exponentials: with 3 digit exponents (-ve)
  x = -1.e-300;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -3.e-286;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -3.e+123;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -3.e-123;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -9.92312342451423451155315e-123;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 4, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 5, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 10, string);
  printf("%e: %s \n",x,string);
 // exponentials: with 3 digit exponents (+ve)
  x = 2.e-300;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = 5.e-286;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = 6.e+123;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = 7.e-123;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = 9.92312342451423451155315e-123;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 4, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 5, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 10, string);
  printf("%e: %s \n",x,string);
 // exponentials: with 2 digit exponents (+ve)
  x = -1.e-30;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -3.e-28;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -3.e+12;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -3.e-13;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -256.14589345e-83;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 4, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 5, string);
  printf("%e: %s \n",x,string);

  giza_format_number(x, 10, string);
  printf("%e: %s \n",x,string);

  x = 1.e-30;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = 1.e+30;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = 6.283643E-29;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);

  x = -4.99999999999999980217349007449654627661539343338E-24;
  giza_format_number(x, 3, string);
  printf("%e: %s \n",x,string);
*/

}
