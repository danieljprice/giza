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

int
main ()
{
  double xpts[2], ypts[2];
  int i;
  xpts[0] = 0.0;
  xpts[1] = 1.;

  giza_open_device ("?", "line style");
  giza_start_warnings ();

  for (i = 1; i < 7; i++)
    {
      giza_set_line_style (i);
      ypts[0] = 0.05 * i;
      ypts[1] = 0.05 * i;

      giza_line (2, xpts, ypts);

    }

  giza_close_device ();
}
