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
#include "giza-transforms-private.h"
#include "giza-io-private.h"
#include "giza-stroke-private.h"
#include "giza-window-private.h"
#include "giza-viewport-private.h"
#include "giza-drivers-private.h"
#include "giza.h"
#include <math.h>
#include <stdio.h>

static void _giza_tick_intervals (double xmin, double xmax, double xinterval,
				  int *i1, int *i2);

/**
 * Drawing: giza_box
 *
 * Synopsis: Annotates the viewport with labelled axis/frame
 *
 * Input:
 *  -xopt  :- String of options for the x-axis.
 *            The options may be in any order. See
 *            below for details
 *  -xtick :- The distance, in world coordinates,
 *            between major ticks on the x-axis.
 *            If 0.0 the interval is chosen
 *            by giza_box.
 *  -nxsub :- The number of minor ticks to be placed
 *            between each major tick. If 0 the
 *            number is chosen by giza_box.
 *  -yopt  :- Similar to xopt but for the y-axis.
 *  -ytick :- Similar to xtick but for the y-axis.
 *  -nysub :- Similar to nxsub but for the y-axis.
 *
 * Options:
 *  -A :- Draw the axis.
 *  -B :- Draw the bottom or left edge of the frame.
 *  -C :- Draw the top or right edge of the frame.
 *  -T :- Draw major ticks.
 *  -S :- Draw minor ticks.
 *  -N :- Label the axis.
 *  -V :- Orient numeric y labels vertically (only
 *        applies to left and right axis).
 *  -G :- Draw grid lines at major intervals
 *  -M :- Write numeric labels above x-axis or to right of y-axis
 *        instead of usual position
 *  -L :- Label axis logarithmically
 */
