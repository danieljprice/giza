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
 * Copyright (C) 2010 James Wetter & Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com, daniel.price@sci.monash.edu.au
 *
 */

#include "giza-private.h"
#include "giza-drivers-private.h"
#include "giza-io-private.h"
#include "giza-warnings-private.h"
#include <stdio.h>
#include <string.h>

/**
 * If warnings are switched on displays a warning message to stderr
 */
void
_giza_warning (char *source, char *message)
{
  if (_giza_get_warnings ())
    {
      fprintf (stderr, "%%giza - Warning - %s: %s\n", source, message);
    }
}

/**
 * Prints a message to stdout
 */
void
_giza_message (char *message)
{
   fprintf (stdout, "%%giza: %s\n", message); 
}

/**
 * Prints the error to stderr.
 */
void
_giza_error (char *source, char *message)
{
  fprintf (stderr, "%%giza - ERROR - %s: %s\n", source, message);
}

/**
 * NOTE: should have a default.
 * Prompts the user to enter a device
 */
int
_giza_prompt_for_device (void)
{
  char input[20];
  int count = 0;
  int devType = GIZA_DEVICE_IV;
  /*int err;*/

  do
    {
      printf ("Graphics device/type (? to see list, default /xw):");
      /* err = scanf ("%s", input); */
      /*
       *  Use fgets here instead of scanf so that we can accept a
       *  newline character as the default input
       */
      char *p = fgets(input,20,stdin);
      if (p != NULL)
        {
          size_t last = strlen(input) - 1;
          if (input[last] == '\n') 
             {
               if (last == 0)
                 {
                   devType = _giza_default_device ();
                   break;
                 } 
               else
                 {
                   input[last] = '\0';
                 }
             }

          if (!p)
	    {
	      _giza_error ("_giza_prompt_for_device", "Error reading input");
	    }
          else
	    {
	      char *devTypeStr;
	      _giza_split_device_string (input, &devTypeStr);
	      devType = _giza_device_to_int (devTypeStr);
	    }

          if (devType == GIZA_DEVICE_IV)
	    {
	      _giza_display_devices ();
	    }
          else
	    {
	      break;
	    }
        }
      count++;
    }
  while (count>0); //(count < 3);

  return devType;
}

/**
 * This routine prints the available devices.
 */
void
_giza_display_devices (void)
{
  char *deviceList;
  _giza_init_device_list (&deviceList);
  printf ("%s\n", deviceList);
  _giza_free_device_list (deviceList);
}

void
_giza_newpage_prompt (void)
{ 
  char input[2];
  printf (" Press RETURN for next page: ");
  /*
   *  Use fgets here instead of scanf so that we can accept a
   *  newline character as the default input
   */
  char *p = fgets(input,2,stdin);
}

/**
 * This routine gives the default device
 */
int
_giza_default_device (void)
{
  return GIZA_DEVICE_XW;
}
