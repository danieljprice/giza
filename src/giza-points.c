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
#include "giza-transforms-private.h"
#include "giza-stroke-private.h"
#include "giza-io-private.h"
#include <giza.h>
#include <math.h>

/* triangle pointings */
#define GIZA_POINT_DOWN ((int)1)
#define GIZA_POINT_UP   ((int)-1)

/* Internal functions */
static void _giza_point        (double x, double y);
static void _giza_rect         (double x, double y, int fill, double scale);
static void _giza_rect_concave (double x, double y, int fill, double scale, double bulge_fraction);
static void _giza_plus         (double x, double y);
static void _giza_plus_size    (double x, double y, double size);
static void _giza_fat_plus     (double x, double y, int fill, double scale, double inset_fraction);
static void _giza_triangle     (double x, double y, int fill, int updown, float scale, float offset_fraction);
static void _giza_diamond      (double x, double y, int fill, double scale);
static void _giza_polygon      (double x, double y, int nsides , int fill, double scale);
static void _giza_star         (double x, double y, int npoints, double ratio, int fill, double scale);
static void _giza_star_line    (double x, double y, int npoints, double scale);
static void _giza_circle       (double x, double y);
static void _giza_circle_size  (double x, double y, double size, int fill);
static void _giza_cross        (double x, double y, double scale);
static void _giza_arrow        (double x, double y, double angle, double scale);
static void _giza_char         (int symbol, double x, double y);
static void _giza_drawchar     (const char *string, double x, double y);
static void _giza_start_draw_symbols (int *oldTrans, int *oldLineStyle, int *oldLineCap,
                                      double *oldLineWidth, double *oldCh);
static void _giza_end_draw_symbols   (int oldTrans, int oldLineStyle, int oldLineCap,
                                      double oldLineWidth, double oldCh);
static void _giza_draw_symbol (double xd, double yd, int symbol);

/* Stores the height of the markers */
static double markerHeight;

/**
 * Drawing: giza_points
 *
 * Synopsis: Plot n points at x[n], y[n] in world coords.
 *
 * Input:
 *  -n      :- the number of points
 *  -x      :- the x-coordinates of the points in world coords
 *  -y      :- the y-coordinates of the points in world coords
 *  -symbol :- the type of marker to use
 *
 * Symbols:
 *  -default :- small point (same as 1)
 *  -0       :- square
 *  -1       :- .
 *  -2       :- +
 *  -3       :- *
 *  -4       :- o
 *  -5       :- x
 *  ->31    :- from the Unicode table
 *
 */
void
giza_points (int n, const double* x, const double* y, int symbol)
{
  if (!_giza_check_device_ready ("giza_points"))
    return;
  if (n < 1) return;

  int oldTrans,oldLineStyle,oldLineCap;
  double oldLineWidth,oldCh;

  /* initialise symbol drawing */
  _giza_start_draw_symbols (&oldTrans,&oldLineStyle,&oldLineCap,&oldLineWidth,&oldCh);

  /* for each point find where to put each marker */
  int i;
  double xd, yd;
  for (i = 0; i < n; i++)
    {
      /* convert world coords to device coords */
      _giza_set_trans (GIZA_TRANS_WORLD);
      xd = x[i];
      yd = y[i];
      cairo_user_to_device (Dev[id].context, &xd, &yd);
      _giza_set_trans (GIZA_TRANS_IDEN);

      /* draw the symbol */
      _giza_draw_symbol(xd, yd, symbol);
    }

  _giza_stroke ();
  giza_flush_device ();

  /* restore old setting */
  _giza_end_draw_symbols (oldTrans,oldLineStyle,oldLineCap,oldLineWidth,oldCh);
}

/**
 * Drawing: giza_points_float
 *
 * Synopsis: Same functionality as giza_points but takes floats.
 *
 * See Also: giza_points
 */
void
giza_points_float (int n, const float* x, const float* y, int symbol)
{
  if (!_giza_check_device_ready ("giza_points"))
    return;
  if (n < 1) return;

  int oldTrans,oldLineStyle,oldLineCap;
  double oldLineWidth,oldCh;

  /* initialise symbol drawing */
  _giza_start_draw_symbols (&oldTrans,&oldLineStyle,&oldLineCap,&oldLineWidth,&oldCh);

  /* for each point find where to put each marker */
  double xd, yd;
  int i;
  for (i = 0; i < n; i++)
    {
      xd = (double) x[i];
      yd = (double) y[i];

      /* convert world coords to device coords */
      _giza_set_trans (GIZA_TRANS_WORLD);
      cairo_user_to_device (Dev[id].context, &xd, &yd);
      _giza_set_trans (GIZA_TRANS_IDEN);

      /* draw the symbol */
      _giza_draw_symbol(xd, yd, symbol);
    }

  giza_end_buffer ();
  _giza_stroke ();
  giza_flush_device ();

  /* restore old settings */
  _giza_end_draw_symbols (oldTrans,oldLineStyle,oldLineCap,oldLineWidth,oldCh);
}

