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
#include <giza.h>

/**
 * Text: giza_label
 *
 * Synopsis: Labels the plot
 *
 * Input:
 *  -labelx :- x-axis label
 *  -labely :- y-axis label
 *  -title  :- The title of the plot
 */
void
giza_label (const char *labelx, const char *labely, const char *title)
{
  if (!_giza_check_device_ready ("giza_label"))
    return;

  giza_annotate("B",3.0,0.5,0.5,labelx);
  giza_annotate("L",3.0,0.5,0.5,labely);
  giza_annotate("T",2.0,0.5,0.5,title);
}
