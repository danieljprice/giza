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
#include "giza-drivers-private.h"
#include "giza-io-private.h"
#include <math.h>
#include <stdlib.h>

/**
 * Checks if the currently selected device is open (but not necessarily bound
 * to a cairo context on GIZA_DEVICE_CAIRO).
 */
int
_giza_check_device_open (char *source)
{
  if (id < 0 || id >= GIZA_MAX_DEVICES)
    {
      _giza_error (source, "No device open.");
      return 0;
    }
  if (!Dev[id].deviceOpen)
    {
      _giza_error (source, "No device open.");
      return 0;
    }
  return 1;
}

/**
 * Checks if the device is ready to draw: open, and for external cairo devices
 * a caller-owned context must be bound via giza_set_cairo_context.
 */
int
_giza_check_device_ready (char *source)
{
  if (!_giza_check_device_open (source))
    return 0;

  if (Dev[id].type == GIZA_DEVICE_CAIRO && Dev[id].context == NULL)
    {
      _giza_error (source, "No cairo context bound (call giza_set_cairo_context first).");
      return 0;
    }
  return 1;
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

void _giza_init(void)
{
   Sets.autolog = 0;
   char *tmp = getenv ("GIZA_LOG");
   if(tmp)
     {
       Sets.autolog = 1;
     }
}