/**
 * Drawing: giza_single_point
 *
 * Synopsis: Plots a single point at x, y in world coords.
 *
 * Input:
 *  -x      :- the x-coordinate of the point in world coords
 *  -y      :- the y-coordinate of the point in world coords
 *  -symbol :- the type of marker to use
 *
 */
void
giza_single_point (double x, double y, int symbol)
{
  double xpts[1], ypts[1];
  xpts[0] = x;
  ypts[0] = y;
  giza_points (1, xpts, ypts, symbol);
}

/**
 * Drawing: giza_single_point_float
 *
 * Synopsis: Same functionality as giza_single_point,
 * but uses floats.
 *
 * See Also: giza_single_point
 */
void
giza_single_point_float (float x, float y, int symbol)
{
  double xpts[1], ypts[1];
  xpts[0] = (double) x;
  ypts[0] = (double) y;
  giza_points (1, xpts, ypts, symbol);
}

/**
 * DJP: Internal routine to draw symbol at a given position
 *      in device coordinates
 */
void
_giza_draw_symbol_device (double xd, double yd, int symbol)
{
  int oldTrans,oldLineStyle,oldLineCap;
  double oldLineWidth,oldCh;

  /* store existing font matrix */
  cairo_matrix_t mat;
  cairo_get_font_matrix (Dev[id].context, &mat);

  /* initialise symbol drawing */
  _giza_start_draw_symbols (&oldTrans,&oldLineStyle,&oldLineCap,&oldLineWidth,&oldCh);

  /* for each point find where to put each marker */
  _giza_draw_symbol(xd, yd, symbol);

  _giza_stroke ();
  giza_flush_device ();

  /* restore old setting */
  _giza_end_draw_symbols (oldTrans,oldLineStyle,oldLineCap,oldLineWidth,oldCh);

   /* restore font matrix */
   cairo_set_font_matrix (Dev[id].context, &mat);
}


/**
 * DJP: Internal routine to initialise drawing of symbols
 *      (mainly to avoid repeated code in float/double versions)
 */
void
_giza_start_draw_symbols (int *oldTrans, int *oldLineStyle, int *oldLineCap,
                          double *oldLineWidth, double *oldCh)
{
  /* store the old trans and line width */
  *oldTrans = _giza_get_trans ();
  giza_get_line_style       (oldLineStyle);
  giza_get_line_cap         (oldLineCap);
  giza_get_line_width       (oldLineWidth);
  giza_get_character_height (oldCh);

  /* Set the height for manually drawn markers */
  _giza_get_markerheight(&markerHeight);

  /* Set the line width for manually drawn markers */
  /*_giza_set_trans (GIZA_TRANS_IDEN); */

  giza_set_line_width( 0.75 * *oldLineWidth );
  giza_set_line_style(1);
  giza_set_line_cap(0);

  /* Set the character size to draw the Unicode markers at */
  giza_set_character_height (*oldCh * 0.8);

}

void
_giza_end_draw_symbols (int oldTrans, int oldLineStyle, int oldLineCap,
                        double oldLineWidth, double oldCh)
{
  /* restore old settings */
  _giza_set_trans (oldTrans);
  giza_set_line_style       (oldLineStyle);
  giza_set_line_cap         (oldLineCap);
  giza_set_line_width       (oldLineWidth);
  giza_set_character_height (oldCh);
}
/**
 * DJP: Internal routine to draw a single point,
 * assuming initialisation routines have been called
 * (so cannot be called externally)
 *
 * This is mainly to avoid repeated code in the
 *  float and double routines
 *
 * Input:
 *  -x, y   :- the coordinate of the point in USER coords
 *  -symbol :- the type of marker to use
 */
