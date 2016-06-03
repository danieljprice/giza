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
#include "giza-io-private.h"
#include "giza-text-private.h"
#include <giza.h>

#define GIZA_SAVE_MAX 30
static int _giza_save_ncalls;
static double lw[GIZA_SAVE_MAX];
static double ch[GIZA_SAVE_MAX];
static int ls[GIZA_SAVE_MAX];
static int lc[GIZA_SAVE_MAX];
static int fs[GIZA_SAVE_MAX];
static int ci[GIZA_SAVE_MAX];
static int textbgci[GIZA_SAVE_MAX];
static int arrowfs[GIZA_SAVE_MAX];
static double arrowangle[GIZA_SAVE_MAX];
static double arrowcutback[GIZA_SAVE_MAX];
static double hatch_angle[GIZA_SAVE_MAX];
static double hatch_spacing[GIZA_SAVE_MAX];
static double hatch_phase[GIZA_SAVE_MAX];
static double band_lw[GIZA_SAVE_MAX];
static int band_ls[GIZA_SAVE_MAX];
static char font[GIZA_FONT_LEN];

/**
 * Settings: giza_save
 *
 * Synopsis: Saves current plot settings
 *
 * See Also: giza_restore
 */
void
giza_save (void)
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
  giza_get_text_background(&textbgci[i]);
  giza_get_character_height(&ch[i]);
  giza_get_arrow_style(&arrowfs[i],&arrowangle[i],&arrowcutback[i]);
  giza_get_font(font,20);
  giza_get_hatching_style(&hatch_angle[i],&hatch_spacing[i],&hatch_phase[i]);
  giza_get_band_style(&band_ls[i],&band_lw[i]);
}

/**
 * Settings: giza_restore
 *
 * Synopsis: Restores plot settings stored via giza_save
 *
 * See Also: giza_save
 */
void
giza_restore (void)
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
  giza_set_text_background(textbgci[i]);
  giza_set_character_height(ch[i]);
  giza_set_arrow_style(arrowfs[i],arrowangle[i],arrowcutback[i]);
  giza_set_font(font);
  giza_set_hatching_style(hatch_angle[i],hatch_spacing[i],hatch_phase[i]);
  giza_set_band_style(band_ls[i],band_lw[i]);

}

/**
 * Saves current plot settings
 *
 */
void
_giza_init_save (void)
{
  _giza_save_ncalls = 0;
  return;
}