void
giza_box (const char *xopt, double xtick, int nxsub,
	  const char *yopt, double ytick, int nysub)
{
  if (!_giza_check_device_ready ("giza_box"))
    return;

  _giza_expand_clipping ();

  int oldBuf;
  giza_get_buffering(&oldBuf);

  /* Table of log10() values for log axis ticks */
  double logTab[9];
  int k;
  for (k = 0; k < 9; k++)
    {
      logTab[k] = log10 (k + 1);
    }

  /* Begin buffering */
  giza_begin_buffer ();

  int xdraw_axis = 0, xdraw_bottom = 0, xdraw_top = 0, xdraw_majticks = 0, xdraw_minticks = 0, xdraw_labels =
    0, xdraw_labels_ontop = 0, xdraw_log = 0, xdraw_grid = 0;

  int ydraw_axis = 0, ydraw_left = 0, ydraw_right = 0, ydraw_majticks = 0, ydraw_minticks = 0, ydraw_labels =
    0, ydraw_labels_toright = 0, ydraw_vertical = 0, ydraw_log = 0, ydraw_grid = 0;

  double xintervalMaj, xintervalMin, xval, xratio;
  double yintervalMaj, yintervalMin, yval, yratio;
  int nv, np;
  int nMinTicksx, major;
  int nMinTicksy;
  double majTickL, subTickL, currentTickL;
  char tmp[100];
  int i, i1, i2, j, jmax, jtmp;
  giza_window_t Win = Dev[id].Win;

  /* set x-options */
  for (i = 0; xopt[i]; i++)
    {
      switch (xopt[i])
	{
	case ('a'):
	case ('A'):
	  /* Check the axis will be inside the box! */
	  if (Win.ymin < 0 && Win.ymax > 0)
	    xdraw_axis = 1;
	  break;
	case ('b'):
	case ('B'):
	  xdraw_bottom = 1;
	  break;
	case ('c'):
	case ('C'):
	  xdraw_top = 1;
	  break;
	case ('t'):
	case ('T'):
	  xdraw_majticks = 1;
	  break;
	case ('s'):
	case ('S'):
	  xdraw_minticks = 1;
	  break;
	case ('n'):
	case ('N'):
	  xdraw_labels = 1;
	  break;
	case ('m'):
	case ('M'):
	  xdraw_labels_ontop = 1;
	  break;
	case ('l'):
	case ('L'):
	  xdraw_log = 1;
	  break;
	case ('g'):
	case ('G'):
	  xdraw_grid = 1;
	  break;
	default:
	  break;
	}
    }

  /* set y-options */
  for (i = 0; yopt[i]; i++)
    {
      switch (yopt[i])
	{
	case ('a'):
	case ('A'):
	  /* check the axis will be in the box! */
	  if (Win.xmin < 0 && Win.xmax > 0)
	    ydraw_axis = 1;
	  break;
	case ('b'):
	case ('B'):
	  ydraw_left = 1;
	  break;
	case ('c'):
	case ('C'):
	  ydraw_right = 1;
	  break;
	case ('t'):
	case ('T'):
	  ydraw_majticks = 1;
	  break;
	case ('s'):
	case ('S'):
	  ydraw_minticks = 1;
	  break;
	case ('n'):
	case ('N'):
	  ydraw_labels = 1;
	  break;
	case ('m'):
	case ('M'):
	  ydraw_labels_toright = 1;
	  break;
	case ('v'):
	case ('V'):
	  ydraw_vertical = 1;
	  break;
	case ('l'):
	case ('L'):
	  ydraw_log = 1;
	  break;
	case ('g'):
	case ('G'):
	  ydraw_grid = 1;
	  break;
	default:
	  break;
	}
    }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  /* set major tick length in pixels */
  majTickL = Dev[id].fontExtents.max_x_advance * 0.33;
  subTickL = 0.;

  /* convert to world coords */
  cairo_device_to_user_distance (Dev[id].context, &subTickL, &majTickL);
  majTickL = -majTickL;

  /* set minor tick length as a fraction of the major tick length */
  subTickL = majTickL * 0.5;

  /* Choose x tick intervals */
  if (xdraw_log)
    {
      nMinTicksx = 1;
      xintervalMaj = 1.;
    }
  else if (_giza_equal(xtick,0.))
    {
      xintervalMaj =
	7. * Dev[id].fontExtents.max_x_advance / 
        ((Dev[id].VP.xmax - Dev[id].VP.xmin)*Dev[id].width);
      if (xintervalMaj > 0.2)
	xintervalMaj = 0.2;
      if (xintervalMaj < 0.05)
	xintervalMaj = 0.05;
      xintervalMaj = xintervalMaj * (Win.xmax - Win.xmin);
      xintervalMaj = giza_round (xintervalMaj, &nMinTicksx);
    }
  else
    {
      xintervalMaj = xtick;
      nMinTicksx = nxsub;
      if (nxsub < 1 || !xdraw_minticks)
	nMinTicksx = 1;
    }
  xintervalMin = xintervalMaj / (double) nMinTicksx;

  /* Draw X ticks */
  if (xdraw_majticks || xdraw_minticks || xdraw_grid)
    {
      _giza_tick_intervals (Win.xmin, Win.xmax, xintervalMin, &i1, &i2);
      jmax = 0;
      if (xdraw_log)
	jmax = 8;

      if (xdraw_axis || xdraw_bottom || xdraw_top || xdraw_grid)
	{
	  for (i = i1; i <= i2; i++)
	    {
	      /* If log axis ticks always have 9 minor ticks */
	      for (j = 0; j <= jmax; j++)
		{
		  /* log axis ticks are major when j = 0 */
		  major = (i % nMinTicksx == 0) && xdraw_majticks && (j == 0);
		  currentTickL = subTickL;
		  if (major)
		    currentTickL = majTickL;
		  xval = (i + logTab[j]) * xintervalMin;

		  /* don't draw over axis or outside the box */
		  if ((xdraw_axis && (i == 0)) || (xval >= Win.xmax) || (xval <= Win.xmin))
		    currentTickL = 0;

		  /* bottom */
		  if (xdraw_bottom)
		    {
		      cairo_move_to (Dev[id].context, xval, Win.ymin);
		      cairo_line_to (Dev[id].context, xval, Win.ymin + currentTickL);
		    }
		  /* grid */
		  if (xdraw_grid)
		    {
		      cairo_move_to (Dev[id].context, xval, Win.ymin);
		      cairo_line_to (Dev[id].context, xval, Win.ymax);
		    }
		  /* axis */
		  else if (xdraw_axis)
		    {
		      cairo_move_to (Dev[id].context, xval, -currentTickL);
		      cairo_line_to (Dev[id].context, xval, currentTickL);
		    }
		  /* top */
		  if (xdraw_top)
		    {
		      cairo_move_to (Dev[id].context, xval, Win.ymax);
		      cairo_line_to (Dev[id].context, xval, Win.ymax - currentTickL);
		    }
		}
	    }
	}

      _giza_stroke ();
    }

  /* X labels */
  if (xdraw_labels || xdraw_labels_ontop)
    {
      _giza_tick_intervals (Win.xmin, Win.xmax, xintervalMaj, &i1, &i2);
      np = (int) floor (log10 (fabs (xintervalMaj)));
      nv = _giza_nint (xintervalMaj/pow (10., np));

      for (i = i1; i <= i2; i++)
	{
	  xval = i * xintervalMaj;
	  xratio = (xval - Win.xmin) / (Win.xmax - Win.xmin);
	  if (xratio >= 0. && xratio <= 1.)
	    {
	      if (xdraw_log)
		{
                  jtmp = _giza_nint(xval);
                  if (jtmp == 1) {
                     sprintf (tmp, "10");
                  } else if (jtmp == 0) {
                     sprintf (tmp, "1");
                  } else {
		     sprintf (tmp, "10^{%i}", jtmp);
                  }
		}
	      else
		{
		  giza_format_number (i*nv, np, GIZA_NUMBER_FORMAT_AUTO, tmp);
		}

	      if (xdraw_labels)
		{
		  giza_annotate ("B", 1.2, xratio, 0.5, tmp);
		}
	      if (xdraw_labels_ontop)
		{
		  giza_annotate ("T", .4, xratio, 0.5, tmp);
		}
	    }
	}
      _giza_stroke ();
    }

  /* extra labels for log axis */
  if (xdraw_log && (Win.xmax - Win.xmin < 2.))
    {
      _giza_tick_intervals (Win.xmin, Win.xmax, xintervalMin, &i1, &i2);
      for (i = i1 - 1; i <= i2; i++)
	{
	  for (j = 1; j <= 4; j += 3)
	    {
	      xval = (i + logTab[j]) * xintervalMin;
	      if (xval <= Win.xmax && xval >= Win.xmin)
		{
		  xratio = (xval - Win.xmin) / (Win.xmax - Win.xmin);
		  xval = pow (10, xval);
		  giza_format_number (j+1, _giza_nint (i * xintervalMin), GIZA_NUMBER_FORMAT_AUTO, tmp);

		  if (xdraw_labels)
		    {
		      giza_annotate ("B", 1.2, xratio, 0.5, tmp);
		    }
		  if (xdraw_labels_ontop)
		    {
		      giza_annotate ("T", .4, xratio, 0.5, tmp);
		    }
		}
	    }
	}
    }

  /* set major tick length in pixels */
  majTickL = Dev[id].fontExtents.max_x_advance * 0.33;
  subTickL = 0.;
  /* convert to world coords */
  cairo_device_to_user_distance (Dev[id].context, &majTickL, &subTickL);

  /* set minor tick as a fraction of the major length */
  subTickL = majTickL * 0.5;

  /* Choose y tick intervals */
  if (ydraw_log)
    {
      nMinTicksy = 1;
      yintervalMaj = 1.;
    }
  else if (_giza_equal(ytick,0.))
    {
      yintervalMaj =
	7. * Dev[id].fontExtents.max_x_advance / 
        ((Dev[id].VP.ymax - Dev[id].VP.ymin) * Dev[id].height);
      if (yintervalMaj > 0.2) yintervalMaj = 0.2;
      if (yintervalMaj < 0.05) yintervalMaj = 0.05;
      yintervalMaj = yintervalMaj * (Win.ymax - Win.ymin);
      yintervalMaj = giza_round (yintervalMaj, &nMinTicksy);
    }
  else
    {
      yintervalMaj = ytick;
      nMinTicksy = nysub;
      if (nysub < 1 || !ydraw_minticks)
	nMinTicksy = 1;
    }
  yintervalMin = yintervalMaj / (double) nMinTicksy;

  /* Draw y ticks */
  if (ydraw_majticks || ydraw_minticks || ydraw_grid)
    {
      _giza_tick_intervals (Win.ymin, Win.ymax, yintervalMin, &i1, &i2);
      jmax = 0;
      if (ydraw_log)
	jmax = 8;

      if (ydraw_axis || ydraw_left || ydraw_right || ydraw_grid)
	{
	  for (i = i1; i <= i2; i++)
	    {
	      for (j = 0; j <= jmax; j++)
		{
		  major = (i % nMinTicksy == 0) && ydraw_majticks && (j == 0);
		  currentTickL = subTickL;
		  if (major)
		    currentTickL = majTickL;
		  yval = (i + logTab[j]) * yintervalMin;

		  /* don't draw over the axis or outside the box */
		  if ((ydraw_axis && (i == 0)) || yval >= Win.ymax || yval <= Win.ymin)
		    currentTickL = 0.;

		  /* left */
		  if (ydraw_left)
		    {
		      cairo_move_to (Dev[id].context, Win.xmin, yval);
		      cairo_line_to (Dev[id].context, Win.xmin + currentTickL, yval);
		    }
		  /* grid */
		  if (ydraw_grid)
		    {
		      cairo_move_to (Dev[id].context, Win.xmin, yval);
		      cairo_line_to (Dev[id].context, Win.xmax, yval);
		    }
		  /* axis */
		  else if (ydraw_axis)
		    {
		      cairo_move_to (Dev[id].context, -currentTickL, yval);
		      cairo_line_to (Dev[id].context, currentTickL, yval);
		    }
		  /* right */
		  if (ydraw_right)
		    {
		      cairo_move_to (Dev[id].context, Win.xmax, yval);
		      cairo_line_to (Dev[id].context, Win.xmax - currentTickL, yval);
		    }
		}
	    }
	}

      _giza_stroke ();
    }

  /* Label Y */
  if (ydraw_labels || ydraw_labels_toright)
    {
      _giza_tick_intervals (Win.ymin, Win.ymax, yintervalMaj, &i1, &i2);
      np = (int) floor (log10 (fabs (yintervalMaj)));
      nv = _giza_nint (yintervalMaj/pow (10., np));

      for (i = i1; i <= i2; i++)
	{
	  yval = i * yintervalMaj;
	  yratio = (yval - Win.ymin) / (Win.ymax - Win.ymin);
	  if (yratio >= 0. && yratio <= 1.)
	    {
	      if (ydraw_log)
		{
                  jtmp = _giza_nint(yval);
                  if (jtmp == 1) {
                     sprintf (tmp, "10");
                  } else if (jtmp == 0) {
                     sprintf (tmp, "1");
                  } else {
		     sprintf (tmp, "10^{%i}", jtmp);
                  }
		}
	      else
		{
		  giza_format_number (i*nv, np, GIZA_NUMBER_FORMAT_AUTO, tmp);
		}

	      if (ydraw_vertical)
		{
		  if (ydraw_labels)
		    giza_annotate ("LV", .7, yratio, 1., tmp);
		  if (ydraw_labels_toright)
		    giza_annotate ("RV", .7, yratio, 0., tmp);
		}
	      else
		{
		  if (ydraw_labels)
		    giza_annotate ("L", .7, yratio, 0.5, tmp);
		  if (ydraw_labels_toright)
		    giza_annotate ("R", 1.2, yratio, 0.5, tmp);
		}
	    }
	}
    }

  /* extra labels for log axis */
  if (ydraw_log && (Win.ymax - Win.ymin < 2.))
    {
      _giza_tick_intervals (Win.ymin, Win.ymax, yintervalMin, &i1, &i2);
      for (i = i1 - 1; i <= i2; i++)
	{
	  for (j = 1; j <= 4; j += 3)
	    {
	      yval = (i + logTab[j]) * yintervalMin;
	      if (yval <= Win.ymax && yval >= Win.ymin)
		{
		  yratio = (yval - Win.ymin) / (Win.ymax - Win.ymin);
		  yval = pow (10, yval);
		  giza_format_number (j+1, _giza_nint (i*yintervalMin), GIZA_NUMBER_FORMAT_AUTO, tmp);
		  if (ydraw_vertical)
		    {
		      if (ydraw_labels)
			{
			  giza_annotate ("LV", .7, yratio, 1., tmp);
			}
		      if (ydraw_labels_toright)
			{
			  giza_annotate ("RV", .7, yratio, 0., tmp);
			}
		    }
		  else
		    {
		      if (ydraw_labels)
			{
			  giza_annotate ("L", .7, yratio, .5, tmp);
			}
		      if (ydraw_labels_toright)
			{
			  giza_annotate ("R", 1.2, yratio, 0.5, tmp);
			}
		    }
		}
	    }
	}
    }

  /* draw the bottom of the box */
  if (xdraw_bottom)
    {
      cairo_move_to (Dev[id].context, Win.xmin, Win.ymin);
      cairo_line_to (Dev[id].context, Win.xmax, Win.ymin);
    }

  /* draw the right of the box */
  if (ydraw_right)
    {
      cairo_move_to (Dev[id].context, Win.xmax, Win.ymin);
      cairo_line_to (Dev[id].context, Win.xmax, Win.ymax);
    }

  /* draw the top of the box */
  if (xdraw_top)
    {
      cairo_move_to (Dev[id].context, Win.xmax, Win.ymax);
      cairo_line_to (Dev[id].context, Win.xmin, Win.ymax);
    }

  /* draw the left of the box */
  if (ydraw_left)
    {
      cairo_move_to (Dev[id].context, Win.xmin, Win.ymax);
      cairo_line_to (Dev[id].context, Win.xmin, Win.ymin);
    }

  /* draw the x axis */
  if (xdraw_axis)
    {
      cairo_move_to (Dev[id].context, Win.xmin, 0.);
      cairo_line_to (Dev[id].context, Win.xmax, 0.);
    }

  /* draw the y axis */
  if (ydraw_axis)
    {
      cairo_move_to (Dev[id].context, 0., Win.ymin);
      cairo_line_to (Dev[id].context, 0., Win.ymax);
    }

  /* stroke all the paths */
  int lc;
  giza_get_line_cap (&lc);
  giza_set_line_cap (CAIRO_LINE_CAP_SQUARE);
  _giza_stroke ();
  giza_set_line_cap (lc);
  _giza_set_trans (oldTrans);

  /* end buffer if it was not on before this function call */
  if (!oldBuf)
    {
      giza_end_buffer ();
    }

  giza_flush_device ();

  /* Restore clipping */
  giza_set_viewport (Dev[id].VP.xmin, Dev[id].VP.xmax, Dev[id].VP.ymin, Dev[id].VP.ymax);
}