void
_giza_draw_symbol (double xd, double yd, int symbol)
{

  /* Use Unicode to draw the marker */
  if (symbol > 31)
    {
      _giza_char (symbol, xd, yd);
    }
  /* Manually draw the marker */
  else
    {
      switch (symbol)
        {
        case 31: /* down arrow */
          _giza_arrow (xd, yd, 0.5*M_PI, 2.5);
          break;
        case 30: /* up arrow */
          _giza_arrow (xd, yd, -0.5*M_PI, 2.5);
          break;
        case 29: /* right arrow */
          _giza_arrow (xd, yd, 0., 2.5);
          break;
        case 28: /* left arrow */
          _giza_arrow (xd, yd, M_PI, 2.5);
          break;
        case 27: /* hollow circles of various sizes */
        case 26:
        case 25:
        case 24:
        case 23:
        case 22:
        case 21:
        case 20:
        case 17: /* MV: filled circle that should scale w/ character height too */
          {
              /* It looks the open circles do not follow an exact linear scale 
               * from 19 -> 27. In stead we make scale from 0.3 -> 7 in 8
               * exponentional steps (27 - 19 = 8)
               */
              const double  minScale = 0.3, maxScale = 7, nStep = 27 - 19;
              const double  factor   = pow(maxScale / minScale, 1./nStep);
              _giza_circle_size (xd, yd, /*size:*/(symbol==17) ? Dev[id].ch/2.0 : minScale * pow(factor, symbol-19), /*fill:*/symbol==17 );
          }
          break;
        case 15: /* hollow up+down triangle, where we do slightly different raises on the the up/down triangle */
          _giza_triangle(xd, yd, 0, GIZA_POINT_UP,   1.0, 0.5);
          _giza_triangle(xd, yd, 0, GIZA_POINT_DOWN, 1.0, 0.5);
          break;
        case 14: /* open plus sign */
          _giza_fat_plus(xd, yd, 0, 2.0, 0.5);
          break;
        case 13: /* solid triangle */
          _giza_triangle(xd, yd, 1, GIZA_POINT_UP, 0.7, 1);
          break;
        case 12: /* five-pointed star */
        case 18: /* Filled version of symbol 12 (five-pointed star) */
          _giza_star (xd, yd, 5, 0.4, /*fill:*/symbol==18, /*scale: sym#12 > sym#18*/(symbol==12 ? 2.0 : 1.75));
          break;
        case 11: /* hollow diamond */
          _giza_diamond (xd, yd, /*fill:*/0, 1.25*Dev[id].ch);
          break;
        case 10: /* square with concave sides, slightly larger than default rect */
          _giza_rect_concave(xd, yd, 0, 1.8, .3);
          break;
        case 9: /* circle with small dot (like Sun symbol) */
          _giza_circle_size(xd, yd, 0.2*Dev[id].ch,  1);
          _giza_circle_size(xd, yd, Dev[id].ch, 0);
          break;
        case 8: /* circle and plus */
          _giza_circle_size (xd, yd, Dev[id].ch, 0);
          _giza_plus_size (xd, yd, Dev[id].ch);
          break;
        case 7: /* hollow upward-pointing triangle */
          _giza_triangle(xd, yd, 0, GIZA_POINT_UP, 0.7, 1);
          break;
        case 5: /* cross (x) */
          _giza_cross (xd, yd, 1.25*Dev[id].ch);
          break;
        case 4: /* hollow circle */
          _giza_circle_size (xd, yd, Dev[id].ch, 0);
          break;
        case 3: /* asterisk */
          _giza_star_line(xd, yd, 3/*nsides*/, 1.25 * sqrt(Dev[id].ch));
          break;
        case 2: /* plus */
          _giza_plus_size (xd, yd, Dev[id].ch);
          break;
        case 1: /* single small point that scales w/ character height */
          /*_giza_circle_size (xd, yd, 0.5, 1);*/
          _giza_point (xd, yd);
          break;
        case 19: /* slightly larger open rect (just shy of 3x size of #6 */
        case 16: /* filled square */
        case 6: /* hollow square */
        case 0: /*      id.      */
          _giza_rect (xd, yd, /*fill:*/symbol==16, /*scale:*/((symbol == 19) ? 3.5 : (symbol == 16 ? 1.25 : 1.5)) );
          break;
        case -1: /* single small point */
        case -2:
          _giza_point (xd, yd);
          break;
        case -3: /* solid polygons */
        case -4:
        case -5:
        case -6:
        case -7:
        case -8:
          /* scale those up a bit compared to original code */
          _giza_polygon (xd, yd, -symbol, 1, 2.5);
          break;
        default:
          _giza_point (xd, yd);
          break;
        }
    }
}

/**
 * Draws a small circle at x, y
 */
