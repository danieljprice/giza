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

#include "giza-private.h"
#include "giza.h"

/**
 * Drawing: giza_box_time
 *
 * Synopsis: Same as giza_box, but labels axes with time-style labels
 *           i.e. DD HH MM SS as used in RA - DEC plots for Astronomy.
 *           If this option is used then giza_set_window should have been
 *           called with min/max given in seconds of time.
 *
 * Input:
 *  -xopt  :- As for giza_box, plus additional options below
 *  -xtick :- The distance, in world coordinates,
 *            between major ticks on the x-axis.
 *            If 0.0 the interval is chosen
 *            by giza_box_time.
 *  -nxsub :- The number of minor ticks to be placed
 *            between each major tick. If 0 the
 *            number is chosen by giza_box.
 *  -yopt  :- Similar to xdraw_minticks but for the y-axis.
 *  -ytick :- Similar to xtick but for the y-axis.
 *  -nysub :- Similar to nxsub but for the y-axis.
 *
 * Options for xopt and yopt (same as for giza_box, plus those below):
 *  -Z :- for time-style labelling: (DD) HH MM SS.S
 *  -Y :- exclude the day field so labels are just HH MM SS.S (hours can be >24)
 *  -X :- keep HH field in range (0..24)
 *  -H :- use d,h,m,s superscripts to label numbers
 *  -D :- use degree o, minute ' and second '' superscripts to label numbers
 *  -F :- do not draw the first label (left- or bottom-most)
 *  -O :- omit leading zeros in numbers < 10, i.e. 3 instead of 03
 *
 * See Also: giza_box, giza_box_float, giza_box_time_float
 */
void
giza_box_time (const char *xopt, double xtick, int nxsub,
	  const char *yopt, double ytick, int nysub)
{
  if (!_giza_check_device_ready ("giza_box_time"))
    return;

  giza_box(xopt,xtick,nxsub,yopt,ytick,nysub);

}

/**
 * Drawing: giza_box_time_float
 *
 * Synopsis: Same functionality as giza_box_time but takes floats instead of doubles.
 *
 * See Also: giza_box_time, giza_box
 */
void
giza_box_time_float (const char *xopt, float xtick, int nxsub,
		     const char *yopt, float ytick, int nysub)
{
  if (!_giza_check_device_ready ("giza_box_time_float"))
    return;

  giza_box_time (xopt, (double) xtick, nxsub, yopt, (double) ytick, nysub);
}
