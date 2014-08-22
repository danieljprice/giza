/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2014 Daniel Price
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
#include "giza-subpanel-private.h"
#include <giza.h>
#include <stdlib.h>

/**
 * Device: giza_subpanel
 *
 * Synopsis: Set the number of sub panels
 *
 * Input:
 *  -nx :- number of sub-panels in x direction
 *  -ny :- number of sub panels in y direction
 *
 * If nx is negative then panels are traversed
 * down-then-across instead of the default
 * across-then-down
 *
 * See also: giza_select_panel, giza_get_panel
 *
 */
void
giza_subpanel (int nx, int ny)
{
  if (!_giza_check_device_ready ("giza_subpanel"))
    return;

  /* query viewport */
  double vpxmin, vpxmax, vpymin, vpymax;
  giza_get_viewport(GIZA_UNITS_NORMALISED,&vpxmin,&vpxmax,&vpymin,&vpymax);

  if (nx < 0) {
     Dev[id].altpanelorder = 1;
  } else {
     Dev[id].altpanelorder = 0;
  }
  nx = abs(nx);
  ny = abs(ny);
  if (nx < 1) nx = 1;
  if (ny < 1) ny = 1;
  Dev[id].nx = nx;
  Dev[id].ny = ny;
  Dev[id].panelwidth  = Dev[id].width/nx;
  Dev[id].panelheight = Dev[id].height/ny;

  /* set current panel */
  Dev[id].ix = nx;
  Dev[id].iy = ny;
  
  /* set viewport */
  giza_set_viewport(vpxmin,vpxmax,vpymin,vpymax);
}

/**
 * Device: giza_set_panel
 *
 * Synopsis: Select the panel we are currently plotting in
 *
 * Input:
 *  -ix :- panel index in x direction
 *  -iy :- panel index in y direction
 *
 * See also: giza_subpanel, giza_get_panel
 *
 */
void
giza_set_panel (int ix, int iy)
{
  if (!_giza_check_device_ready ("giza_select_panel"))
    return;

  if (ix < 1 || ix > Dev[id].nx) {
     ix = 1;
  }
  if (iy < 1 || iy > Dev[id].ny) {
     iy = 1;
  }

  Dev[id].ix = ix;
  Dev[id].iy = iy;

  /* set viewport */
  giza_set_viewport(Dev[id].VP.xmin,Dev[id].VP.xmax,Dev[id].VP.ymin,Dev[id].VP.ymax);

}

/**
 * Device: giza_get_panel
 *
 * Synopsis: Select the panel we are currently plotting in
 *
 * Input:
 *  -ix :- panel index in x direction
 *  -iy :- panel index in y direction
 *
 * See also: giza_subpanel, giza_set_panel
 *
 */
void
giza_get_panel (int *ix, int *iy)
{
  if (!_giza_check_device_ready ("giza_query_panel"))
    return;

  *ix = Dev[id].ix;
  *iy = Dev[id].iy;

}

/**
 * Device: giza_init_subpanel
 *
 * Synopsis: initialises subpanel settings for device
 *
 */
void
_giza_init_subpanel ()
{
  if (!_giza_check_device_ready ("_giza_init_subpanel"))
    return;

  Dev[id].nx = 1;
  Dev[id].ny = 1;
  Dev[id].ix = 1;
  Dev[id].iy = 1;
  Dev[id].altpanelorder = 0;
  Dev[id].panelwidth  = Dev[id].width;
  Dev[id].panelheight = Dev[id].height;
}

/**
 * Device: giza_advance_panel
 *
 * Synopsis: Moves to next panel
 *
 */
void
_giza_advance_panel (int *newpage)
{
   int ix = Dev[id].ix;
   int iy = Dev[id].iy;
   *newpage = 0;
   
   if (Dev[id].altpanelorder) {
      iy = iy + 1;
      if (iy > Dev[id].ny) {
         iy = 1;
         ix = ix + 1;
         if (ix > Dev[id].nx) {
            ix = 1;
            *newpage = 1;
         }
      }
   } else {
      ix = ix + 1;
      if (ix > Dev[id].nx) {
         ix = 1;
         iy = iy + 1;
         if (iy > Dev[id].ny) {
            iy = 1;
            *newpage = 1;
         }
      }
   }

   Dev[id].ix = ix;
   Dev[id].iy = iy;
}
