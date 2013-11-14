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

#include "giza-io-private.h"
#include <math.h>
#include "giza-private.h"
#include <stdlib.h>

/**
 * Checks if the currently selected device is open.
 *
 * Return value:
 *  -1 if the currently selected device is open.
 *  -0 if the currently selected device is not open
 * Input:
 *  -source :- A string that should contain the name of
 *     the function calling _giza_check_device_ready so it can
 *     be included in an error message that will be printed
 *     to stderr if no device is open.
 */
int
_giza_check_device_ready (char *source)
{
  if (id < 0 || id >= GIZA_MAX_DEVICES) 
    {

    _giza_error(source, "No device open.");
    return 0;

    }
  else if (!Dev[id].deviceOpen)
    {
      _giza_error (source, "No device open.");
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
