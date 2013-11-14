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
#include "giza-transforms-private.h"
#include "giza-window-private.h"
#include <string.h>
#include <giza.h>


/**
 * Text: giza_annotate
 *
 * Synopsis: Writes text with a position relative to the viewport.
 *
 * Input:
 *  -side          :- Must contain a character 'B','L','T'
 *                    or 'R' specifying the bottom, left,
 *                    top or right margin respectively. The
 *                    position of the text will be relative
 *                    to the specified side.
 *  -displacement   :- The displacement of the text from the
 *                    edge of the view-port specified in
 *                    side, measured out of the view-port in
 *                    units of character height.
 *  -coord         :- The location of the text along the edge
 *                    of the view port specified in side,
 *                    measured as a fraction of that size
 *  -justification :- Specifies the justification of the text.
 *                    0.0 means the left most edge of the string
 *                    will be aligned with coord, 1.0 means the
 *                    right most edge of the string will be
 *                    aligned with coord etc.
 *  -string        :- The text that will be displayed.
 */
void
giza_annotate (const char *side, double displacment, double coord,
	      double justification, const char *string)
{
  if (!_giza_check_device_ready ("giza_annotate"))
    {
      return;
    }
  if (!side)
    {
      _giza_warning ("giza_annotate",
		    "No side argument passed, skipping annotate");
      return;
    }

  int oldTrans = _giza_get_trans ();

  /* work out where to put the text (done in world coords) */
  _giza_set_trans (GIZA_TRANS_WORLD);

  double x = 0, y = 0, angle = 0;
  double xch, ych;
  giza_get_character_size (GIZA_UNITS_WORLD, &xch, &ych);

  giza_window_t Win = Dev[id].Win;

  /* check which side to draw on */
  /* if drawing along the bottom */
  if (strchr (side, 'B') || strchr (side, 'b'))
    {
      /* find the position on the x axis to place text */
      x = Win.xmin + (Win.xmax - Win.xmin) * coord;

      /* shift the position of the text away from the viewport by displacement */
      y = Win.ymin - (ych * displacment);
    }
  /* if drawing along the top */
  else if (strchr (side, 'T') || strchr (side, 't'))
    {
      /* find the position on the x axis to place text */
      x = Win.xmin + (Win.xmax - Win.xmin) * coord;

      /* shift the position of the text away from the viewport by displacement */
      y = Win.ymax + ych * (displacment - 0.5);
    }
  /* if drawing along the left */
  else if (strchr (side, 'L') || strchr (side, 'l'))
    {
      if (strchr (side, 'V') || strchr (side, 'v'))
	{
	  /* find the position on the y axis */
	  y = Win.ymin + (Win.ymax - Win.ymin) * coord - ych * 0.25;

	  /* find the distance from the y axis */
	  x = Win.xmin - (xch * displacment);

	  /* Set the angle */
	  angle = 0;
	}
      else
	{
	  /* find the position on the y axis to place text */
	  y = Win.ymin + (Win.ymax - Win.ymin) * coord;

	  /* shift the position of the text away from the viewport by displacement */
	  x = Win.xmin - (xch * displacment);

	  /* set the angle */
	  angle = 90;
	}
    }
  /* if drawing along the right */
  else if (strchr (side, 'R') || strchr (side, 'r'))
    {
      if (strchr (side, 'V') || strchr (side, 'v'))
	{
	  /* find the position on the y axis */
	  y = Win.ymin + (Win.ymax - Win.ymin) * coord - ych * 0.25;

	  /* find the distance from the y axis */
	  x = Win.xmax + (xch * displacment);

	  /* Set the angle */
	  angle = 0;
	}
      else
	{
	  /* find the position on the y axis to place text */
	  y = Win.ymin + (Win.ymax - Win.ymin) * coord;

	  /* shift the position of the text away from the view port by displacement */
	  x = Win.xmax + (xch * displacment);

	  /* set the angle */
	  angle = 90;
	}
    }
  else
    {
      _giza_warning ("giza_annotate",
		    "invalid side string, skipping annotate.");
      return;
    }

  giza_ptext (x, y, angle, justification, string);
  _giza_set_trans (oldTrans);

  giza_flush_device ();
}

/**
 * Text: giza_annotate_float
 *
 * Synopsis: The same functionality as giza_annotate but takes floats instead of doubles.
 *
 * See Also: giza_annotate
 */
void
giza_annotate_float (const char *side, float displacment, float coord,
		    float justification, const char *string)
{
  giza_annotate (side, (double) displacment, (double) coord,
		(double) justification, string);
}
