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

#include "giza-colour-private.h"
#include "giza-private.h"
#include "giza-io-private.h"
#include <math.h>
#include <stdlib.h>
#include <giza.h>

static giza_ctab _giza_colour_table;
static giza_ctab _giza_saved_colour_tables[GIZA_CTAB_SAVE_MAX];
static int _giza_save_ctab_ncalls;

/**
 * Settings: giza_set_colour_table
 *
 * Synopsis: Sets the colour table.
 *
 * Input:
 *  -controlPoints :- Specifies at which fraction of the ramp the control points are placed
 *  -red   :- the normalised value of the red component at each control point
 *  -green :- the normalised value of the green component at each control point
 *  -blue  :- the normalised value of the blue component at each control point
 *  -n     :- the number of control points
 *  -contrast   :- contrast of colour ramp (normally 1.0). Use -contrast to reverse direction of ramp
 *  -brightness :- brightness of colour ramp (normally 0.5)
 *
 * Return:
 *  -0 :- No error.
 *  -1 :- An error occurred.
 */
int
giza_set_colour_table (const double *controlPoints, const double *red, const double *green, const double *blue, int n, 
                       double contrast, double brightness)
{
  if (!_giza_check_device_ready ("giza_set_colour_table"))
    return 1;

  if (n < 2 || n > GIZA_CTAB_MAXSIZE)
    {
      _giza_warning ("giza_set_colour_table", "Invalid number of control points, colour table not set");
      return 1;
    }

  int i, tmpn = 0;
  int iprev = 0;
  /*printf("contrast = %f, brightness = %f \n",contrast,brightness);*/

  if (contrast >= 0.) 
    {
      for (i = 0; i < n; i++)
        {
          /* check the control point is valid */
          if (controlPoints[i] >= 0.  && controlPoints[i] <= 1.)
	    {
              if (tmpn > 0) iprev = tmpn - 1;
              if (tmpn <= 0 || controlPoints[i] > _giza_colour_table.controlPoints[iprev])
	        {
	          tmpn++;
	          _giza_colour_table.controlPoints[tmpn-1] = contrast*controlPoints[i];
                  _giza_colour_table.red[tmpn-1]   = _giza_set_in_range(red[i],0.,1.);
                  _giza_colour_table.green[tmpn-1] = _giza_set_in_range(green[i],0.,1.);
                  _giza_colour_table.blue[tmpn-1]  = _giza_set_in_range(blue[i],0.,1.);
	        }
            }
        }
    }
  else
    {
      for (i = n-1; i >= 0; i--)
        {
          /* check the control point is valid */
          if (controlPoints[i] >= 0.  && controlPoints[i] <= 1.)
	    {
              if (tmpn > 0) iprev = tmpn - 1;
              if (tmpn <= 0 || (1. + contrast*controlPoints[i]) > _giza_colour_table.controlPoints[iprev])
	        {
	          tmpn++;
	          _giza_colour_table.controlPoints[tmpn-1] = 1. + contrast*controlPoints[i];
                  _giza_colour_table.red[tmpn-1]   = _giza_set_in_range(red[i],0.,1.);
                  _giza_colour_table.green[tmpn-1] = _giza_set_in_range(green[i],0.,1.);
                  _giza_colour_table.blue[tmpn-1]  = _giza_set_in_range(blue[i],0.,1.);
	        }
            }
        }    
    }

  _giza_colour_table.n = tmpn;
   if (tmpn < n)
   {
     _giza_warning ("giza_set_colour_table", "Invalid values for control points in colour table settings");
     return 2;
   }

  /* use the installed colour table to set the colours of colour indices in the specified range */
  _giza_set_range_from_colour_table(_giza_colour_index_min,_giza_colour_index_max);

  return 0;
}

/**
 * Settings: giza_set_colour_table_float
 *
 * Synopsis: Same functionality as giza_set_colour_table but takes floats
 *
 * See also: giza_set_colour_table
 */
int
giza_set_colour_table_float (const float *controlPoints, const float *red, const float *green, const float *blue, int n, 
                             float contrast, float brightness)
{
  if (!_giza_check_device_ready ("giza_set_colour_table_float"))
    return 1;

  double dred[n];
  double dgreen[n];
  double dblue[n];
  double dcontrol[n];
  int i;
  
  for (i = 0; i < n; i++)
    {
      dred[i]   = (double) red[i];
      dgreen[i] = (double) green[i];
      dblue[i]  = (double) blue[i];
      dcontrol[i] = (double) controlPoints[i];
    }

  return giza_set_colour_table(dcontrol,dred,dgreen,dblue,n,(double) contrast,(double) brightness);
}

/**
 * Settings: giza_rgb_from_table
 *
 * Synopsis: Gets the rgb values from the colour table corresponding to the fraction of the table.
 *
 * Input:
 *  -pos   :- The fraction along the table to retrieve the colour from.
 *           A value less then 0 is assigned the colour at zero, and above one assigned
 *           the colour at one.
 *  -red    :- Gets set to the red component of the colour at pos.
 *  -green  :- Gets set to the green component of the colour at pos.
 *  -blue   :- Gets set to the blue component of the colour at pos.
 *
 * See also: giza_set_colour_table
 */
