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

void set_triangle(double angle, double *xpts, double *ypts)
{
   xpts[0] = 0.5;
   ypts[0] = 0.5;
   xpts[1] = 0.5 + 0.25*cos(3.1415926/180.*angle);
   ypts[1] = 0.5;
   xpts[2] = 0.5;
   ypts[2] = 0.5 + 0.5*sin(3.1415926/180.*angle);
}

int
main ()
{
  giza_open_device ("?", "arrows");
  giza_start_warnings ();
  giza_set_window(0.,1.,0.,10.);
  giza_box ("BCTN", 0., 0, "BCTN", 0., 0);
  giza_arrow (0.475, 0.475, 0.5, 0.5);
  giza_arrow (0.5, 0., 0.5, 0.25);
  giza_arrow (0., 0.5, 0.25, 0.5);

  giza_set_arrow_style (2, 10., 0.);
  giza_arrow (0.525, 0.525, 0.550, 0.550);

  giza_set_arrow_style (1, 45., 0.);
  giza_arrow (0.425, 0.425, 0.450, 0.450);

  giza_set_arrow_style (1, 30., 0.);
  giza_arrow (0.575, 0.575, 0.600, 0.600);

  giza_set_arrow_style (1, 45., 1.0);
  giza_arrow (0.625, 0.625, 0.650, 0.650);
 
  giza_set_window(0.,1.,0.,1.);

  giza_arrow (0.5, 0., 0.5, 0.25);
  giza_arrow (0., 0.5, 0.25, 0.5);

  double angle=30.;
  double xpts[3],ypts[3];
  set_triangle(angle,xpts,ypts);
  giza_set_fill(2);
  giza_polygon(3,xpts,ypts);
  giza_set_arrow_style(1,angle,0.);
  giza_arrow (0.5, 0.5, 0.5 + 0.25*cos(3.1415926/180.*angle), 0.5);

  giza_set_arrow_style (2, 45., 0.);
  giza_arrow (0.525, 0.525, 0.550, 0.550);

  giza_set_arrow_style (1, 85., 0.);
  giza_arrow (0.425, 0.425, 0.450, 0.450);

  giza_set_arrow_style (1, 30., 0.);
  giza_arrow (0.575, 0.575, 0.600, 0.600);

  giza_set_arrow_style (1, 45., .9);
  giza_arrow (0.625, 0.625, 0.650, 0.650);

  giza_close_device ();
}
