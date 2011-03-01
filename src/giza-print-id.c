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
#include "giza-transforms-private.h"
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
  /* get user id */
  char *userid = getlogin();
  if (userid == NULL) userid = "";
  
  /* get current date and time */
  struct tm *current;
  time_t now;
  time(&now);
  current = localtime(&now);

  /* format the date and time into a string */
  char date[20];
  strftime(date,sizeof(date)," %e-%h-%Y %H:%M",current);
  char *string = strcat(userid,date);
  
  double ch;
  giza_get_character_height(&ch);
  giza_set_character_height(0.6);

  /* query dimensions of the surface and size of string */
  double dx,dy,x,y,width,height;
  giza_get_paper_size(GIZA_UNITS_DEVICE,&width,&height);
  giza_qtextlen(GIZA_UNITS_DEVICE,string,&dx,&dy);
  
  /* place text at bottom right corner of surface */
  x = width - 0.1*dx;
  y = height - dy;

  /* convert this to world coordinates */
  int oldtrans = _giza_get_trans();
  _giza_set_trans(GIZA_TRANS_WORLD);
  cairo_device_to_user (context, &x, &y);
  
  /* print text at location */
  giza_ptext(x,y,0.,1.,string);
  
  /* reset settings */
  _giza_set_trans(oldtrans);
  giza_set_character_height(ch);
}
