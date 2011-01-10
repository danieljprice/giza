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
#include "giza-transforms-private.h"
#include "giza-io-private.h"
#include "giza-line-style-private.h"
#include <giza.h>

int _giza_line_style, _giza_n;
double _giza_dashes[8];

/**
 * Settings: giza_set_line_style
 *
 * Synopsis: Sets the current line style.
 *
 * Input:
 *  -ls :- the new line style
 *
 * Line Styles:
 *  -GIZA_LS_SOLID            :- solid
 *  -GIZA_LS_LONG_DASH        :- long dash
 *  -GIZA_LS_SHORT_DASH       :- short dash
 *  -GIZA_LS_DOT              :- dot
 *  -GIZA_LS_DASH_DOT         :- dash dot
 *  -GIZA_LS_DASH_DOT_DOT_DOT :- dash dot dot dot
 *
 */
void
giza_set_line_style (int ls)
{
  if (!_giza_check_device_ready ("giza_set_fill"))
    return;
  
  _giza_set_line_style (ls, context);
  _giza_line_style = ls;
}

/**
 * Settings: giza_get_line_style
 *
 * Synopsis: Query the current line style.
 *
 * Input:
 *  -ls :- gets set to the current line style
 */
void
giza_get_line_style (int *ls)
{
  if (!_giza_check_device_ready ("giza_get_line_style"))
    return;
  *ls = _giza_line_style;
}

void
_giza_set_line_style (int ls, cairo_t *ct)
{
  double longDash, shortDash, lw;

  giza_get_character_size (2, &longDash, &shortDash);
  longDash = 0.85 * longDash;
  shortDash = 0.33 * longDash;

  switch (ls)
    {
    case GIZA_LS_SOLID:
      _giza_n= 0;
      break;
    case GIZA_LS_LONG_DASH:
      _giza_n= 2;
      _giza_dashes[0] = longDash;
      _giza_dashes[1] = 0.33 * longDash;
      break;
    case GIZA_LS_SHORT_DASH:
      _giza_n= 2;
      _giza_dashes[0] = shortDash;
      _giza_dashes[1] = 0.5 * shortDash;
      break;
    case GIZA_LS_DOT:
      _giza_n= 2;
      giza_get_line_width (&lw);
      _giza_dashes[0] = Dev.deviceUnitsPermm * 0.25 * lw;
      _giza_dashes[1] = Dev.deviceUnitsPermm * lw;
      break;
    case GIZA_LS_DASH_DOT:
      _giza_n = 4;
      giza_get_line_width (&lw);
      _giza_dashes[0] = longDash; // dash
      _giza_dashes[1] = Dev.deviceUnitsPermm * lw; // space
      _giza_dashes[2] = Dev.deviceUnitsPermm * 0.25 * lw; // dot
      _giza_dashes[3] = Dev.deviceUnitsPermm * lw; // space
      break;
    case GIZA_LS_DASH_DOT_DOT_DOT:
      _giza_n = 8;
      giza_get_line_width (&lw);
      _giza_dashes[0] = longDash; // dash
      _giza_dashes[1] = Dev.deviceUnitsPermm * lw; // space
      _giza_dashes[2] = Dev.deviceUnitsPermm * 0.25 * lw; // dot
      _giza_dashes[3] = Dev.deviceUnitsPermm * lw; // space
      _giza_dashes[4] = Dev.deviceUnitsPermm * 0.25 * lw; // dot
      _giza_dashes[5] = Dev.deviceUnitsPermm * lw; // space
      _giza_dashes[6] = Dev.deviceUnitsPermm * 0.25 * lw; // dot
      _giza_dashes[7] = Dev.deviceUnitsPermm * lw; // space
      break;
    default:
      _giza_warning ("giza_set_line_style", "Invalid line stlye, line style not set");
      return;
    }

  _giza_set_cairo_dashes (ct);
}

void
_giza_set_cairo_dashes (cairo_t *ct)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  cairo_set_dash (ct, _giza_dashes, _giza_n, 0.);

  _giza_set_trans (oldTrans);
}

void
_giza_init_line_style (void)
{
  _giza_line_style = GIZA_LS_SOLID;
}
