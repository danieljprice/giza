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

int 
main ()
{
  giza_open_device ("?", "errors");
  giza_start_warnings ();
  double xpts[2], ypts[2], errors[2];
  xpts[0] = .3;
  xpts[1] = 0.7;
  ypts[0] = 0.7;
  ypts[1] = 0.3;
  errors[0] = 0.01;
  errors[1] = 0.1;

  giza_points (2, xpts, ypts, 4);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_error_bars (1, 2, xpts, ypts, errors, 1.);


  giza_change_page ();

  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_error_bars (2, 2, xpts, ypts, errors, 1.); 
  giza_points (2, xpts, ypts, 178);

  giza_change_page ();

  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_error_bars (3, 2, xpts, ypts, errors, 1.); 
  giza_points (2, xpts, ypts, 178);

  giza_change_page ();

  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_error_bars (4, 2, xpts, ypts, errors, 4.); 
  giza_points (2, xpts, ypts, 178);

  giza_change_page ();

  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_error_bars (5, 2, xpts, ypts, errors, 4.); 
  giza_points (2, xpts, ypts, 178);

  giza_change_page ();

  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_error_bars (6, 2, xpts, ypts, errors, 4.); 
  giza_points (2, xpts, ypts, 178);

  giza_close_device ();
}