static void
_giza_point (double x, double y)
{
  cairo_arc (Dev[id].context, x, y, 1.0, 0., 2.* M_PI);
  cairo_fill (Dev[id].context);
  _giza_stroke();
}

/**
 * Draw a rectangle centred at x, y
 */
static void
_giza_rect (double x, double y, int fill, double scale)
{
  const double size = scale * markerHeight;

  cairo_rectangle (Dev[id].context, x - 0.5 * size, y - 0.5 * size, size, size );
  if (fill) { cairo_fill(Dev[id].context); }
  cairo_stroke( Dev[id].context );
}

/**
 * Draw a rectangle with concave sides centred at x, y. 
 * The principle size of of the rect is "0.5 * markerHeight * scale"
 * The 'dent' is bulge_fraction * the size of the square
 */
static void
_giza_rect_concave (double x, double y, int fill, double scale, double bulge_fraction)
{
  /* compute radius of circle, start/end angles and center */
  const double dx = scale * 0.8 * markerHeight;
  const double dy = bulge_fraction * dx;
  const double beta = atan( dy/dx ), two_beta = 2 * beta;
  const double one_over_tan2beta = 1. / tan(2*beta);
  const double R      = dy + dx * one_over_tan2beta;
  const double center = dx * (1 + one_over_tan2beta);

  cairo_arc(Dev[id].context, x + center, y, R, M_PI     - two_beta, M_PI     + two_beta);
  cairo_new_sub_path(Dev[id].context);
  cairo_arc(Dev[id].context, x - center, y, R, 2*M_PI   - two_beta, 2*M_PI   + two_beta);
  cairo_new_sub_path(Dev[id].context);
  cairo_arc(Dev[id].context, x, y - center, R, M_PI_2   - two_beta, M_PI_2   + two_beta);
  cairo_new_sub_path(Dev[id].context);
  cairo_arc(Dev[id].context, x, y + center, R, 3*M_PI_2 - two_beta, 3*M_PI_2 + two_beta);
  if (fill) { cairo_fill(Dev[id].context); }
  cairo_stroke(Dev[id].context);
}

/**
 * Draw a plus centred at x, y
 */
static void
_giza_plus (double x, double y)
{
  cairo_move_to (Dev[id].context, x - markerHeight * 0.8, y);
  cairo_line_to (Dev[id].context, x + markerHeight * 0.8, y);
  cairo_move_to (Dev[id].context, x, y - markerHeight * 0.8);
  cairo_line_to (Dev[id].context, x, y + markerHeight * 0.8);
  _giza_stroke ();
}

/**
 * Draw a plus centred at x, y
 */
static void
_giza_plus_size (double x, double y, double size)
{
  cairo_move_to (Dev[id].context, x - markerHeight * 0.8 * size, y);
  cairo_line_to (Dev[id].context, x + markerHeight * 0.8 * size, y);
  cairo_move_to (Dev[id].context, x, y - markerHeight * 0.8 * size);
  cairo_line_to (Dev[id].context, x, y + markerHeight * 0.8 * size);
  _giza_stroke ();
}

/**
 * Draw a 'fat' plus centred at x, y
 * principal size = 0.5 * markerHeigth,
 * this symbol's real size = principal size scaled by 'scale'
 * The insets are inset_fraction * real size
 */
static void
_giza_fat_plus (double x, double y, int fill, double scale, double inset_fraction)
{
  const double side  = 0.6 * markerHeight * scale;
  const double inset = inset_fraction * side, outset = side - inset;

  cairo_move_to     (Dev[id].context, x - side, y - outset/2);
  cairo_rel_line_to (Dev[id].context, 0       , outset); /* up */
  cairo_rel_line_to (Dev[id].context, inset   , 0);      /* right */
  cairo_rel_line_to (Dev[id].context, 0       , inset);  /* up */
  cairo_rel_line_to (Dev[id].context, outset  , 0);      /* right */
  cairo_rel_line_to (Dev[id].context, 0       , -inset); /* down */
  cairo_rel_line_to (Dev[id].context, inset   , 0);      /* right */
  cairo_rel_line_to (Dev[id].context, 0       , -outset);/* down */
  cairo_rel_line_to (Dev[id].context, -inset  , 0);      /* left */
  cairo_rel_line_to (Dev[id].context, 0       , -inset); /* down */
  cairo_rel_line_to (Dev[id].context, -outset , 0);      /* left */
  cairo_rel_line_to (Dev[id].context, 0       , inset);  /* up */
  cairo_rel_line_to (Dev[id].context, -inset  , 0);      /* left */
  if (fill) { cairo_fill(Dev[id].context); }
  cairo_stroke(  Dev[id].context );
}

