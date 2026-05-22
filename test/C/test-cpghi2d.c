/* Test cpghi2d (waterfall / cross-section plot) */
#include <cpgplot.h>
#include <math.h>

#define NXV 40
#define NYV 20

int main()
{
  float data[NXV * NYV];
  float x[NXV];
  float ylims[NXV];
  int i, j;

  /* Create data: sin wave that decays with row */
  for (j = 0; j < NYV; j++) {
    for (i = 0; i < NXV; i++) {
      float xi = (float)i / 5.0f;
      float decay = expf(-(float)j / 10.0f);
      data[j * NXV + i] = sinf(xi) * decay;
    }
  }

  /* X coordinates */
  for (i = 0; i < NXV; i++) {
    x[i] = (float)i;
  }

  if (cpgopen("test-cpghi2d.png/png") <= 0) return 1;

  cpgenv(0.0f, 50.0f, -1.5f, 5.0f, 0, 0);
  cpglab("X", "Y", "cpghi2d: waterfall plot");

  cpgsci(1);
  cpghi2d(data, NXV, NYV, 1, NXV, 1, NYV, x, 0, 0.2f, 0, ylims);

  /* Page 2: with horizontal offset */
  cpgpage();
  cpgenv(0.0f, 60.0f, -1.5f, 5.0f, 0, 0);
  cpglab("X", "Y", "cpghi2d: waterfall with ioff=1");

  cpgsci(2);
  cpghi2d(data, NXV, NYV, 1, NXV, 1, NYV, x, 1, 0.2f, 0, ylims);

  cpgend();
  return 0;
}
