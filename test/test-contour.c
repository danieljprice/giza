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
  giza_open_device ("?", "contour");
  giza_start_warnings ();
  //giza_set_window (-1.5, 1.5, -1.5, 1.5);
  giza_set_window (0., 1., .0, 1.);
 
  int dimx = 10, dimy = 10;
  int i,j;
  int ncont = 2;
  double cont1[2] = {.5, .75};
  
  double affine1[6] = {1./(double)dimx, 0., 0., 1./(double)dimy, 0., 0.};
  double data1[dimy][dimx];

  for (i = 0; i < dimx; ++i)
    {
      for (j = 0; j < dimy; ++j)
        {
	  data1[j][i] = (double) i/(double)dimx;
	}
    }
  
  giza_contour (dimx, dimy, data1, 0, dimx - 1, 0, dimy - 1, ncont, cont1, affine1);
  giza_box ("BNCT", 0., 0, "BCNT", 0., 0);

  giza_change_page ();

  giza_set_window (-1.5, 1.5, -1.5, 1.5);
  dimx = 150;
  dimy = 150;
  ncont = 5;
  double cont[5] = {-1., -.5, 0., .5, 1.};
  double affine[6] = {3./((double)dimx), 0., 0., 3./((double) dimy), -1.5, -1.5};
  double data[dimy][dimx];
  double tmp1, tmp2, tmp3;

  for (i = 0; i < dimx; ++i)
    {
      for (j = 0; j < dimy; ++j)
        {
          double x = -1.5 + (double)i/((double)dimx - 1) * 3.; 
          double y = -1.5 + (double)j/((double)dimy - 1) * 3.;
          data[j][i] = x * cos(10*x*y);
	  //tmp1 = (x * x) + (y - 0.842) * (y + 0.842);
	  //tmp2 = x * (y-0.842) + x * ( y + 0.842);
	  //tmp3 = (tmp1 * tmp1 + tmp2*tmp2);
	  //data[j][i] = 1/(tmp3 * tmp3);
          //printf ("f(%f, %f) = %f\n", x, y, data[j][i]);
	}
    }

  giza_render (dimx, dimy, data, 0, dimx - 1, 0, dimy - 1, -1., 1., affine);
  giza_contour (dimx, dimy, data, 0, dimx - 1, 0, dimy - 1, ncont, cont, affine);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_close_device ();

}
