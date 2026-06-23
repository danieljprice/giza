/* giza test program — see COPYING.LESSER in the top-level directory.
 * This library is free software; redistribute under the terms of the
 * GNU Lesser General Public License (LGPL, see COPYING.LESSER for details).
 */

/* Test cpgscrl (scroll window) */
#include <cpgplot.h>
#include <stdio.h>
#include <math.h>

int main()
{
  if (cpgopen("test-cpgscrl.png/png") <= 0) return 1;

  /* Draw a sine curve */
  cpgenv(0.0f, 10.0f, -1.5f, 1.5f, 0, 0);
  cpglab("X", "Y", "cpgscrl test: before scroll");

  int n = 100;
  float x[100], y[100];
  int i;
  for (i = 0; i < n; i++) {
    x[i] = (float)i * 10.0f / (n - 1);
    y[i] = sinf(x[i]);
  }
  cpgsci(2);
  cpgline(n, x, y);

  /* Page 2: draw same curve, then scroll */
  cpgpage();
  cpgenv(0.0f, 10.0f, -1.5f, 1.5f, 0, 0);

  cpgsci(2);
  cpgline(n, x, y);

  /* Scroll window right by 2.0 */
  cpgscrl(2.0f, 0.5f);

  /* Verify the window shifted */
  float x1, x2, y1, y2;
  cpgqwin(&x1, &x2, &y1, &y2);
  printf("After cpgscrl(2.0, 0.5): window = [%f, %f] x [%f, %f]\n",
         x1, x2, y1, y2);
  printf("Expected: [2.0, 12.0] x [-1.0, 2.0]\n");

  /* Draw box with new window coords to show the shift */
  cpgsci(1);
  cpgbox("BCNTS", 0.0f, 0, "BCNTS", 0.0f, 0);
  cpglab("X", "Y", "After cpgscrl(2.0, 0.5)");

  cpgend();
  return 0;
}
