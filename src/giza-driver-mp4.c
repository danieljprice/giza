/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2023 Daniel Price
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
#include "giza-driver-png-private.h"
#include "giza-driver-mp4-private.h"
#include <giza.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define GIZA_DEVICE_EXTENSION ".mp4"
#define GIZA_FFMPEG_FLAGS_DEFAULT "-r 10 -vb 50M -bt 100M -pix_fmt yuv420p -vf setpts=4.*PTS"

/**
 * Closes an open mp4 device. This closes the sequence of png images
 * and compiles them into a movie
 */
void
_giza_close_device_mp4 (int last)
{
  _giza_close_device_png(last);
  if (Dev[id].drawn && last==1) {
     /* construct filename for mp4 file */
     int lenext = strlen(GIZA_DEVICE_EXTENSION);
     int length = strlen(Dev[id].prefix) + lenext + 5;
     char fileName[length + 1];
     _giza_get_filename_for_device(fileName, Dev[id].prefix, 0, GIZA_DEVICE_EXTENSION, 1);

     /* delete the existing mp4 file if it exists */
     if (access(fileName, F_OK) != -1)
       {
         remove(fileName);
       }

     /* allow the user to change the ffmpeg flags with an environment variable */
     char *userFlags = getenv("GIZA_FFMPEG_FLAGS");
     char *flagsToUse = userFlags ? userFlags : GIZA_FFMPEG_FLAGS_DEFAULT;

     /* construct ffmpeg command and repeat it to the user */
     char command[512]; /* Adjust size as needed */
     sprintf(command, "ffmpeg -i %s_%%04d.png %s %s", Dev[id].prefix,flagsToUse,fileName);
     _giza_message(command);

     /* issue command and check that it succeeded */
     int success = system(command);
     if (success != 0) {
        _giza_message("Error: ffmpeg command failed: please install ffmpeg using your package manager");
     } else {
       char tmp[length + 10];
       sprintf(tmp, "%s created", fileName);
       _giza_message(tmp);

       /* if the mp4 file exists, clean up the temporary .png files */
       if (access(fileName, F_OK) != -1)
       {
           for (int i = 0; i <= Dev[id].pgNum; i++)
           {
               _giza_get_filename_for_device(fileName, Dev[id].prefix, i, ".png", 0);

               if (access(fileName, F_OK) != -1)
               {
                   if (remove(fileName) != 0)
                   {
                       printf("Unable to delete %s \n",fileName);
                   }
               }
           }
       }
     }
  }
}

