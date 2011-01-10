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
 * DJP (21/4/10): added _giza_get_sigfigs utility to fix bug in this
 *                for exponentials
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
static double _giza_round (double x, int *nsub);
//static int _giza_get_sigfigs (const double xmin, const double xmax,
//			      const double xinterval);

/**
 * Drawing: giza_box
 *
 * Synopsis: Annotates the view port with labelled axis/frame
 *
 * Input:
 *  -xopts :- String of options for the x-axis.
 *            The options may be in any order. See 
 *            below for
 *            details
 *  -xtick :- The distance, in world coordinates, 
 *            between major ticks on the x-axis. 
 *            If 0.0 the interval is chosen 
 *            by giza_box.
 *  -nxsub :- The number of minor ticks to be placed 
 *            between each major tick. If 0 the 
 *            number is chosen by giza_box.
 *  -yopts :- Similar to xopts but for the y-axis.
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
 */
void
giza_box (const char *xopt, double xtick, int nxsub,
	  const char *yopt, double ytick, int nysub)
{
  if (!_giza_check_device_ready ("giza_plot_box"))
    return;

  _giza_expand_clipping ();

  int oldBuf = Sets.buf;

  // Table of log10() values for log axis ticks
  double logTab[9];
  int k;
  for (k = 0; k < 9; k++)
    {
      logTab[k] = log10 (k + 1);
    }

  // Begin buffering
  giza_begin_buffer ();

  int xopta = 0, xoptb = 0, xoptc = 0, xoptt = 0, xopts = 0, xoptn =
    0, xoptm = 0, xoptl = 0;

  int yopta = 0, yoptb = 0, yoptc = 0, yoptt = 0, yopts = 0, yoptn =
    0, yoptm = 0, yoptv = 0, yoptl = 0;

  double xintervalMaj, xintervalMin, xval, xratio;
  double yintervalMaj, yintervalMin, yval, yratio;
  int nv, np;
  int nMinTicksx, major;
  int nMinTicksy;
  double majTickL, subTickL, currentTickL;

  // set x-options
  int i;
  for (i = 0; xopt[i]; i++)
    {
      switch (xopt[i])
	{
	case ('a'):
	case ('A'):
	  // Check the axis will be inside the box!
	  if (Win.ymin < 0 && Win.ymax > 0)
	    xopta = 1;
	  break;
	case ('b'):
	case ('B'):
	  xoptb = 1;
	  break;
	case ('c'):
	case ('C'):
	  xoptc = 1;
	  break;
	case ('t'):
	case ('T'):
	  xoptt = 1;
	  break;
	case ('s'):
	case ('S'):
	  xopts = 1;
	  break;
	case ('n'):
	case ('N'):
	  xoptn = 1;
	  break;
	case ('m'):
	case ('M'):
	  xoptm = 1;
	  break;
	case ('l'):
	case ('L'):
	  xoptl = 1;
	  break;
	default:
	  break;
	}
    }

  // set y-options
  for (i = 0; yopt[i]; i++)
    {
      switch (yopt[i])
	{
	case ('a'):
	case ('A'):
	  // check the axis will be in the box!
	  if (Win.xmin < 0 && Win.xmax > 0)
	    yopta = 1;
	  break;
	case ('b'):
	case ('B'):
	  yoptb = 1;
	  break;
	case ('c'):
	case ('C'):
	  yoptc = 1;
	  break;
	case ('t'):
	case ('T'):
	  yoptt = 1;
	  break;
	case ('s'):
	case ('S'):
	  yopts = 1;
	  break;
	case ('n'):
	case ('N'):
	  yoptn = 1;
	  break;
	case ('m'):
	case ('M'):
	  yoptm = 1;
	  break;
	case ('v'):
	case ('V'):
	  yoptv = 1;
	  break;
	case ('l'):
	case ('L'):
	  yoptl = 1;
	  break;
	default:
	  break;
	}
    }

  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);

  // set major tick length in pixels
  majTickL = Sets.fontExtents.max_x_advance * 0.33;
  subTickL = 0.;

  //convert to world coords
  cairo_device_to_user_distance (context, &subTickL, &majTickL);
  majTickL = -majTickL;

  // set minor tick length as a fraction of the major tick length
  subTickL = majTickL * 0.5;

  // Choose x tick intervals
  if (xoptl)
    {
      nMinTicksx = 1;
      xintervalMaj = 1.;
    }
  else if (xtick == 0)
    {
      xintervalMaj =
	7. * Sets.fontExtents.max_x_advance / ((VP.xmax - VP.xmin) *
					       Dev.width);
      if (xintervalMaj > 0.2)
	xintervalMaj = 0.2;
      if (xintervalMaj < 0.05)
	xintervalMaj = 0.05;
      xintervalMaj = xintervalMaj * (Win.xmax - Win.xmin);
      xintervalMaj = _giza_round (xintervalMaj, &nMinTicksx);
    }
  else
    {
      xintervalMaj = xtick;
      nMinTicksx = nxsub;
      if (nxsub < 1 || !xopts)
	nMinTicksx = 1;
    }
  xintervalMin = xintervalMaj / (double) nMinTicksx;

  // Draw X ticks
  if (xoptt || xopts)
    {
      int i, i1, i2, j, jmax;
      _giza_tick_intervals (Win.xmin, Win.xmax, xintervalMin, &i1, &i2);
      jmax = 0;
      if (xoptl)
	jmax = 8;

      if (xopta || xoptb || xoptc)
	{
	  for (i = i1; i <= i2; i++)
	    {
	      // If log axis ticks always have 9 minor ticks
	      for (j = 0; j <= jmax; j++)
		{
		  // log axis ticks are major when j = 0
		  major = (i % nMinTicksx == 0) && xoptt && (j == 0);
		  currentTickL = subTickL;
		  if (major)
		    currentTickL = majTickL;
		  xval = (i + logTab[j]) * xintervalMin;

		  // don't draw over axis or outside the box
		  if ((xopta && (i == 0)) || (xval >= Win.xmax) || (xval <= Win.xmin))
		    currentTickL = 0;

		  // bottom
		  if (xoptb)
		    {
		      cairo_move_to (context, xval, Win.ymin);
		      cairo_line_to (context, xval, Win.ymin + currentTickL);
		    }
		  // axis
		  if (xopta)
		    {
		      cairo_move_to (context, xval, 0.);
		      cairo_line_to (context, xval, currentTickL);
		    }
		  // top
		  if (xoptc)
		    {
		      cairo_move_to (context, xval, Win.ymax);
		      cairo_line_to (context, xval, Win.ymax - currentTickL);
		    }
		}
	    }
	}

      _giza_stroke ();
    }

  // X labels
  if (xoptn || xoptm)
    {
      int i, i1, i2;
      _giza_tick_intervals (Win.xmin, Win.xmax, xintervalMaj, &i1, &i2);
      np = floor (log10 (fabs (xintervalMaj)));
      nv = _giza_nint (xintervalMaj/pow (10., np));

      for (i = i1; i <= i2; i++)
	{
	  xval = i * xintervalMaj;
	  xratio = (xval - Win.xmin) / (Win.xmax - Win.xmin);
	  if (xratio >= 0. && xratio <= 1.)
	    {
	      char tmp[50];
	      if (xoptl)
		{
		  sprintf (tmp, "10^{%i}", _giza_nint (xval));
		}
	      else
		{
		  giza_format_number (i*nv, np, GIZA_NUMBER_FORMAT_AUTO, tmp);
		}

	      if (xoptn)
		{
		  giza_annotate ("B", 1.2, xratio, 0.5, tmp);
		}
	      if (xoptm)
		{
		  giza_annotate ("T", .4, xratio, 0.5, tmp);
		}
	    }
	}
      _giza_stroke ();
    }

  // extra labels for log axis
  if (xoptl && (Win.xmax - Win.xmin < 2.))
    {
      int i, i1, i2, j;
      char tmp[100];
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
		  giza_format_number (j, _giza_nint (i * xintervalMin), GIZA_NUMBER_FORMAT_AUTO, tmp);
		  if (xoptn)
		    {
		      giza_annotate ("B", 1.2, xratio, 0.5, tmp);
		    }
		  if (xoptm)
		    {
		      giza_annotate ("T", .4, xratio, 0.5, tmp);
		    }
		}
	    }
	}
    }

  // set major tick length in pixels
  majTickL = Sets.fontExtents.max_x_advance * 0.33;
  subTickL = 0.;
  //convert to world coords
  cairo_device_to_user_distance (context, &majTickL, &subTickL);

  // set minor tick as a fraction of the major length
  subTickL = majTickL * 0.5;

  // Choose y tick intervals
  if (yoptl)
    {
      nMinTicksy = 1;
      yintervalMaj = 1.;
    }
  else if (ytick == 0)
    {
      yintervalMaj =
	7. * Sets.fontExtents.max_x_advance / ((VP.ymax - VP.ymin) *
					       Dev.height);
      if (yintervalMaj > 0.2) yintervalMaj = 0.2;
      if (yintervalMaj < 0.05) yintervalMaj = 0.05;
      yintervalMaj = yintervalMaj * (Win.ymax - Win.ymin);
      yintervalMaj = _giza_round (yintervalMaj, &nMinTicksy);
    }
  else
    {
      yintervalMaj = ytick;
      nMinTicksy = nysub;
      if (nysub < 1 || !yopts)
	nMinTicksy = 1;
    }
  yintervalMin = yintervalMaj / (double) nMinTicksy;

  // Draw y ticks
  if (yoptt || yopts)
    {
      int i, i1, i2, j, jmax;
      _giza_tick_intervals (Win.ymin, Win.ymax, yintervalMin, &i1, &i2);
      jmax = 0;
      if (yoptl)
	jmax = 8;

      if (yopta || yoptb || yoptc)
	{
	  for (i = i1; i <= i2; i++)
	    {
	      for (j = 0; j <= jmax; j++)
		{
		  major = (i % nMinTicksy == 0) && yoptt && (j == 0);
		  currentTickL = subTickL;
		  if (major)
		    currentTickL = majTickL;
		  yval = (i + logTab[j]) * yintervalMin;

		  // don't draw over the axis or outside the box
		  if ((yopta && (i == 0)) || yval >= Win.ymax || yval <= Win.ymin)
		    currentTickL = 0.;

		  // left
		  if (yoptb)
		    {
		      cairo_move_to (context, Win.xmin, yval);
		      cairo_line_to (context, Win.xmin + currentTickL, yval);
		    }
		  // axis
		  if (yopta)
		    {
		      cairo_move_to (context, 0., yval);
		      cairo_line_to (context, currentTickL, yval);
		    }
		  // right
		  if (yoptc)
		    {
		      cairo_move_to (context, Win.xmax, yval);
		      cairo_line_to (context, Win.xmax - currentTickL, yval);
		    }
		}
	    }
	}

      _giza_stroke ();
    }

  // Label Y
  if (yoptn || yoptm)
    {
      int i, i1, i2;
      _giza_tick_intervals (Win.ymin, Win.ymax, yintervalMaj, &i1, &i2);
      np = floor (log10 (fabs (yintervalMaj)));
      nv = _giza_nint (yintervalMaj/pow (10., np));

      for (i = i1; i <= i2; i++)
	{
	  yval = i * yintervalMaj;
	  yratio = (yval - Win.ymin) / (Win.ymax - Win.ymin);
	  if (yratio >= 0. && yratio <= 1.)
	    {
	      char tmp[50];
	      if (yoptl)
		{
		  sprintf (tmp, "10^{%i}", _giza_nint (yval));
		}
	      else
		{
		  giza_format_number (i*nv, np, GIZA_NUMBER_FORMAT_AUTO, tmp);
		}

	      if (yoptv)
		{
		  if (yoptn)
		    giza_annotate ("LV", .7, yratio, 1., tmp);
		  if (yoptm)
		    giza_annotate ("RV", .7, yratio, 0., tmp);
		}
	      else
		{
		  if (yoptn)
		    giza_annotate ("L", .7, yratio, 0.5, tmp);
		  if (yoptm)
		    giza_annotate ("R", 1.2, yratio, 0.5, tmp);
		}
	    }
	}
    }

  // extra labels for log axis
  if (yoptl && (Win.ymax - Win.ymin < 2.))
    {
      int i, i1, i2, j;
      char tmp[100];
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
		  giza_format_number (j, _giza_nint (i*yintervalMin), GIZA_NUMBER_FORMAT_AUTO, tmp);
		  if (yoptv)
		    {
		      if (yoptn)
			{
			  giza_annotate ("LV", .7, yratio, 1., tmp);
			}
		      if (yoptm)
			{
			  giza_annotate ("RV", .7, yratio, 0., tmp);
			}
		    }
		  else
		    {
		      if (yoptn)
			{
			  giza_annotate ("L", .7, yratio, .5, tmp);
			}
		      if (yoptm)
			{
			  giza_annotate ("R", 1.2, yratio, 0.5, tmp);
			}
		    }
		}
	    }
	}
    }

  // draw the bottom of the box
  if (xoptb)
    {
      cairo_move_to (context, Win.xmin, Win.ymin);
      cairo_line_to (context, Win.xmax, Win.ymin);
    }

  // draw the right of the box
  if (yoptc)
    {
      cairo_move_to (context, Win.xmax, Win.ymin);
      cairo_line_to (context, Win.xmax, Win.ymax);
    }

  // draw the top of the box
  if (xoptc)
    {
      cairo_move_to (context, Win.xmax, Win.ymax);
      cairo_line_to (context, Win.xmin, Win.ymax);
    }

  // draw the left of the box
  if (yoptb)
    {
      cairo_move_to (context, Win.xmin, Win.ymax);
      cairo_line_to (context, Win.xmin, Win.ymin);
    }

  // draw the x axis
  if (xopta)
    {
      cairo_move_to (context, Win.xmin, 0.);
      cairo_line_to (context, Win.xmax, 0.);
    }

  // draw the y axis
  if (yopta)
    {
      cairo_move_to (context, 0., Win.ymin);
      cairo_line_to (context, 0., Win.ymax);
    }

  // stroke all the paths
  int lc;
  giza_get_line_cap (&lc);
  giza_set_line_cap (CAIRO_LINE_CAP_SQUARE);
  _giza_stroke ();
  giza_set_line_cap (lc);
  _giza_set_trans (oldTrans);

  // end buffer if it was not on before this function call
  if (!oldBuf)
    {
      giza_end_buffer ();
    }

  if (!Sets.buf)
    {
      giza_flush_device ();
    }

  // Restore clipping
  giza_set_viewport (VP.xmin, VP.xmax, VP.ymin, VP.ymax);
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
static double
_giza_round (double x, int *nsub)
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
  ilog = xlog;
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
 * Finds the required number of significant figures to distinguish
 * between the tick intervals (DJP)
 */
//static int
//_giza_get_sigfigs (const double xmin, const double xmax,
//		   const double xinterval)
//{
//  double max = fabs (xmax), min = fabs (xmin);
//  int nsigfig;
//  if (max > min)
//    {
//      nsigfig = fabs (log10 (max)) + 1;
//    }
//  else
//    {
//      nsigfig = fabs (log10 (min)) + 1;
//    }
//  nsigfig = nsigfig + fabs (log10 (xinterval)) + 1;
//
//  /* DJP: this is a workaround for the fact that
//   * the sigfigs calculation does not work at all
//   * for exponentials: just use a maximum
//   */
//  if (nsigfig > 12 || nsigfig < 2)
//    nsigfig = 12;
//  return nsigfig;
//}

