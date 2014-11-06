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
#include "giza-stroke-private.h"
#include "giza-transforms-private.h"
#include "giza-fill-private.h"
#include <giza.h>
#include <math.h>

/**
 * Settings: giza_set_fill
 *
 * Synopsis: Sets the current fill style.
 *
 * Input:
 *  -fs :- the new fill style
 *
 * Fill Styles:
 *  -1 or GIZA_FILL_SOLID       :- solid
 *  -2 or GIZA_FILL_HOLLOW      :- hollow
 *  -3 or GIZA_FILL_HATCH       :- hatch
 *  -4 or GIZA_FILL_CROSSHATCH  :- crosshatch
 */
void
giza_set_fill (int fs)
{
  if (!_giza_check_device_ready ("giza_set_fill"))
    return;
  if (fs < 1 || fs > GIZA_MAX_FILL_STYLES)
    {
      _giza_warning ("giza_set_fill", "Invalid fill style, fill style not set");
      return;
    }

  Dev[id].fs = fs;
}

/**
 * Settings: giza_get_fill
 *
 * Synopsis: Query the current fill style.
 *
 * Input:
 *  -fs :- gets set to the current fill style.
 */
void
giza_get_fill (int *fs)
{
  if (!_giza_check_device_ready ("giza_get_fill"))
    return;

  *fs = Dev[id].fs;
}

void
_giza_init_fill (void)
{
  Dev[id].fs = GIZA_FILL_SOLID;
  Dev[id].hatch_angle = 45.;
  Dev[id].hatch_spacing = 1.;
  Dev[id].hatch_phase = 0.;
}

/**
 * Fills the currently drawn path.
 */
void
_giza_fill (void)
{
  if(!_giza_check_device_ready ("_giza_fill"))
     return;

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);

  double ri,gi,bi,alphai;
  int ci;
  int hatch_size;
  double lw;
  double xoffset,dx,dy,dr;
  double pi = 3.1415926536;
  double xmin,xmax,ymin,ymax,x0,y0,x,y,angle;
  int i, nlines;

  switch (Dev[id].fs)
    {
    case GIZA_FILL_CROSSHATCH:
    case GIZA_FILL_HATCH:
      /* For hatched pattern we must initialise
       * the pattern every time fill is called,
       * because the hatching needs to be done
       * with the current colour index
       */
      hatch_size = (int) (8.*Dev[id].hatch_spacing);
      if (hatch_size <= 0)
        {
          _giza_error("giza_fill","hatch spacing <= 0, ignoring fill attributes");
          return;
        }
      lw = 1.5;
      angle = -pi*Dev[id].hatch_angle/180.;
      cairo_save(Dev[id].context);
      /* clip plotting to within the fill area
       * but do not (yet) destroy the fill area */
      cairo_clip_preserve(Dev[id].context);
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 4, 0)
      cairo_clip_extents(Dev[id].context, &xmin,&ymin,&xmax,&ymax);
#else
      xmin = 0.;
      xmax = Dev[id].width;
      ymin = 0.;
      ymax = Dev[id].height;
