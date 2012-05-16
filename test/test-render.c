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
#include <math.h>

int
main ()
{
  giza_open_device ("?", "render");
  giza_set_window (0., 10., 0., 10.);
  giza_box ("BCNTS", 0., 0, "BCNTS", 0., 0);

  double cp[2];
  double r[2];
  cp[0] = 0.;
  cp[1] = 1.;
  giza_set_colour_table (cp, cp, cp, cp, 2);
  giza_start_warnings ();
  double data[100][100], affine[6];
  int i, j;
  for (i = 0; i < 10; i++)
    {
      for (j = 0; j < 10; j++)
	{
	  data[i][j] = 1;//i * .1;
	}
    }

  affine[0] = 1.;
  affine[1] = 0.;
  affine[2] = 0.;
  affine[3] = 1.;
  affine[4] = 0.;
  affine[5] = 0.;

  giza_render (10, 10, data, 0, 9, 0, 9, 0., 1., affine);

  giza_change_page ();

  double data2[2][2];
  data[0][0] = 1.;
  data[0][1] = 1.;
  data[1][0] = 1.;
  data[1][1] = 1.;

  giza_set_window (0., 1., 0., 1.);
  affine[4] = .5;
  affine[5] = .5;
  
  cp[0] = 0.;
  cp[1] = 1.;
  giza_set_colour_table (cp, cp, cp, cp, 2);
  giza_render (2, 2, data2, 0, 1, 0, 1, 0., 1., affine);

  giza_box ("BCNTS", 0., 0, "BCNTS", 0., 0);
  giza_flush_device ();

  giza_change_page ();

  giza_set_window (.0, 100., 0., 100.);
  giza_box ("BCNTS", 0., 0, "BCNTS", 0., 0);
  int n = 5;
  double controlpoints[n];
  double red[n], green[n], blue[n];

  affine[0] = 1.;
  affine[1] = 0.;
  affine[2] = 0.;
  affine[3] = 1.;
  affine[4] = 0.;
  affine[5] = 0.;

  controlpoints[0] = 0.;
  controlpoints[1] = 0.475;
  controlpoints[2] = 0.694;
  controlpoints[3] = 0.745;
  controlpoints[4] = 1.;

  red[0] = 0.;
  red[1] = 0.686;
  red[2] = 1.;
  red[3] = 1.;
  red[4] = 1.;

  green[0] = 0.;
  green[1] = 0.004;
  green[2] = 0.420;
  green[3] = 0.518;
  green[4] = 1.;

  blue[0] = 0.;
  blue[1] = 0.;
  blue[2] = 0.;
  blue[3] = 0.;
  blue[4] = 1.;

  for (i = 0; i < 100; i++)
    {
      for (j = 0; j < 100; j++)
        {
          data[i][j] = sqrt (i * j);
        }
    }

  giza_set_colour_table (controlpoints, red, green, blue, 100);
  giza_render (100, 100, data, 0, 99, 0, 99, 0, 100, affine);

  giza_close_device ();
}
