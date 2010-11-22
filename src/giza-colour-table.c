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

#include "giza-colour-table-private.h"
#include "giza-private.h"
#include "giza-io-private.h"
#include <math.h>
#include <stdlib.h>

struct CP_Colour_Table
{
  int n;
  double *controlPoints;
  double *red;
  double *green;
  double *blue;
} _giza_colour_table;


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
 *
 * Return:
 *  -0 :- No error.
 *  -1 :- An error occurred.
 */
int
giza_set_colour_table (double *controlPoints, double *red, double *green, double *blue, int n)
{
  if (!_giza_check_device_ready ("giza_set_colour_table"))
    return 1;
  
  if (n < 2)
    {
      _giza_warning ("giza_set_colour_table", "Invalid number of control points, colour table not set");
      return 1;
    }

  _giza_colour_table.controlPoints = realloc (_giza_colour_table.controlPoints, n * sizeof(double));
  _giza_colour_table.red = realloc (_giza_colour_table.red, n * sizeof(double));
  _giza_colour_table.green = realloc (_giza_colour_table.green, n * sizeof(double));
  _giza_colour_table.blue = realloc (_giza_colour_table.blue, n * sizeof(double));
  int i, tmpn = 0;

  for (i = 0; i < n; i++)
    {
      // check the control point is valid
      if(controlPoints[i] >= 0.  && controlPoints[i] <= 1.)
	{
	  if(tmpn <= 0 || controlPoints[i] > _giza_colour_table.controlPoints[tmpn - 1])
	    {
	      _giza_colour_table.controlPoints[i] = controlPoints[i];

	      if (red[i] > 1.) 
		_giza_colour_table.red[i] = 1.;
	      else if (red[i] < 0.) 
		_giza_colour_table.red[i] = 0.;
	      else
		_giza_colour_table.red[i] = red[i];

	      if (green[i] > 1.) 
		_giza_colour_table.green[i] = 1.;
	      else if (green[i] < 0.) 
		_giza_colour_table.green[i] = 0.;
	      else
		_giza_colour_table.green[i] = green[i];

	      if (blue[i] > 1.) 
		_giza_colour_table.blue[i] = 1.;
	      else if (blue[i] < 0.) 
		_giza_colour_table.blue[i] = 0.;
	      else
		_giza_colour_table.blue[i] = blue[i];
      
	      tmpn++;
	    }
	}
    }
  _giza_colour_table.n = tmpn;

  return 0;
}

/**
 * Settings: giza_set_colour_table_float
 *
 * Synopsis: Same functionality giza_set_colour_table. Not simple a call to set colour table to
 * avoid memory wastage for big arrays.
 */
