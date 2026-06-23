/* giza test program — see COPYING.LESSER in the top-level directory.
 * This library is free software; redistribute under the terms of the
 * GNU Lesser General Public License (LGPL, see COPYING.LESSER for details).
 */

/* Test cpgconl (labeled contours) */
#include <cpgplot.h>
#include <math.h>
#include <stdio.h>

#define NX 80
#define NY 80

int main()
{
  float data[NX * NY];
  float tr[6];
  int i, j;

  /* data = sin(x)*cos(y) over [-pi, pi] */
  tr[0] = -M_PI - 2.0 * M_PI / NX;
  tr[1] = 2.0 * M_PI / NX;
  tr[2] = 0.0;
  tr[3] = -M_PI - 2.0 * M_PI / NY;
  tr[4] = 0.0;
  tr[5] = 2.0 * M_PI / NY;

  for (j = 0; j < NY; j++) {
    for (i = 0; i < NX; i++) {
      float x = tr[0] + tr[1] * (i + 1);
      float y = tr[3] + tr[5] * (j + 1);
      data[j * NX + i] = sinf(x) * cosf(y);
    }
  }

  if (cpgopen("test-cpgconl.png/png") <= 0) return 1;

  cpgenv(-M_PI, M_PI, -M_PI, M_PI, 1, 0);
  cpglab("X", "Y", "cpgconl: labeled contours of sin(x)cos(y)");

  /* Draw labeled contours at several levels */
  float levels[] = {-0.5f, 0.0f, 0.5f};
  const char *labels[] = {"-0.5", "0.0", "+0.5"};
  int colors[] = {2, 1, 4};
  int intval = 20;
  int minint = 10;

  for (i = 0; i < 3; i++) {
    cpgsci(colors[i]);
    cpgconl(data, NX, NY, 1, NX, 1, NY, levels[i], tr,
            labels[i], intval, minint);
  }

  cpgsci(1);
  cpgbox("BCNTS", 0.0f, 0, "BCNTS", 0.0f, 0);

  cpgend();
  return 0;
}