/**
 * Draws a hollow circle at x, y
 */
static void
_giza_circle (double x, double y)
{
  cairo_move_to(Dev[id].context, x + markerHeight*0.5, y);
  cairo_arc (Dev[id].context, x, y, markerHeight * 0.5, 0., 2. * M_PI);
  _giza_stroke ();
}

/**
 * Draws a hollow circle at x, y, with size and fill arguments
 * size is in units of 'canonical symbol size' (== 0.5 * markerHeight)
 */
static void
_giza_circle_size (double x, double y, double size, int fill)
{
  cairo_arc (Dev[id].context, x, y, size * markerHeight * 0.8, 0., 2. * M_PI);
  if (fill) { cairo_fill(Dev[id].context); }
  cairo_stroke ( Dev[id].context );
}

/**
 * Draws a downward pointing triangle at x, y, either hollow or solid,
 * pointing UP (updown=-1) or DOWN (updown=+1), with isosceles sides 
 * of size scale * markerHeight, with the baseline offset scaled
 * by offset_fraction (offset_fraction = 1 => isosceles triangle
 * with center x,y, otherwise the triangle is displaced by offset_fraction
 * in the y direction)
 */
static void
_giza_triangle(double x, double y, int fill, int updown, float scale, float offset_fraction)
{
  cairo_move_to (Dev[id].context, x - markerHeight * scale, y - offset_fraction * updown * markerHeight * scale);
  cairo_line_to (Dev[id].context, x + markerHeight * scale, y - offset_fraction * updown * markerHeight * scale);
  cairo_line_to (Dev[id].context, x, y + updown * markerHeight * scale);
  cairo_close_path (Dev[id].context);
  if (fill) { cairo_fill(Dev[id].context); }
  cairo_stroke (Dev[id].context);
}

/**
 * Draws a diamond at x, y, either hollow or solid
 */
static void
_giza_diamond(double x, double y, int fill, double scale)
{
  cairo_move_to (Dev[id].context, x - scale*markerHeight*0.4/*0.5*/, y );
  cairo_line_to (Dev[id].context, x, y + scale*markerHeight * 0.75/*0.625*/ );
  cairo_line_to (Dev[id].context, x + scale*markerHeight*0.4/*0.5*/, y );
  cairo_line_to (Dev[id].context, x, y - scale*markerHeight * 0.75/*0.625*/ );
  cairo_close_path (Dev[id].context);
  if (fill) { cairo_fill(Dev[id].context); }
  _giza_stroke ();
}


/**
 * Draws a cross at x, y
 */
static void
_giza_cross (double x, double y, double scale)
{
  const double dx = (1./sqrt(2.))*markerHeight*scale;
  cairo_move_to (Dev[id].context, x, y);
  cairo_rel_move_to (Dev[id].context, -dx * 0.5, -dx * 0.5);
  cairo_rel_line_to (Dev[id].context, dx, dx);
  cairo_rel_move_to (Dev[id].context, -dx, 0);
  cairo_rel_line_to (Dev[id].context, dx, -dx);
  _giza_stroke ();
}

/**
 * Draws an arrow at x, y at scale 'scale'.
 * Unit of scale is '0.5 * markerHeight' for the
 * length of the arrow.
 */
static void
_giza_arrow (double x, double y, double angle, double scale)
{
  const double r = scale * 0.5 * markerHeight;
  const double headwidth  = 0.5 * r;
  const double headlength = 0.5 * r;
  const double cosa = cos(angle), sina = sin(angle);
  cairo_move_to (Dev[id].context, x - r*cosa, y - r*sina);
  cairo_line_to (Dev[id].context, x + r*cosa, y + r*sina);
  cairo_rel_line_to (Dev[id].context, - headlength*cosa + headwidth*sina, headwidth*cosa - headlength*sina);
  cairo_move_to (Dev[id].context, x + r*cosa, y + r*sina);
  cairo_rel_line_to (Dev[id].context, - headlength*cosa - headwidth*sina, -headwidth*cosa - headlength*sina);
  _giza_stroke ();
}


/**
 * Draws a general polygon at x, y with nsides sides at scale scale.
 * Units of scale are 'normalized symbol size' = 0.5 * markerHeight
 */
