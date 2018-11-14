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
 *  --4 :- draw box and major tick marks only
 *  --3 :- draw box and tick marks (major and minor) only
 *  --2 :- no axis, no box, no labels
 *  --1 :- draw the box only
 *  -0  :- draw a box and label it with world coords
 *  -1  :- same as 0 but also draw the axis
 *  -2  :- same as 1 but also draw grid lines at major intervals
 *  -10  :- draw box and label X-axis logarithmically
 *  -20  :- draw box and label Y-axis logarithmically
 *  -30  :- draw box and label X- and Y-axis logarithmically
 *
 */
void
giza_set_environment (double xmin, double xmax, double ymin, double ymax, int just, int axis)
{
  if(!_giza_check_device_ready ("giza_set_environment"))
    return;

  giza_change_page ();
  giza_set_viewport_default ();

  char opts[8], yopts[8];

  switch(axis)
    {
    case -4:
      strcpy (opts, "BCT");
      strcpy (yopts, "BCT");
      break;
    case -3:
      strcpy (opts, "BCST");
      strcpy (yopts, "BCST");
      break;
    case -2:
      strcpy (opts, "");
      strcpy (yopts,"");
      break;
    case -1:
      strcpy (opts, "BC");
      strcpy (yopts,"BC");
      break;
    case 1:
      strcpy (opts, "ABCTSN");
      strcpy (yopts,"ABCTSN");
      break;
    case 2:
      strcpy (opts, "ABCTSNG");
      strcpy (yopts,"ABCTSNG");
      break;
    case 10:
      strcpy (opts, "BCTSNL");
      strcpy (yopts,"BCTSN");
      break;
    case 20:
      strcpy (opts, "BCTSN");
      strcpy (yopts,"BCTSNL");
      break;
    case 30:
      strcpy (opts, "BCTSNL");
      strcpy (yopts,"BCTSNL");
      break;
    default:
      _giza_warning ("giza_set_environment", "Invalid axis option, setting to 0");
    case 0:
      strcpy (opts, "BCTSN");
      strcpy (yopts,"BCTSN");
      break;
    }

  if (just)
    giza_set_window_equal_scale (xmin, xmax, ymin, ymax);
  else
    giza_set_window (xmin, xmax, ymin, ymax);

  giza_box (opts, 0., 0, yopts, 0., 0);
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