#endif
      dx = xmax - xmin;
      dy = ymax - ymin;
      /* set geometry of box */
      x0 = 0.5*(xmin + xmax);
      y0 = 0.5*(ymin + ymax);
      dr = sqrt(dx*dx + dy*dy);
      xmin = x0 - 0.5*dr;
      xmax = x0 + 0.5*dr;
      ymin = y0 - 0.5*dr;
      ymax = y0 + 0.5*dr;

      cairo_identity_matrix(Dev[id].context);
      /* fill with a transparent background */
      giza_get_colour_representation_alpha(GIZA_BACKGROUND_COLOUR,&ri,&gi,&bi,&alphai);
      cairo_set_source_rgba(Dev[id].context, ri, gi, bi, 0.);
      cairo_fill(Dev[id].context);

      /* draw lines making the pattern with current colour index */
      giza_get_colour_index(&ci);
      giza_get_colour_representation_alpha(ci,&ri,&gi,&bi,&alphai);
      cairo_set_source_rgba(Dev[id].context, ri, gi, bi, alphai);
      cairo_set_line_width(Dev[id].context, lw);

      xoffset = hatch_size*Dev[id].hatch_phase;
      nlines = (int) (xmax - xmin)/hatch_size + 1;
      if (Dev[id].fs == GIZA_FILL_CROSSHATCH)
        {
          /* draw vertical or / lines */
         for (i = 0; i <= nlines; i++)
            {
             x = xmin + (i-1)*hatch_size + xoffset;
             y = ymin;
             _giza_rotate_pos(&x,&y,angle,x0,y0);
             cairo_move_to(Dev[id].context, x, y);

             x = xmin + (i-1)*hatch_size + xoffset;
             y = ymax;
             _giza_rotate_pos(&x,&y,angle,x0,y0);
             cairo_line_to(Dev[id].context, x, y);
            }
        }

       /* draw horizontal or \ lines */
      for (i = 0; i <= nlines; i++)
        {
          x = xmin + xoffset;
          y = ymin + (i-1)*hatch_size;
          _giza_rotate_pos(&x,&y,angle,x0,y0);
          cairo_move_to(Dev[id].context, x, y);

          x = xmax + xoffset;
          y = ymin + (i-1)*hatch_size;
          _giza_rotate_pos(&x,&y,angle,x0,y0);
          cairo_line_to(Dev[id].context, x, y);
        }

      cairo_stroke(Dev[id].context);
      cairo_restore(Dev[id].context);
      break;
    case GIZA_FILL_SOLID:
      cairo_fill (Dev[id].context);
      break;
    case GIZA_FILL_HOLLOW:
      _giza_stroke ();
      break;
    default:
      _giza_warning ("_giza_fill", "Invalid fill style");
      _giza_stroke ();
      break;
    }

  _giza_set_trans (oldTrans);
}

void
_giza_rotate_pos (double *x, double *y, double angle, double x0, double y0)
{
   double xnew,ynew;
   double cosangle = cos(angle);
   double sinangle = sin(angle);
   xnew = (*x - x0)*cosangle - (*y - y0)*sinangle;
   ynew = (*x - x0)*sinangle + (*y - y0)*cosangle;
   *x = xnew + x0;
   *y = ynew + y0;
}


/**
 * Settings: giza_set_hatching_style
 *
 * Synopsis: Sets the hatching style when using GIZA_FILL_HATCH
 *
 * Input:
 *  -angle   :- the angle of the hatching pattern with respect to the horizontal
 *  -spacing :- the line spacing, in units of the default (1.0)
 *  -phase   :- a number between 0.0 and 1.0 specifying the offset
 *              along the horizontal axis to start the hatching lines
 *
 * See Also: giza_fill, giza_get_hatching_style
 */
void
giza_set_hatching_style (double angle, double spacing, double phase)
{
  if(!_giza_check_device_ready ("giza_set_hatching_style"))
     return;

  Dev[id].hatch_angle   = angle;
  Dev[id].hatch_spacing = spacing;
  Dev[id].hatch_phase   = phase;
}

/**
 * Settings: giza_set_hatching_style_float
 *
 * Synopsis: Same as giza_set_hatching_style, but takes floats
 *
 */
void
giza_set_hatching_style_float (float angle, float spacing, float phase)
{
  giza_set_hatching_style((double) angle, (double) spacing, (double) phase);
}

/**
 * Settings: giza_get_hatching_style
 *
 * Synopsis: Queries the current hatching style wgen using GIZA_FILL_HATCH
 *
 * Output:
 *  -angle   :- the angle of the hatching pattern with respect to the horizontal
 *  -spacing :- the line spacing, in units of the default (1.0)
 *  -phase   :- a number between 0.0 and 1.0 specifying the offset
 *              along the horizontal axis to start the hatching lines
 *
 * See Also: giza_fill, giza_set_hatching_style
 */
void
giza_get_hatching_style (double *angle, double *spacing, double *phase)
{
  if(!_giza_check_device_ready ("giza_get_hatching_style"))
     return;

  *angle   = Dev[id].hatch_angle;
  *spacing = Dev[id].hatch_spacing;
  *phase   = Dev[id].hatch_phase;
}

/**
 * Settings: giza_get_hatching_style_float
 *
 * Synopsis: Same as giza_get_hatching_style, but takes floats
 *
 */
void
giza_get_hatching_style_float (float *angle, float *spacing, float *phase)
{
  double dangle,dspacing,dphase;
  giza_get_hatching_style(&dangle, &dspacing, &dphase);
  *angle   = (float) dangle;
  *spacing = (float) dspacing;
  *phase   = (float) dphase;
}