static void
_giza_polygon (double x, double y, int nsides, int fill, double scale)
{
 /* Define radius */
 double r = scale * 0.5 * markerHeight;

 /* Set first vertex above marker position */
 double alpha = 1.5 * M_PI;
 double cosalpha = cos(alpha);
 double sinalpha = sin(alpha);
 cairo_move_to (Dev[id].context, x + r * cosalpha, y + r * sinalpha);

 /* Define other vertexes */
 double alpha_step = 2. * M_PI / ((double) nsides);
 int i;
 for (i = 1; i < nsides; i++)
 {
  alpha += alpha_step;
  cosalpha = cos(alpha);
  sinalpha = sin(alpha);
  cairo_line_to (Dev[id].context, x + r * cosalpha, y + r * sinalpha);
 }
 cairo_close_path(Dev[id].context);
 if (fill) { cairo_fill(Dev[id].context); }
 _giza_stroke ();

}

/**
 * Draws an n-pointed star at x,y
 */
static void
_giza_star (double x, double y, int npoints, double ratio, int fill, double scale)
{
 /* Define outer and inner radius */
 double r = 0.8 * markerHeight * scale;
 double ri = ratio * r;

 /* Set first vertex so that shape appears flat-bottomed */
 double alpha = (0.5 + 1./npoints)* M_PI;
 double cosalpha = cos(alpha);
 double sinalpha = sin(alpha);
 /* Define other vertexes */
 double alpha_step = 2 * M_PI / npoints;
 int i;

 cairo_move_to (Dev[id].context, x + r * cosalpha, y + r * sinalpha);

 for (i = 1; i < npoints; i++)
 {
  alpha += 0.5*alpha_step;
  cosalpha = cos(alpha);
  sinalpha = sin(alpha);
  cairo_line_to (Dev[id].context, x + ri * cosalpha, y + ri * sinalpha);
  alpha += 0.5*alpha_step;
  cosalpha = cos(alpha);
  sinalpha = sin(alpha);
  cairo_line_to (Dev[id].context, x + r * cosalpha, y + r * sinalpha);
 }
 alpha += 0.5*alpha_step;
 cosalpha = cos(alpha);
 sinalpha = sin(alpha);
 cairo_line_to (Dev[id].context, x + ri * cosalpha, y + ri * sinalpha);
 cairo_close_path(Dev[id].context);
 if (fill) { cairo_fill(Dev[id].context); }

 cairo_stroke ( Dev[id].context );
}

/*
 * draw an n-line star (like polygon, only not drawing the edges but the
 * connections to points half a circle away.
 */
static void
_giza_star_line (double x, double y, int nsides, double scale)
{
 /* Define radius */
 double r = scale * 0.5 * markerHeight;

 /* Set first vertex above marker position */
 double alpha = 1.5 * M_PI, alpha_step = M_PI / ((double)nsides);
 double dx = r * cos(alpha);
 double dy = r * sin(alpha);

 cairo_move_to (Dev[id].context, x + dx, y + dy);

 /* Define other vertices */
 int i;
 for (i = 0; i < nsides; i++)
 {
   /* first line to other vertex */
   cairo_line_to(Dev[id].context, x - dx, y - dy);
   alpha += alpha_step;
   dx     = r * cos(alpha);
   dy     = r * sin(alpha);
   cairo_move_to (Dev[id].context, x + dx, y + dy);
 }
 _giza_stroke ();
}

/**
 * Draws the Unicode symbol at x, y.
 * NOTE: Only supports ASCII now.
 */
static void
_giza_char (int symbol, double x, double y)
{
  char str[4];
/*  cairo_text_extents_t extents; */
  if (symbol <= 127)
    {
      str[0] = (char) symbol;
      str[1] = '\0';
    }
  else
    {
      str[0] = 'A';
      str[1] = '\0';
    }
  _giza_drawchar(str,x,y);
}

/**
 * Draws a character at x, y.
 */
static void
_giza_drawchar (const char *str, double x, double y)
{
  cairo_text_extents_t extents;
  cairo_text_extents (Dev[id].context, str, &extents);
  /*
   * use information about the glyph to centre
   * the character on the x, y position
   */
  cairo_move_to (Dev[id].context, x - 0.5*extents.width  - extents.x_bearing,
                          y - 0.5*extents.height - extents.y_bearing);
  cairo_show_text (Dev[id].context, str);
}

void
_giza_get_markerheight (double *mheight)
{
  *mheight = Dev[id].fontExtents.max_x_advance * 0.2;
  return;
}
