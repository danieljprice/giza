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

#include "giza-io-private.h"
#include <math.h>
#include "giza-private.h"

/* Indicates if a device is open and ready to be drawn to. */
static int deviceOpen = 0;
/* Indicates if the user has specified a size for the device when opening it */
static int sizeSpecified = 0;
/* Indicates if the device has been drawn to (determines if change page does anything */
static int drawn = 0;

/**
 * Checks if a device is open.
 *
 * Return value:
 *  -1 if a device is open.
 *  -0 if a device is not open
 * Input:
 *  -source :- A string that should contain the name of
 *     the function calling _giza_check_device_ready so it can
 *     be included in an error message that will be printed
 *     to stderr if no device is open.
 */
int
_giza_check_device_ready (char *source)
{
  if (!deviceOpen)
    {
      _giza_error (source, "No device open.");
      return 0;
    }
  return 1;
}

/**
 * Sets the private variable deviceOpen to one.
 */
void
_giza_set_deviceOpen (void)
{
  deviceOpen = 1;
  drawn = 0;
}

/**
 * Queries whether or not device is open (no warnings)
 */
int
_giza_get_deviceOpen (void)
{
  return deviceOpen;
}

/**
 * Sets the private variable deviceOpen to zero.
 */
void
_giza_reset_deviceOpen (void)
{
  deviceOpen = 0;
  drawn = 0;
}

/**
 * Checks if a device size has been specified by the user.
 *
 * Return value:
 *  -1 if it has been specified.
 *  -0 if it has not been specified.
 */
int
_giza_sizeSpecified (void)
{
  return sizeSpecified;
}

/**
 * Sets the private variable sizeSpecified to one.
 */
void
_giza_set_sizeSpecified (void)
{
  sizeSpecified = 1;
}

/**
 * Sets the private variable sizeSpecified to zero.
 */
void
_giza_reset_sizeSpecified (void)
{
  sizeSpecified = 0;
}

/**
 * Set the private variable drawn to 1.
 */
void
_giza_set_drawn (void)
{
  drawn = 1;
}

/**
 * Set the private variable drawn to 0.
 */
void
_giza_reset_drawn (void)
{
  drawn = 0;
}

/**
 * Checks it something has been drawn to the device
 */
int
_giza_has_drawn (void)
{
  return drawn;
}

int
_giza_nint (double x)
{
  if (x < 0)
    return (int) (x - 0.5);

  return (int) (x + 0.5);
}

int
_giza_equal(double x1, double x2)
{
  if (fabs(x1-x2) <= GIZA_TINY) {
     return 1;
  } else {
     return 0;
  }
}

