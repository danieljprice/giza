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
 * Copyright (C) 2010-2011 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-stroke-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <math.h>

#define GIZA_FILL_SOLID  1
#define GIZA_FILL_HOLLOW 2
#define GIZA_FILL_HATCH 3
#define GIZA_FILL_CROSSHATCH 4

#define GIZA_MAX_FILL_STYLES 4

void _giza_rotate_pos (double *x, double *y, double angle, double x0, double y0);

int _giza_fill_style;
double _giza_hatch_angle;
double _giza_hatch_spacing;
double _giza_hatch_phase;

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

  _giza_fill_style = fs;
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

  *fs = _giza_fill_style;
}

void
_giza_init_fill (void)
{
  _giza_fill_style = GIZA_FILL_SOLID;
  _giza_hatch_angle = 45.;
  _giza_hatch_spacing = 1.;
  _giza_hatch_phase = 0.;
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
  int lw;
  double dx,sizex,sizey;
  double pi = 3.1415926536;
  double xmin,xmax,ymin,ymax,x0,y0,x,y,angle;
  int i, nlines;

  switch (_giza_fill_style)
    {
    case GIZA_FILL_CROSSHATCH:
    case GIZA_FILL_HATCH:
      /* For hatched pattern we must initialise
       * the pattern every time fill is called,
       * because the hatching needs to be done 
       * with the current colour index
       */
      hatch_size = (int) 8.*_giza_hatch_spacing;
      if (hatch_size <= 0)
        {
          _giza_error("giza_fill","hatch spacing <= 0, ignoring fill attributes");
          return;
        }
      lw = 1.5;
      angle = -pi*_giza_hatch_angle/180.;
      cairo_save(context);
      /* clip plotting to within the fill area
       * but do not (yet) destroy the fill area */
      cairo_clip_preserve(context);
      cairo_clip_extents(context, &xmin,&ymin,&xmax,&ymax);
      sizex = xmax - xmin;
      sizey = ymax - ymin;
      x0 = 0.5*(xmin + xmax);
      y0 = 0.5*(ymin + ymax);
     
      cairo_identity_matrix(context);
      /* fill with a transparent background */
      giza_get_colour_representation_alpha(GIZA_BACKGROUND_COLOUR,&ri,&gi,&bi,&alphai);
      cairo_set_source_rgba(context, ri, gi, bi, 0.);
      cairo_fill(context);

      /* draw lines making the pattern with current colour index */
      giza_get_colour_index(&ci);
      giza_get_colour_representation_alpha(ci,&ri,&gi,&bi,&alphai);
      cairo_set_source_rgba(context, ri, gi, bi, alphai);
      cairo_set_line_width(context, lw);
      
      dx = hatch_size*_giza_hatch_phase;
      nlines = 3*(int) (xmax - xmin)/hatch_size + 1;
      if (_giza_fill_style == GIZA_FILL_CROSSHATCH) 
        {
          //cairo_set_antialias(context, CAIRO_ANTIALIAS_NONE);    

          /* draw vertical or / lines */
         for (i = 0; i <= nlines; i++)
            {
             x = xmin + (i-1)*hatch_size + dx;
             y = ymin;
             _giza_rotate_pos(&x,&y,angle,x0,y0);
             cairo_move_to(context, x, y);

             x = xmin + (i-1)*hatch_size + dx;
             y = ymax;
             _giza_rotate_pos(&x,&y,angle,x0,y0);
             cairo_line_to(context, x, y);
            }

        } else {
          /* the antialiasing causes a dashed look in the hatched pattern
           * that is fixed by turning it off - there should be a better 
           * way but this is a workaround for the fact that cairo
           * renders the pattern pixmap before it is finally
           * rendered to the surface */
        }
          /* draw horizontal or \ lines */
         for (i = 0; i <= nlines; i++)
            {
             x = xmin;
             y = ymin + (i-1)*hatch_size;
             _giza_rotate_pos(&x,&y,angle,x0,y0);
             cairo_move_to(context, x, y);

             x = xmax;
             y = ymin + (i-1)*hatch_size;
             _giza_rotate_pos(&x,&y,angle,x0,y0);
             cairo_line_to(context, x, y);
            }
      
      cairo_stroke(context);
      cairo_restore(context);
      break;
    case GIZA_FILL_SOLID:
      cairo_fill (context);
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
   xnew = (*x - x0)*cos(angle) - (*y - y0)*sin(angle);
   ynew = (*x - x0)*sin(angle) + (*y - y0)*cos(angle);
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
  _giza_hatch_angle = angle;
  _giza_hatch_spacing = spacing;
  _giza_hatch_phase = phase;
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
  _giza_hatch_angle = (double) angle;
  _giza_hatch_spacing = (double) spacing;
  _giza_hatch_phase = (double) phase;
//  printf(" got angle = %f spacing = %f phase = %f \n",_giza_hatch_angle,_giza_hatch_spacing,_giza_hatch_phase);
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
  *angle = _giza_hatch_angle;
  *spacing = _giza_hatch_spacing;
  *phase = _giza_hatch_phase;
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
  *angle = (float) _giza_hatch_angle;
  *spacing = (float) _giza_hatch_spacing;
  *phase = (float) _giza_hatch_phase;
}
