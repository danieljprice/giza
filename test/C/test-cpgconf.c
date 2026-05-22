/* Test cpgconf (fill between two contour levels) */
#include <cpgplot.h>
#include <math.h>

#define NX 100
#define NY 100

int main()
{
  float data[NX * NY];
  float tr[6];
  int i, j;

  /* data = x * cos(3*x*y) over [-1.5, 1.5] */
  tr[0] = -1.5f - 3.0f / NX;
  tr[1] = 3.0f / NX;
  tr[2] = 0.0f;
  tr[3] = -1.5f - 3.0f / NY;
  tr[4] = 0.0f;
  tr[5] = 3.0f / NY;

  for (j = 0; j < NY; j++) {
    for (i = 0; i < NX; i++) {
      float x = tr[0] + tr[1] * (i + 1);
      float y = tr[3] + tr[5] * (j + 1);
      data[j * NX + i] = x * cosf(3.0f * x * y);
    }
  }

  if (cpgopen("test-cpgconf.png/png") <= 0) return 1;

  cpgenv(-1.5f, 1.5f, -1.5f, 1.5f, 1, 0);
  cpglab("X", "Y", "cpgconf: filled contours");

  /* Fill bands with different colors */
  float levels[] = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};
  int colors[]   = {2, 3, 5, 7};  /* red, green, cyan, yellow */
  int nlev = 5;

  for (i = 0; i < nlev - 1; i++) {
    cpgsci(colors[i]);
    cpgconf(data, NX, NY, 1, NX, 1, NY, levels[i], levels[i + 1], tr);
  }

  /* Overlay line contours in black */
  cpgsci(1);
  float cont[] = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};
  cpgcont(data, NX, NY, 1, NX, 1, NY, cont, 5, tr);

  cpgbox("BCNTS", 0.0f, 0, "BCNTS", 0.0f, 0);

  cpgend();
  return 0;
}
