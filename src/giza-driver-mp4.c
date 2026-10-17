/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2023 Daniel Price
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
#include "giza-driver-png-private.h"
#include "giza-driver-mp4-private.h"
#include <giza.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define GIZA_DEVICE_EXTENSION ".mp4"
#define GIZA_FFMPEG_FLAGS_DEFAULT "-r 10 -vb 50M -bt 100M -pix_fmt yuv420p -vf setpts=4.*PTS -loglevel quiet"

/* paths: alnum / _ - .   flags: same plus space and -_.=:/%* */
static int
_giza_mp4_ok (const char *s, int paths)
{
  const char *extra = paths ? "/_-." : "-_.=:/%*";
  if (!s || !*s)
    return 0;
  for (; *s; s++)
    {
      unsigned char c = (unsigned char) *s;
      if (!paths && (c == ' ' || c == '\t'))
        continue;
      if (!isalnum (c) && !strchr (extra, (int) c))
        return 0;
    }
  return 1;
}

static void
_giza_mp4_ffmpeg (const char *prefix, const char *out, const char *flags)
{
  char input[GIZA_MAX_DEVSTRING + 16], *copy, *save, *tok, *av[64];
  int n = 0;
  pid_t pid;

  snprintf (input, sizeof (input), "%s_%%04d.png", prefix);
  copy = strdup (flags);
  if (!copy)
    return;
  av[n++] = "ffmpeg";
  av[n++] = "-i";
  av[n++] = input;
  for (tok = strtok_r (copy, " \t", &save); tok && n < 62; tok = strtok_r (NULL, " \t", &save))
    av[n++] = tok;
  av[n++] = (char *) out;
  av[n] = NULL;
  if ((pid = fork ()) == 0)
    {
      execvp ("ffmpeg", av);
      _exit (127);
    }
  if (pid > 0)
    waitpid (pid, NULL, 0);
  free (copy);
}

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
     char *userFlags = getenv("GIZA_FFMPEG_FLAGS");
     char const *flagsToUse = userFlags ? userFlags : GIZA_FFMPEG_FLAGS_DEFAULT;

     _giza_get_filename_for_device(fileName, Dev[id].prefix, 0, GIZA_DEVICE_EXTENSION, 1);

     /* delete the existing mp4 file if it exists */
     if (remove(fileName) != 0 && errno != ENOENT)
       _giza_message("Warning: could not remove existing mp4 file");

     if (!_giza_mp4_ok (Dev[id].prefix, 1) || !_giza_mp4_ok (fileName, 1))
       {
         _giza_message ("Error: unsafe characters in mp4 path; ffmpeg not run");
         return;
       }
     if (!_giza_mp4_ok (flagsToUse, 0))
       {
         if (userFlags)
           _giza_message ("Warning: invalid GIZA_FFMPEG_FLAGS; using defaults");
         flagsToUse = GIZA_FFMPEG_FLAGS_DEFAULT;
       }

     /* construct ffmpeg command and repeat it to the user */
     int mylen = strlen(Dev[id].prefix) + strlen(flagsToUse) + strlen(fileName) + 50;
     char command[mylen + 1];
    snprintf(command, sizeof(command), "GIZA_FFMPEG_FLAGS='%s'", flagsToUse);
     _giza_message(command);
    snprintf(command, sizeof(command), "ffmpeg -i %s_%%04d.png $GIZA_FFMPEG_FLAGS %s", Dev[id].prefix,fileName);
     _giza_message(command);
    snprintf(command, sizeof(command), "ffmpeg -i %s_%%04d.png %s %s", Dev[id].prefix,flagsToUse,fileName);
     _giza_message(command);

     _giza_mp4_ffmpeg (Dev[id].prefix, fileName, flagsToUse);

     if (access (fileName, F_OK) != -1)
       {
         char tmp[length + 10];
         snprintf (tmp, sizeof (tmp), "%s created", fileName);
         _giza_message (tmp);

         /* if the mp4 file exists, clean up the temporary .png files */
         for (int i = 0; i <= Dev[id].pgNum; i++)
           {
             _giza_get_filename_for_device (fileName, Dev[id].prefix, i, ".png", 0);
             if (remove (fileName) != 0 && errno != ENOENT)
               printf ("Unable to delete %s \n", fileName);
           }
       }
     else
       {
         _giza_message ("Error: ffmpeg command failed: please install ffmpeg using your package manager");
       }
  }
}
