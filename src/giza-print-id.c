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

#include "giza-private.h"
#include "giza-window-private.h"
#include <giza.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/**
 * Text: giza_print_id
 *
 * Synopsis: Prints user ID, date and time on the plot
 *
 */
void
giza_print_id (void)
{
  if (!_giza_check_device_ready ("giza_print_id"))
    {
      return;
    }
  char *userid = getlogin();
  if (userid == NULL) userid = "";
  
  struct tm *current;
  time_t now;
  time(&now);
  current = localtime(&now);
  char date[20];
  strftime(date,20," %e-%h-%Y %H:%M",current);
  char *string = strcat(userid,date);
  
  double ch;
  giza_get_character_height(&ch);
  giza_set_character_height(0.6);

  //double dx,dy,x,y;
  //giza_qtextlen(GIZA_UNITS_WORLD,string,dx,dy);
  giza_text(Win.xmax,Win.ymin,string);
  
  giza_set_character_height(ch);
}
