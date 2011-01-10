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
  giza_open_device ("?", "arrows");
  giza_start_warnings ();
  giza_arrow (0.475, 0.475, 0.5, 0.5);

  giza_set_arrow_style (2, 45., 0.);
  giza_arrow (0.525, 0.525, 0.550, 0.550);

  giza_set_arrow_style (1, 30., 0.);
  giza_arrow (0.575, 0.575, 0.600, 0.600);

  giza_set_arrow_style (1, 45., .9);
  giza_arrow (0.625, 0.625, 0.650, 0.650);
  giza_close_device ();
}
