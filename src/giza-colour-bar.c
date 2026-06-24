/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING.LESSER file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2 of the LGPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the LGPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

#include "giza-private.h"
#include <giza.h>
#include <string.h>

/* Fraction of total width reserved for tick labels and units text. */
static double const annotation_fraction = 0.6;

/* Displacement of the units label from the wedge viewport edge (character heights). */
static double const label_separation = 2.2;

/* Grey ramp occupies this band on the cross-axis world window (plot-adjacent edge). */
static double const ramp_band_inner = 0.9;
static double const ramp_band_outer = 1.1;

static int
_giza_label_is_blank (const char *label)
{
  if (!label)
    return 1;
  while (*label)
    {
      if (*label != ' ' && *label != '\t')
        return 0;
      label++;
    }
  return 1;
}

/**
 * Drawing: giza_colour_bar
 *
 * Synopsis: Draws a colour bar (wedge) using the current colour ramp
 *
 * Input:
 *  -side    :- edge of viewport to draw colour bar relative to, either 'B' (bottom), 'T' (top), 'L' (left) or 'R' (right)
 *  -disp    :- displacement of the bar in character heights from the specified edge
 *  -width   :- total outward extent of the colour bar in character heights, including
 *              room for tick labels and an optional units label
 *  -valMin  :- The value in data that gets assigned the colour corresponding to zero on the
 *              colour ramp (The ramp is set by giza_set_colour_table)
 *  -valMax  :- The value in data that gets assigned the colour corresponding to one on the
 *              colour ramp
 *  -label   :- Text label to annotate colour bar with
 *
 * See Also: giza_render, giza_colour_bar_float, giza_set_colour_table
 */
void
giza_colour_bar (const char *side, double disp, double width,
                 double valMin, double valMax, const char *label)
{
  double xmin, xmax, ymin, ymax;
  double vptxmin, vptxmax, vptymin, vptymax;
  double xch, ych;
  double old_ch, wedge_ch, label_width_ch;
  double wedge_xmin, wedge_xmax, wedge_ymin, wedge_ymax;
  double ch_cross, ch_along;
  int npixwedg = 100;
  double sample[100];
  double vmin, vmax, dx;
  int i;
  int bottom = 0, top = 0, left = 0, right = 0;
  int greyscale = 0;
  double affine[6] = { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };

  if (!_giza_check_device_ready ("giza_colour_bar"))
    return;

  giza_get_window (&xmin, &xmax, &ymin, &ymax);
  giza_get_viewport (GIZA_UNITS_NORMALIZED, &vptxmin, &vptxmax, &vptymin, &vptymax);
  giza_get_character_size (GIZA_UNITS_NORMALIZED, &xch, &ych);
  giza_get_character_height (&old_ch);

  if (valMax < valMin)
    {
      vmin = valMax;
      vmax = valMin;
    }
  else
    {
      vmin = valMin;
      vmax = valMax;
    }

  dx = (vmax - vmin) / ((double) (npixwedg - 1));
  for (i = 0; i < npixwedg; i++)
    sample[i] = vmin + i * dx;

  wedge_ch = width * (1.0 - annotation_fraction);
  if (wedge_ch <= 0.)
    wedge_ch = width;

  label_width_ch = label_separation;
  if (!_giza_label_is_blank (label))
    label_width_ch += 1.0;

  giza_set_character_height (annotation_fraction * width * old_ch / label_width_ch);

  if (strchr (side, 'B') || strchr (side, 'b'))
    bottom = 1;
  else if (strchr (side, 'T') || strchr (side, 't'))
    top = 1;
  else if (strchr (side, 'L') || strchr (side, 'l'))
    left = 1;
  else if (strchr (side, 'R') || strchr (side, 'r'))
    right = 1;

  if (strchr (side, 'G') || strchr (side, 'g'))
    greyscale = 1;

  wedge_xmin = vptxmin;
  wedge_xmax = vptxmax;
  wedge_ymin = vptymin;
  wedge_ymax = vptymax;

  if (bottom || top)
    {
      ch_cross = ych;
      ch_along = ych;

      if (bottom)
        {
          wedge_ymax = wedge_ymin - disp * ch_along;
          wedge_ymin = wedge_ymax - wedge_ch * ch_cross;
        }
      else
        {
          wedge_ymin = wedge_ymax + disp * ch_along;
          wedge_ymax = wedge_ymin + wedge_ch * ch_cross;
        }

      giza_set_viewport (wedge_xmin, wedge_xmax, wedge_ymin, wedge_ymax);
      giza_set_window (1.0, (double) npixwedg, ramp_band_inner, ramp_band_outer);
      if (greyscale)
        giza_render_gray (npixwedg, 1, sample, 0, npixwedg - 1, 0, 0, valMin, valMax,
                         GIZA_EXTEND_PAD, GIZA_FILTER_DEFAULT, affine);
      else
        giza_render (npixwedg, 1, sample, 0, npixwedg - 1, 0, 0, valMin, valMax,
                     GIZA_EXTEND_PAD, GIZA_FILTER_DEFAULT, affine);

      giza_set_window (vmin, vmax, 0.0, 1.0);
      if (bottom)
        giza_box ("BCNST", 0.0, 0, "BC", 0.0, 0);
      else
        giza_box ("BCMST", 0.0, 0, "BC", 0.0, 0);
    }
  else if (left || right)
    {
      ch_cross = xch;
      ch_along = xch;

      if (left)
        {
          wedge_xmax = wedge_xmin - disp * ch_along;
          wedge_xmin = wedge_xmax - wedge_ch * ch_cross;
        }
      else
        {
          wedge_xmin = wedge_xmax + disp * ch_along;
          wedge_xmax = wedge_xmin + wedge_ch * ch_cross;
        }

      giza_set_viewport (wedge_xmin, wedge_xmax, wedge_ymin, wedge_ymax);
      giza_set_window (ramp_band_inner, ramp_band_outer, 1.0, (double) npixwedg);
      if (greyscale)
        giza_render_gray (1, npixwedg, sample, 0, 0, 0, npixwedg - 1, valMin, valMax,
                         GIZA_EXTEND_PAD, GIZA_FILTER_DEFAULT, affine);
      else
        giza_render (1, npixwedg, sample, 0, 0, 0, npixwedg - 1, valMin, valMax,
                     GIZA_EXTEND_PAD, GIZA_FILTER_DEFAULT, affine);

      giza_set_window (0.0, 1.0, vmin, vmax);
      if (left)
        giza_box ("BC", 0.0, 0, "BCNST", 0.0, 0);
      else
        giza_box ("BC", 0.0, 0, "BCMST", 0.0, 0);
    }

  if (!_giza_label_is_blank (label))
    giza_annotate (side, label_separation, 1.0, 1.0, label);

  giza_set_viewport (vptxmin, vptxmax, vptymin, vptymax);
  giza_set_window (xmin, xmax, ymin, ymax);
  giza_set_character_height (old_ch);

  giza_flush_device ();
}

/**
 * Drawing: giza_colour_bar_float
 *
 * Synopsis: Same functionality as giza_colour_bar, but takes floats
 *
 * See Also: giza_colour_bar
 */
void
giza_colour_bar_float (const char *side, float disp, float width,
                       float valMin, float valMax, const char *label)
{
  giza_colour_bar (side, (double) disp, (double) width,
                   (double) valMin, (double) valMax, label);
}
