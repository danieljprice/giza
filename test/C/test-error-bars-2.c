/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 5a) of the GPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
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

#define N 9

int main (void)
{
   char title [10];
   int dir;
   double x [N] = {-4, -3, -2, -1, 0, 1, 2, 3, 4};
   double y [N] = {-4, -3, -2, -1, 0, 1, 2, 3, 4};
   double err [N] = {1, .5, 1, .5, 1, .5, 1, .5, 1};

   (void) giza_open_device ("?", "Error bars");

   for (dir = 1; dir < 10 ; dir ++)
     {
	giza_set_environment (-6, 6, -6, 6, 1, 0);
	(void) sprintf (title, "dir = %d", dir);
	giza_label ("", "", title);
	giza_points (N, x, y, 2);
	giza_error_bars (dir, N, x, y, err, 2);
     }
   giza_close_device ();
}
