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

#include "giza-private.h"
#include "giza-io-private.h"
#include <giza.h>
#include <string.h>

/**
 * Settings: giza_set_environment
 *
 * Synopsis: Sets the plotting environment.
 *
 * Input:
 *  -xmin :- The min x value in world coords
 *  -xmax :- The max x value in world coords
 *  -ymin :- The min y value in world coords
 *  -ymax :- The max y value in world coords
 *  -just :- Give the x and y axis equal scales
 *  -axis :- Options for drawing axis, ticks etc.
 *
 * Axis:
 *  --2 :- no axis, no box, no labels
 *  --1 :- draw the box only
 *  -0  :- draw a box and label it with world coords
 *  -1  :- same as 0 but draw the axis aswell (not yet implimented)
 *
 */
void
giza_set_environment (double xmin, double xmax, double ymin, double ymax, int just, int axis)
{
  if(!_giza_check_device_ready ("giza_set_environment"))
    return;
    
  giza_change_page();
  giza_set_viewport_default();

  char opts[7];
  
  switch(axis)
    {
    case -2:
      strcpy (opts, "");
      break;
    case -1:
      strcpy (opts, "BC");
      break;
    case 0:
      strcpy (opts, "BCTSN");
      break;
    case 1:
      strcpy (opts, "ABCTSN");
      break;
    default:
      _giza_warning ("giza_set_environment", "Invalid axis option, setting to 0");
      strcpy (opts, "BCTSN");
      break;
    }

  if (just)
    giza_set_window_equal_scale (xmin, xmax, ymin, ymax);
  else
    giza_set_window (xmin, xmax, ymin, ymax);

  giza_box (opts, 0., 0, opts, 0., 0);
}

/**
 * Settings: giza_set_environment_float
 *
 * Synopsis: Same functionality as giza_set_environment but takes floats
 */
void
giza_set_environment_float (float xmin, float xmax, float ymin, float ymax, int just, int axis)
{
  giza_set_environment ((double) xmin, (double) xmax, (double) ymin, (double) ymax, just, axis);
}