int
giza_set_colour_table_float (float *controlPoints, float *red, float *green, float *blue, int n)
{
  if (!_giza_check_device_ready ("giza_set_colour_table_float"))
    return 1;
    if (n < 2)
    {
      _giza_warning ("giza_set_colour_table_float", "Invalid number of control points, colour table not set");
      return 1;
    }
    
  _giza_colour_table.controlPoints = realloc (_giza_colour_table.controlPoints, n * sizeof(double));
  _giza_colour_table.red = realloc (_giza_colour_table.red, n * sizeof(double));
  _giza_colour_table.green = realloc (_giza_colour_table.green, n * sizeof(double));
  _giza_colour_table.blue = realloc (_giza_colour_table.blue, n * sizeof(double));
  int i, tmpn = 0;

  for (i = 0; i < n; i++)
    {
      // check the control point is valid
      if(controlPoints[i] >= 0.  && controlPoints[i] <= 1.)
	{
	  if(tmpn <= 0 || controlPoints[i] > _giza_colour_table.controlPoints[tmpn - 1])
	    {
	      _giza_colour_table.controlPoints[i] = (double) controlPoints[i];

	      if (red[i] > 1.) 
		_giza_colour_table.red[i] = 1.;
	      else if (red[i] < 0.) 
		_giza_colour_table.red[i] = 0.;
	      else
		_giza_colour_table.red[i] = (double) red[i];

	      if (green[i] > 1.) 
		_giza_colour_table.green[i] = 1.;
	      else if (green[i] < 0.) 
		_giza_colour_table.green[i] = 0.;
	      else
		_giza_colour_table.green[i] = (double) green[i];

	      if (blue[i] > 1.) 
		_giza_colour_table.blue[i] = 1.;
	      else if (blue[i] < 0.) 
		_giza_colour_table.blue[i] = 0.;
	      else
		_giza_colour_table.blue[i] = (double) blue[i];
      
	      tmpn++;
	    }
	}
    }
  _giza_colour_table.n = tmpn;

  return 0;
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
 */
void
giza_rgb_from_table (double pos, double *red, double *green, double *blue)
{
  if (!_giza_check_device_ready ("giza_rgb_from_table"))
    return;

  //
  // find between which control points pos lies
  int i;
  // If it is lower than lowest control point get the lowest colour
  if (pos < _giza_colour_table.controlPoints[0])
    {
      *red = _giza_colour_table.red[0];
      *green = _giza_colour_table.green[0];
      *blue = _giza_colour_table.blue[0];
      return;
    }
  // If it is higher than the highest control point get the highest colour
  if (pos > _giza_colour_table.controlPoints[_giza_colour_table.n - 1])
    {
      *red = _giza_colour_table.red[_giza_colour_table.n - 1];
      *green = _giza_colour_table.green[_giza_colour_table.n - 1];
      *blue = _giza_colour_table.blue[_giza_colour_table.n - 1];
      return;
    }
  // Other wise look in the table
  for (i = 1; i < _giza_colour_table.n; i++)
    {
      if (pos < _giza_colour_table.controlPoints[i])
	{
	  double fraction, d_giza_colour_table;
	  // find where between the two control points pos lies
	  d_giza_colour_table = (_giza_colour_table.controlPoints[i] - _giza_colour_table.controlPoints[i - 1]);
	  if (fabs (d_giza_colour_table) > GIZA_TINY)
	    {
	      fraction = (pos - _giza_colour_table.controlPoints[i - 1]) / d_giza_colour_table;
	    }
	  else
	    {
	      fraction = 1.;
	    }
	  // set the r,g,b using this fraction
	  *red = _giza_colour_table.red[i - 1] + fraction * (_giza_colour_table.red[i] - _giza_colour_table.red[i - 1]);
	  *green =
	    _giza_colour_table.green[i - 1] + fraction * (_giza_colour_table.green[i] -
					    _giza_colour_table.green[i - 1]);
	  *blue =
	    _giza_colour_table.blue[i - 1] + fraction * (_giza_colour_table.blue[i] - _giza_colour_table.blue[i - 1]);
	  return;
	}
    }
}

/**
 * Settings: giza_rgb_from_table_float
 *
 * Synopsis: Same functionality giza_rgb_from_table but takes floats
 */
void
giza_rgb_from_table_float (double pos, float *red, float *green, float *blue)
{
  double dred, dgreen, dblue;

  giza_rgb_from_table (pos, &dred, &dgreen, &dblue);

  *red = (float) dred;
  *green = (float) dgreen;
  *blue = (float) dblue;
}

/**
 * Sets up memory for the colour table and establishes a grey scale colour table
 */
void
_giza_init_colour_table ()
{
  _giza_colour_table.controlPoints = malloc (2 * sizeof(double));
  _giza_colour_table.red = malloc (2 * sizeof(double));
  _giza_colour_table.green = malloc (2 * sizeof(double));
  _giza_colour_table.blue = malloc (2 * sizeof(double));

  double cp[2], r[2], g[2], b[2];
  cp[0] = 0.;
  r[0] = 0.;
  g[0] = 0.;
  b[0] = 0.;
  cp[1] = 1.;
  r[1] = 1.;
  g[1] = 1.;
  b[1] = 1.;

  giza_set_colour_table (cp, r, g, b, 2);
}

/**
 * Frees the memory associated with the colour table
 */
void
_giza_free_colour_table ()
{
  free (_giza_colour_table.controlPoints);
  free (_giza_colour_table.red);
  free (_giza_colour_table.green);
  free (_giza_colour_table.blue);
}
