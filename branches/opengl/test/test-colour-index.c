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
  giza_open_device ("?", "colours");

  double xpts[2], ypts[2];

  xpts[0] = 0.;
  xpts[1] = 1.;

  int i;
  for (i = 1; i < 20; i++)
    {
      giza_set_colour_index (i);

      ypts[0] = 0.02 * i;
      ypts[1] = 0.02 * i;

      giza_line (2, xpts, ypts);

    }

  giza_close_device ();

}
