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
#include "giza-drivers-private.h"
#include "giza-band-private.h"
#include <giza.h>
#include "math.h"
#define GIZA_MAX_BAND_MODES 8

/* storage of band settings, this is declared extern in the header file */
giza_band_t Band;

/**
 * Interactive: giza_band
 *
 * Synopsis: Returns the cursor position and character typed by the user
 * relative to an anchor point
 *
 * Input:
 *  -mode     :- selects the type of shape to draw during input
 *  -moveCurs :- if 1 the cursor is moved to (x, y),
 *               if 0 the cursor is not moved.
 *  -xanc     :- the x-coord of the anchor point.
 *  -yanc     :- the y-coord of the anchor point.
 *  -x        :- Gets set the position of the cursor.
 *  -y        :- Gets set to the position of the cursor.
 *  -ch       :- Gets set to the character pressed by the user.
 *
 * Return:
 *  -1 :- The device has no cursor
 *  -0 :- The call was successful
 *
 * Modes:
 *  -0 or GIZA_BAND_NONE      :- None, behaves like giza_get_key_press
 *  -1 or GIZA_BAND_LINE      :- Straight line drawn from anchor point
 *  -2 or GIZA_BAND_RECTANGLE :- Hollow rectangle
 *  -3 or GIZA_BAND_HORZLINES :- Two horizontal lines
 *  -4 or GIZA_BAND_VERTLINES :- Two vertical lines
 *  -5 or GIZA_BAND_HORZLINE  :- Single horizontal line, ignores anchor point
 *  -6 or GIZA_BAND_VERTLINE  :- Single vertical lines, ignores anchor point
 *  -7 or GIZA_BAND_CROSSHAIR :- Cross hair, ignores anchor point
 *  -8 or GIZA_BAND_CIRCLE    :- Circle centred on anchor point
 *
 * See Also: giza_get_key_press
 */
int
giza_band (int mode, int moveCurs, double xanc, double yanc, double *x, double *y, char *ch)
{
  if(!_giza_check_device_ready ("giza_band"))
    return 1;

  if (mode < 0 || mode > GIZA_MAX_BAND_MODES)
    {
      _giza_error("giza_band","Invalid band mode selected");
      return 2;
    }
  double xanca[1],yanca[1];
  xanca[0] = xanc;
  yanca[0] = yanc;
  return _giza_get_key_press (mode, moveCurs, 1, xanca, yanca, x, y, ch);
}

/**
 * Interactive: giza_band_float
 *
 * Synopsis: Same functionality as giza_band, but uses floats.
 *
 * See Also: giza_band
 */
int
giza_band_float (int mode, int moveCurs, float xanc, float yanc, float *x, float *y, char *ch)
{
  int success;
  double dx, dy;

  success = giza_band (mode, moveCurs, (double) xanc, (double) yanc, &dx, &dy, ch);
  *x = (float) dx;
  *y = (float) dy;

  return success;
}

/*
 * Draws over the old band and draws the new one.
 */
