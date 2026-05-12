/* Test cpgconb (contour with blanking) */
#include <cpgplot.h>
#include <math.h>

#define NX 50
#define NY 50

int main()
{
  float data[NX * NY];
  float tr[6];
  int i, j;

  /* Create data: sin(x)*cos(y) mapped over [-pi, pi] */
  tr[0] = -M_PI - M_PI / NX;   /* x = tr[0] + tr[1]*I + tr[2]*J */
  tr[1] = 2.0 * M_PI / NX;
  tr[2] = 0.0;
  tr[3] = -M_PI - M_PI / NY;
  tr[4] = 0.0;
  tr[5] = 2.0 * M_PI / NY;

  for (j = 0; j < NY; j++) {
    for (i = 0; i < NX; i++) {
      float x = tr[0] + tr[1] * (i + 1);
      float y = tr[3] + tr[5] * (j + 1);
      data[j * NX + i] = sinf(x) * cosf(y);
    }
  }

  /* Set a ring of blank values where 0.8 < r < 1.2 */
  float blank = -999.0f;
  for (j = 0; j < NY; j++) {
    for (i = 0; i < NX; i++) {
      float x = tr[0] + tr[1] * (i + 1);
      float y = tr[3] + tr[5] * (j + 1);
      float r = sqrtf(x * x + y * y);
      if (r > 0.8f && r < 1.2f)
        data[j * NX + i] = blank;
    }
  }

  if (cpgopen("test-cpgconb.png/png") <= 0) return 1;

  /* Page 1: cpgconb with blanking */
  cpgenv(-M_PI, M_PI, -M_PI, M_PI, 1, 0);
  cpglab("X", "Y", "cpgconb: blanked ring (r=0.8..1.2)");

  float contours[] = {-0.8f, -0.6f, -0.4f, -0.2f, 0.0f, 0.2f, 0.4f, 0.6f, 0.8f};
  int nc = 9;

  cpgsci(1);
  cpgconb(data, NX, NY, 1, NX, 1, NY, contours, nc, tr, blank);

  /* Page 2: same data with cpgcons (no blanking) for comparison */
  cpgpage();
  cpgenv(-M_PI, M_PI, -M_PI, M_PI, 1, 0);
  cpglab("X", "Y", "cpgcons: same data, no blanking");
  cpgcons(data, NX, NY, 1, NX, 1, NY, contours, nc, tr);

  cpgend();
  return 0;
}
