/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2015 Daniel Price
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
#include "giza-drivers-private.h"
#include "giza-io-private.h"
#include "giza-warnings-private.h"
#include <stdio.h>
#include <string.h>
#include <giza.h>
#include <time.h>

/**
 * If warnings are switched on displays a warning message to stderr
 */
void
_giza_warning (const char *source, const char *message)
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
_giza_message (const char *message)
{
   fprintf (stdout, "%%giza: %s\n", message);
}

/**
 * Prints the error to stderr.
 */
void
_giza_error (const char *source, const char *message)
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
  char input[GIZA_MAX_DEVSTRING + 5]; /* allow enough for prefix + 5 char extension */
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
      char *p = fgets(input,sizeof(input)-1,stdin);
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
      /* flush stdin here so we don't get repeated entries into fgets */
      fflush(stdin);
/*
      int ch;
      while (((ch = getchar()) != EOF) && (ch != '\n'));
*/
      count++;
    }
  while (count>0); /*(count < 3); */

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
  printf ("%%giza version %s built with cairo v%s\n\n",
          GIZA_VERSION_STRING,CAIRO_VERSION_STRING);
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
  fgets(input,2,stdin);
}

/**
 * This routine gives the default device
 */
int
_giza_default_device (void)
{
  return GIZA_DEVICE_XW;
}

/**
 * constructs file name for log file
 * using current date and time
 */
void
_giza_get_log_file (char *string, int len)
{
   time_t now;
   time(&now);
   struct tm tstruct = *localtime(&now);
   strftime(string,len,"giza-%Y-%m-%d-%H:%M:%S.png",&tstruct);
}

/**
 * writes current cairo surface to png file
 * with a filename based on the current date and time
 */
void
_giza_write_log_file (cairo_surface_t *surface)
{
   char msg[80];
   char string[80];
   _giza_get_log_file(string,sizeof(string));
   sprintf(msg,"writing %s",string);
   _giza_message(msg);
   cairo_status_t status;
   status = cairo_surface_write_to_png(surface,string);
   if (status != CAIRO_STATUS_SUCCESS)
      _giza_error("log file",cairo_status_to_string(status));
}

/**
 * Settings: giza_begin_autolog
 *
 * Synopsis: Turns on automatic logging of interactive devices
 *  This writes a png file every time the page is
 *  changed in an interactive device, with a name
 *  based on the current date/time (giza-%Y-%M-%D-%H:%M:%S.png).
 *  Logging can also be turned on by setting the GIZA_LOG
 *  environment variable.
 *
 * See Also: giza_end_autolog
 */
void giza_begin_autolog (void)
{
  if(!_giza_check_device_ready ("giza_start_autolog")) return;
  Sets.autolog = 1;
}

/**
 * Settings: giza_end_autolog
 *
 * Synopsis: Turns off automatic logging feature.
 *
 * See Also: giza_begin_autolog
 */
void giza_end_autolog (void)
{
  if(!_giza_check_device_ready ("giza_end_autolog")) return;
  Sets.autolog = 0;
}
