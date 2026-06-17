/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 5a) of the GPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
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

/* Regression test for issue #73: Unicode characters in plot strings. */

#include <giza.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int check_png_file (const char *filename);
static int check_text_length (const char *text);

int
main (void)
{
  int id;
  const char *unicode = "\xc2\xa9 Giza";
  const char *outfile = "test-unicode.png";
  double xbox[4], ybox[4];

  if (!check_text_length (unicode))
    {
      fprintf (stderr, "Error: Unicode string has zero measured width\n");
      return 1;
    }

  id = giza_open_device ("/png", "test-unicode");
  if (id <= 0)
    {
      fprintf (stderr, "Error: giza_open_device failed\n");
      return 1;
    }

  giza_start_warnings ();
  giza_set_environment (0., 1., 0., 1., 0, 0);
  giza_box ("BCNT", 0., 0, "BCNT", 0., 0);
  giza_qtext (0.5, 0.5, 0., 0.5, unicode, xbox, ybox);
  giza_ptext (0.5, 0.5, 0., 0.5, unicode);
  giza_label ("x", "y", unicode);
  giza_close_device ();

  if (!check_png_file (outfile))
    {
      return 1;
    }

  printf ("Unicode text test passed (C)\n");
  return 0;
}

static int
check_text_length (const char *text)
{
  double xlen, ylen;

  giza_open_device ("/null", "test-unicode-len");
  giza_qtextlen (GIZA_UNITS_NORMALIZED, text, &xlen, &ylen);
  giza_close_device ();

  return (xlen > 0. && ylen > 0.);
}

static int
check_png_file (const char *filename)
{
  if (access (filename, F_OK) == -1)
    {
      fprintf (stderr, "Error: required file %s does not exist\n", filename);
      return 0;
    }

  printf ("File %s exists -> OK\n", filename);
  return 1;
}