/**
 * Drawing: giza_box_float
 *
 * Synopsis: Same functionality as giza_box but takes floats instead of doubles.
 *
 * See Also: giza_box
 */
void
giza_box_float (const char *xopt, float xtick, int nxsub,
		const char *yopt, float ytick, int nysub)
{
  if (!_giza_check_device_ready ("giza_plot_box"))
    return;

  giza_box (xopt, (double) xtick, nxsub, yopt, (double) ytick, nysub);
}

/**
 * Works out which intervals in which to draw ticks.
 */
static void
_giza_tick_intervals (double xmin, double xmax, double xinterval, int *i1,
		      int *i2)
{
  double xlow, xhigh;

  xlow  = xmin / xinterval;
  xhigh = xmax / xinterval;

  *i1 = _giza_nint (xlow);
  *i2 = _giza_nint (xhigh);
}

/**
 * Finds the smallest 'round' number larger than x, where round is defined
 * as 1, 2 or 5 times a power of ten.
 *
 * Input:
 *  -x    :- The number to be rounded
 *  -nsub :- A nice number of minor ticks to put between the 'round' number
 *
 * Return: The smallest 'round' number.
 */
double
giza_round (double x, int *nsub)
{
  int i, ilog;
  double frac, pwr, xlog, xx;
  double nice[] = { 2, 5, 10 };

  if (fabs (x) < GIZA_ZERO_FLOAT)
    {
      *nsub = 2;
      return 0;
    }

  xx = x;
  if (x < 0)
    xx = -x;
  xlog = log10 (xx);
  ilog = (int) xlog;
  if (xlog < 0)
    ilog = ilog - 1;
  pwr = pow (10.0, ilog);
  frac = xx / pwr;
  i = 2;
  if (frac < nice[1])
    i = 1;
  if (frac < nice[0])
    i = 0;
  *nsub = 5;
  if (i == 0)
    *nsub = 2;
  if (x < 0)
    return -pwr * nice[i];
  return pwr * nice[i];
}

/**
 * Same as giza_round but accepts floats
 */
float
giza_round_float (float x, int *nsub)
{
  float result;
  result = (float) giza_round((double) x, nsub);
  return result;
}
