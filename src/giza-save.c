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
#define GIZA_SAVE_MAX 30
int _giza_save_ncalls;
double lw[GIZA_SAVE_MAX];
double ch[GIZA_SAVE_MAX];
int ls[GIZA_SAVE_MAX];
int lc[GIZA_SAVE_MAX];
int fs[GIZA_SAVE_MAX];
int ci[GIZA_SAVE_MAX];
int arrowfs[GIZA_SAVE_MAX];
double arrowangle[GIZA_SAVE_MAX];
double arrowcutback[GIZA_SAVE_MAX];

/**
 * Settings: giza_save
 *
 * Synopsis: Saves current plot settings
 *
 * See Also: giza_restore
 */
void
giza_save ()
{
  if (!_giza_check_device_ready ("giza_save"))
    return;

  _giza_save_ncalls += 1;
  if (_giza_save_ncalls > GIZA_SAVE_MAX) {
     _giza_warning("giza_save","max number of giza_save calls exceeded");
     return;
  }
  int i = _giza_save_ncalls - 1;
  giza_get_line_width(&lw[i]);
  giza_get_line_style(&ls[i]);
  giza_get_line_cap(&lc[i]);
  giza_get_fill(&fs[i]);
  giza_get_colour_index(&ci[i]);
  giza_get_character_height(&ch[i]);
  giza_get_arrow_style(&arrowfs[i],&arrowangle[i],&arrowcutback[i]);
//  giza_get_font(&font[i]);
}

/**
 * Settings: giza_restore
 *
 * Synopsis: Restores plot settings stored via giza_save
 *
 * See Also: giza_save
 */
void
giza_restore ()
{
  if (!_giza_check_device_ready ("giza_restore"))
    return;

  _giza_save_ncalls = _giza_save_ncalls - 1;
  if (_giza_save_ncalls < 0) {
     _giza_warning("giza_restore","unmatched giza_save/giza_restore calls");
     _giza_save_ncalls = 0;
     return;
  }
  int i = _giza_save_ncalls;
  giza_set_line_width(lw[i]);
  giza_set_line_style(ls[i]);
  giza_set_line_cap(lc[i]);
  giza_set_fill(fs[i]);
  giza_set_colour_index(ci[i]);
  giza_set_character_height(ch[i]);
  giza_set_arrow_style(arrowfs[i],arrowangle[i],arrowcutback[i]);
// giza_set_font(&font[i]);

}

/**
 * Saves current plot settings
 *
 */
void
_giza_init_save()
{
  _giza_save_ncalls = 0;
  return;
}
