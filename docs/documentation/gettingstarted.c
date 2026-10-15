/* giza example program — see COPYING.LESSER in the top-level directory.
 * Copyright (c) 2018 Daniel Price
 * This program is free software; redistribute under the terms of the
 * GNU Lesser General Public License (LGPL, see COPYING.LESSER for details).
 */

#include <giza.h>

int
main (int argc, char **argv)
{
  int error;
  // Open the giza device
  error = giza_open_device ("?", "Getting_Started");

  if (error) {
    // Something went wrong so exit
    return 1;
  }

  // Set up the drawing environment
  giza_set_environment (-10., 10., 0., 100., 0, 0);
  
  // Draw a line
  int i, n = 100;
  double xpts[n], ypts[n];

  for (i = 0; i < n; ++i)
    {
      xpts[i] = -10. + (double) i * 20. / (double) (n - 1);
      ypts[i] = xpts[i] * xpts[i];
    }

  giza_line (n, xpts, ypts);

  // Label the axes
  giza_annotate ("B", 2.5, 0.5, 0.5, "Furlongs");
  giza_annotate ("L", 2.5, 0.5, 0.5, "Parsecs");

  // Close the giza device
  giza_close_device ();
  return 0;
}
