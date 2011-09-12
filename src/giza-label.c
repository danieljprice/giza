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
 * Copyright (C) 2010 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
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
giza_label (char *labelx, char *labely, char *title)
{
  if (!_giza_check_device_ready ("giza_label"))
    return;

  giza_annotate("B",3.0,0.5,0.5,labelx);
  giza_annotate("L",3.0,0.5,0.5,labely);
  giza_annotate("T",2.0,0.5,0.5,title);
}