void
_giza_refresh_band (int mode, int nanc, const int *xanc, const int *yanc, int x2, int y2)
{
  if (mode <= 0 || mode > GIZA_MAX_BAND_MODES) return;
  if (nanc <= 0) return;

  /* Draw over the old band */
  cairo_paint (Band.restore);

/*
  int topleftx = x1 - 10;
  int toplefty = y1 - 10;
  int bottomrightx = x2 + 10;
  int bottomrighty = y2 + 10;
  if (x2 - x1 <= 0)
    {
      topleftx = x2 - 10;
      bottomrightx = x1 + 10;
    }
  if (y2 - y1 <= 0)
    {
      toplefty = y2 - 5;
      bottomrighty = y1 + 5;
    }
  cairo_rectangle (Band.box, x1, y1, x2 - x1, y2 - y1);
  cairo_rectangle (Band.restore, x1, y1, x2 - x1, y2 - y1);
  cairo_reset_clip (Band.box);
  cairo_reset_clip (Band.restore);
  cairo_clip (Band.box);
  cairo_clip (Band.restore);
 */
  int x1 = xanc[nanc-1];
  int y1 = yanc[nanc-1];
  int i,j;
 
  switch (mode)
    {
      case 1: /* Straight line */
       /* Draw the band */
        for (j=0;j<=1;j++) {
           if (j==0) {
              cairo_set_source_rgba (Band.box,1.,1.,1.,0.2);
           } else {
              cairo_set_source_rgba (Band.box, 0.6, 0.6, 0.6, 1.0);           
           }
	   cairo_move_to (Band.box, xanc[0], yanc[0]);
           for (i=1;i<=nanc-1;i++) {
	      cairo_line_to (Band.box, xanc[i], yanc[i]);
           }
	   /*cairo_move_to (Band.box, x1, y1);*/
	   cairo_line_to (Band.box, x2, y2);
           if (j==0) {
              cairo_close_path (Band.box);
              cairo_fill (Band.box);
           } else {
              cairo_stroke (Band.box);
           }
        }
	break;
      case 2: /* empty rectangle */
        /* Draw the band */
        cairo_set_source_rgba(Band.box,1.,1.,1.,0.2);
        cairo_rectangle (Band.box, x1, y1, x2 - x1, y2 - y1);
        cairo_fill(Band.box);
        cairo_set_source_rgba (Band.box, 0.6, 0.6, 0.6, 1.0);
        cairo_rectangle (Band.box, x1, y1, x2 - x1, y2 - y1);
        cairo_stroke(Band.box);
	break;
      case 3: /* Two horizontal lines */
        cairo_move_to (Band.box, 0., y1);
        cairo_line_to (Band.box, Band.maxWidth, y1);
        cairo_move_to (Band.box, 0., y2);
        cairo_line_to (Band.box, Band.maxWidth, y2);
        cairo_stroke (Band.box);
	break;
      case 4: /* Two vertical lines */
        cairo_move_to (Band.box, x1, 0.);
        cairo_line_to (Band.box, x1, Band.maxHeight);
        cairo_move_to (Band.box, x2, 0.);
        cairo_line_to (Band.box, x2, Band.maxHeight);
        cairo_stroke (Band.box);
	break;
      case 5: /* Single, horizontal line */
        cairo_move_to (Band.box, 0., y2);
        cairo_line_to (Band.box, Band.maxWidth, y2);
        cairo_stroke (Band.box);
	break;
      case 6: /* Single, vertical line */
        cairo_move_to (Band.box, x2, 0.);
        cairo_line_to (Band.box, x2, Band.maxHeight);
        cairo_stroke (Band.box);
	break;
      case 7: /* Crosshair */
        cairo_move_to (Band.box, 0., y2);
        cairo_line_to (Band.box, Band.maxWidth, y2);
        cairo_move_to (Band.box, x2, 0.);
        cairo_line_to (Band.box, x2, Band.maxHeight);
        cairo_stroke (Band.box);
	break;
      case 8: /* Circle */
        cairo_set_source_rgba(Band.box,1.,1.,1.,0.2);
        cairo_arc (Band.box, x1, y1, sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)), 0., 2 * M_PI);
        cairo_fill(Band.box);
        cairo_set_source_rgba (Band.box, 0.6, 0.6, 0.6, 1.0);
        cairo_arc (Band.box, x1, y1, sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)), 0., 2 * M_PI);
        cairo_stroke (Band.box);
	break;
      default:
        /* do not warn here (too verbose), instead warn in giza_band */
	break;
    }
}

/*
 * Cleans up the stuff associated with the band
 */
void
_giza_destroy_band (int mode)
{
  if (mode == 0) return;

  /* Free memory */
  cairo_destroy (Band.restore);
  cairo_destroy (Band.box);
  cairo_surface_destroy (Band.onscreen);

  /* remove the last band */
  giza_flush_device ();
}