void
giza_rgb_from_table (double pos, double *red, double *green, double *blue)
{
  if (!_giza_check_device_ready ("giza_rgb_from_table"))
    return;

  *red = -1.;

  /* find between which control points pos lies */
  int i;
  /* If it is lower than lowest control point get the lowest colour */
  if (pos < _giza_colour_table.controlPoints[0])
    {
      *red   = _giza_colour_table.red[0];
      *green = _giza_colour_table.green[0];
      *blue  = _giza_colour_table.blue[0];
      return;
    }
  /* If it is higher than the highest control point get the highest colour */
  if (pos >= _giza_colour_table.controlPoints[_giza_colour_table.n - 1])
    {
      *red = _giza_colour_table.red[_giza_colour_table.n - 1];
      *green = _giza_colour_table.green[_giza_colour_table.n - 1];
      *blue = _giza_colour_table.blue[_giza_colour_table.n - 1];
      return;
    }
  /* Otherwise look in the table */
  for (i = 1; i < _giza_colour_table.n; i++)
    {
      if (pos < _giza_colour_table.controlPoints[i])
	{
	  double fraction, d_giza_colour_table;
	  /* find where between the two control points pos lies */
	  d_giza_colour_table = (_giza_colour_table.controlPoints[i] - _giza_colour_table.controlPoints[i - 1]);
	  if (fabs (d_giza_colour_table) > GIZA_TINY)
	    {
	      fraction = (pos - _giza_colour_table.controlPoints[i - 1]) / d_giza_colour_table;
	    }
	  else
	    {
	      fraction = 1.;
	    }
	  /* set the r,g,b using this fraction */
	  *red = _giza_colour_table.red[i - 1] + fraction * (_giza_colour_table.red[i] - _giza_colour_table.red[i - 1]);
	  *green =
	    _giza_colour_table.green[i - 1] + fraction * (_giza_colour_table.green[i] -
					    _giza_colour_table.green[i - 1]);
	  *blue =
	    _giza_colour_table.blue[i - 1] + fraction * (_giza_colour_table.blue[i] - _giza_colour_table.blue[i - 1]);
	  return;
	}
    }

   /* this should never happen (but DOES if pos is NaN or otherwise ill-defined) */
   if (*red < 0.)
     {
      *red = 0.;
      *green = 0.;
      *blue = 0.;
    }
}

/**
 * Settings: giza_rgb_from_table_float
 *
 * Synopsis: Same functionality giza_rgb_from_table but takes floats
 *
 * See Also: giza_rgb_from_table, giza_set_colour_table
 */
void
giza_rgb_from_table_float (float pos, float *red, float *green, float *blue)
{
  double dred, dgreen, dblue;

  giza_rgb_from_table ((double) pos, &dred, &dgreen, &dblue);

  *red   = (float) dred;
  *green = (float) dgreen;
  *blue  = (float) dblue;
}

/**
 * Sets up memory for the main colour table and establishes a grey scale colour table
 */
void
_giza_init_colour_table (void)
{
  _giza_save_ctab_ncalls = 0;
  giza_set_colour_table_gray();
}

/**
 * Frees the memory associated with the main colour table
 */
void
_giza_free_colour_table (void)
{
  /*_giza_deallocate_colour_table(&_giza_colour_table);*/
}

/**
 * Settings: giza_save_colour_table
 *
 * Synopsis: Saves the current colour table
 *
 * See Also: giza_restore_colour_table
 */
void 
giza_save_colour_table (void)
{
  _giza_save_ctab_ncalls += 1;
  if (_giza_save_ctab_ncalls > GIZA_CTAB_SAVE_MAX) {
     _giza_warning("giza_save_colour_table","max number of giza_save_colour_table calls exceeded");
     return;
  }
  int i = _giza_save_ctab_ncalls - 1;

  /* save the current colour table to the array */
  _giza_saved_colour_tables[i] = _giza_colour_table;
}

/**
 * Settings: giza_restore_colour_table
 *
 * Synopsis: Restores the colour table from a previously saved one
 *
 * See Also: giza_save_colour_table
 */
void 
giza_restore_colour_table (void)
{
  _giza_save_ctab_ncalls -= 1;
  if (_giza_save_ctab_ncalls < 0) {
     _giza_warning("giza_restore_colour_table","unmatched save/restore calls");
     _giza_save_ctab_ncalls = 0;
     return;
  }

  /* Restore the colour table from the saved array */
  int i = _giza_save_ctab_ncalls;
  _giza_colour_table = _giza_saved_colour_tables[i];

}

/**
 * (Re)sets the colour table to a simple black->white ramp
 */
int
giza_set_colour_table_gray (void)
{
  double cp[2], r[2], g[2], b[2];
  cp[0] = 0.; r[0] = 0.; g[0] = 0.; b[0] = 0.;
  cp[1] = 1.; r[1] = 1.; g[1] = 1.; b[1] = 1.;
  return giza_set_colour_table (cp, r, g, b, 2, 1., 0.5);
}

/**
 * Sets rgb colours for the colour indices in the given range
 * as a linear ramp based on the current giza colour table
 */
void
_giza_set_range_from_colour_table (int cimin, int cimax)
{
  double delta = (double) (cimax - cimin);
  double pos,r,g,b;
  int i;

  for (i = cimin; i <= cimax; i++)
    {
      pos = (i - cimin)/delta;
      giza_rgb_from_table(pos,&r,&g,&b);
      giza_set_colour_representation(i,r,g,b);
    }
}

