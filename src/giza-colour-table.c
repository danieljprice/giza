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

#include "giza-colour-private.h"
#include "giza-private.h"
#include "giza-io-private.h"
#include <math.h>
#include <stdlib.h>
#include <giza.h>

void _giza_set_range_from_colour_table (int cimin, int cimax);
void _giza_allocate_colour_table (giza_ctab *ct, int n);
void _giza_reallocate_colour_table (giza_ctab *ct, int n);
void _giza_deallocate_colour_table (giza_ctab *ct);

giza_ctab _giza_colour_table;
giza_ctab _giza_saved_colour_tables[GIZA_CTAB_SAVE_MAX];
int _giza_save_ctab_ncalls;

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
  
  if (n < 2 || n > GIZA_CTAB_MAXSIZE)
    {
      _giza_warning ("giza_set_colour_table", "Invalid number of control points, colour table not set");
      return 1;
    }

  /*_giza_reallocate_colour_table(&_giza_colour_table, n);*/
  int i, tmpn = 0;

  for (i = 0; i < n; i++)
    {
      /* check the control point is valid */
      if(controlPoints[i] >= 0.  && controlPoints[i] <= 1.)
	{
	  if(tmpn <= 0 || controlPoints[i] > _giza_colour_table.controlPoints[tmpn - 1])
	    {
	      _giza_colour_table.controlPoints[i] = controlPoints[i];
              _giza_colour_table.red[i]   = _giza_set_in_range(red[i],0.,1.);
              _giza_colour_table.green[i] = _giza_set_in_range(green[i],0.,1.);
              _giza_colour_table.blue[i]  = _giza_set_in_range(blue[i],0.,1.);      
	      tmpn++;
	    }
	}
    }
  _giza_colour_table.n = tmpn; 
   if (tmpn < 2)
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
 * Synopsis: Same functionality giza_set_colour_table. Not simple a call to set colour table to
 * avoid memory wastage for big arrays.
 */
int
giza_set_colour_table_float (float *controlPoints, float *red, float *green, float *blue, int n)
{
  if (!_giza_check_device_ready ("giza_set_colour_table_float"))
    return 1;
  
  if (n < 2 || n > GIZA_CTAB_MAXSIZE)
    {
      _giza_warning ("giza_set_colour_table_float", "Invalid number of control points, colour table not set");
      return 1;
    }
   
  /*_giza_reallocate_colour_table(&_giza_colour_table, n);*/
  int i, tmpn = 0;

  for (i = 0; i < n; i++)
    {
      /* check the control point is valid */
      if(controlPoints[i] >= 0.  && controlPoints[i] <= 1.)
	{
	  if(tmpn <= 0 || controlPoints[i] > _giza_colour_table.controlPoints[tmpn - 1])
	    {
	      _giza_colour_table.controlPoints[i] = (double) controlPoints[i];
              _giza_colour_table.red[i]   = _giza_set_in_range((double) red[i],  0.,1.);
              _giza_colour_table.green[i] = _giza_set_in_range((double) green[i],0.,1.);
              _giza_colour_table.blue[i]  = _giza_set_in_range((double) blue[i], 0.,1.);      
	      tmpn++;
	    }
	}
    }
  _giza_colour_table.n = tmpn;
   if (tmpn < 2)
   {
     _giza_warning ("giza_set_colour_table_float", "Invalid values for control points in colour table settings");
     return 2;
   }

  /* use the installed colour table to set the colours of colour indices in the specified range */
  _giza_set_range_from_colour_table(_giza_colour_index_min,_giza_colour_index_max);

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
    
   /* this should never happen */
   if (*red < 0.) 
     {
      _giza_error("giza_rgb_from_table","Internal error, colour not set");
      *red = 0.;
      *green = 0.;
      *blue = 0.;
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
  /*_giza_allocate_colour_table (&_giza_colour_table, 2);*/

  printf("initialising colour table %i \n",_giza_save_ctab_ncalls);

  double cp[2], r[2], g[2], b[2];
  cp[0] = 0.; r[0] = 0.; g[0] = 0.; b[0] = 0.;
  cp[1] = 1.; r[1] = 1.; g[1] = 1.; b[1] = 1.;

  printf("calling set colour table %i \n",_giza_save_ctab_ncalls);
  giza_set_colour_table (cp, r, g, b, 2);
  printf("initialised colour table %i \n",_giza_save_ctab_ncalls);
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
 * Allocates the memory within a given colour table
 */
/*
void
_giza_allocate_colour_table (giza_ctab *ct, int n)
{
  (*ct).controlPoints = malloc (n * sizeof(double));
  (*ct).red   = malloc (n * sizeof(double));
  (*ct).green = malloc (n * sizeof(double));
  (*ct).blue  = malloc (n * sizeof(double));
}
*/

/**
 * Reallocates the memory associated a given colour table
 */
/*
void
_giza_reallocate_colour_table (giza_ctab *ct, int n)
{
  (*ct).controlPoints = realloc ((*ct).controlPoints, n * sizeof(double));
  (*ct).red   = realloc ((*ct).red,   n * sizeof(double));
  (*ct).green = realloc ((*ct).green, n * sizeof(double));
  (*ct).blue  = realloc ((*ct).blue,  n * sizeof(double));
}
*/

/**
 * Frees the memory associated with a given colour table
 */
/*void
_giza_deallocate_colour_table (giza_ctab *ct)
{
  free ((*ct).controlPoints);
  free ((*ct).red);
  free ((*ct).green);
  free ((*ct).blue);
}
*/

/**
 * Saves the current colour table
 */
void giza_save_colour_table (void)
{
  _giza_save_ctab_ncalls += 1;
  if (_giza_save_ctab_ncalls > GIZA_CTAB_SAVE_MAX) {
     _giza_warning("giza_save_colour_table","max number of giza_save_colour_table calls exceeded");
     return;
  }
  int i = _giza_save_ctab_ncalls - 1;

  printf("saving colour table %i with size %i\n",i,_giza_colour_table.n);
  /* allocate the next colour table in the array */
  /*_giza_allocate_colour_table(&_giza_saved_colour_tables[i],_giza_colour_table.n);*/
  
  /* save the current colour table to the array */
  _giza_saved_colour_tables[i] = _giza_colour_table;
}

/**
 * Restores the colour table from a previously saved one
 */
void giza_restore_colour_table (void)
{
  _giza_save_ctab_ncalls -= 1;
  if (_giza_save_ctab_ncalls < 0) {
     _giza_warning("giza_restore_colour_table","unmatched save/restore calls");
     _giza_save_ctab_ncalls = 0;
     return;
  }

  /* Restore the colour table from the saved array */
  int i = _giza_save_ctab_ncalls;
  /*_giza_reallocate_colour_table(&_giza_colour_table, _giza_saved_colour_tables[i].n);*/
  printf("restoring colour table %i with size %i\n",i,_giza_saved_colour_tables[i].n);
  _giza_colour_table = _giza_saved_colour_tables[i];

  printf("freeing memory %i \n",_giza_save_ctab_ncalls);
  /* Free associated memory */
  /*_giza_deallocate_colour_table(&_giza_saved_colour_tables[i]);*/
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
  return giza_set_colour_table (cp, r, g, b, 2);
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

