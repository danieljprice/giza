/* giza - a scientific plotting layer built on cairo
 *
 * This file is (or was) part of GIZA, a scientific plotting
 * layer built on cairo.
 * GIZA comes with ABSOLUTELY NO WARRANTY.
 * This is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * Copyright (C) 2010 James Wetter. All rights reserved.
 * Contact: wetter.j@gmail.com
 *
 */

#include <giza.h>

int
main ()
{
  giza_open_device ("?", "box!");
  giza_set_window (-5., 5., 0., 1.);
  giza_box ("BCTLN", 0., 0, "BCTN", 0., 0);

  giza_change_page ();

  giza_set_window (0., 100., 0., 1.);
  giza_box ("BCTNLS", 0., 0, "BCTN", 0., 0);

  giza_change_page ();

  giza_set_window (10.2, 11., 0., 1.);
  giza_box ("BCTNL", 0., 0, "BCTN", 0., 0);

  giza_change_page ();

  giza_set_window (0., 1., -5., 5.);
  giza_box ("BCTN", 0., 0, "BCTNL", 0., 0);

  giza_change_page ();

  giza_set_window (0., 1., 0., 100.);
  giza_box ("BCTNS", 0., 0, "BCTNL", 0., 0);

  giza_change_page ();

  giza_set_window (0., 1., 10.2, 11.);
  giza_box ("BCTN", 0., 0, "BCTNL", 0., 0);

 giza_close_device ();
}
