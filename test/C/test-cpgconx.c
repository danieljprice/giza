/* Test cpgconx (contour with user callback) */
#include <cpgplot.h>
#include <math.h>

/* User callback: simple linear transform (equivalent to a tr matrix)
 * Maps grid indices to world coords and calls cpgmove/cpgdraw */
static void myplot(int *visble, float *x, float *y, float *z)
{
  /* Simple identity-ish mapping: world = grid * scale + offset */
  float scale = 6.2832f / 50.0f;  /* 2*pi / N */
  float offset = -3.14159f;
  float wx = (*x - 0.5f) * scale + offset;
  float wy = (*y - 0.5f) * scale + offset;

  if (*visble == 0)
    cpgmove(wx, wy);
  else
    cpgdraw(wx, wy);
}

#define NX 50
#define NY 50

int main()
{
  float data[NX * NY];
  int i, j;
  float pi = 3.14159f;

  for (j = 0; j < NY; j++) {
    for (i = 0; i < NX; i++) {
      float x = -pi + (float)i * 2.0f * pi / (float)NX;
      float y = -pi + (float)j * 2.0f * pi / (float)NY;
      data[j * NX + i] = sinf(x) * cosf(y);
    }
  }

  if (cpgopen("test-cpgconx.png/png") <= 0) return 1;

  cpgenv(-pi, pi, -pi, pi, 1, 0);
  cpglab("X", "Y", "cpgconx: contour with user callback");

  float contours[] = {-0.8f, -0.4f, 0.0f, 0.4f, 0.8f};
  cpgsci(2);
  cpgconx(data, NX, NY, 1, NX, 1, NY, contours, 5, myplot);

  cpgsci(1);
  cpgbox("BCNTS", 0.0f, 0, "BCNTS", 0.0f, 0);

  cpgend();
  return 0;
}
