/* giza test — PGPLOT PGGRAY foreground/background colour semantics */

#include <cpgplot.h>
#include <stdio.h>

#define NX 8
#define NY 8

int main(void)
{
  float data[NX * NY];
  float tr[6];
  float cr, cg, cb;
  int i, j;

  if (cpgopen("/null") <= 0)
    return 1;

  /* Classic PGPLOT: background black (CI 0), foreground white (CI 1) */
  cpgqcr(0, &cr, &cg, &cb);
  if (cr > 0.1f || cg > 0.1f || cb > 0.1f)
    return 2;

  cpgqcr(1, &cr, &cg, &cb);
  if (cr < 0.9f || cg < 0.9f || cb < 0.9f)
    return 3;

  for (j = 0; j < NY; j++)
    for (i = 0; i < NX; i++)
      data[j * NX + i] = (float) (i + j);

  tr[0] = 0.0f;
  tr[1] = 1.0f;
  tr[2] = 0.0f;
  tr[3] = 0.0f;
  tr[4] = 0.0f;
  tr[5] = 1.0f;

  cpgenv(0.0f, (float) NX, 0.0f, (float) NY, 1, 0);

  /* Positive image: FG > BG */
  cpggray(data, NX, NY, 1, NX, 1, NY, 14.0f, 0.0f, tr);

  cpgpage();
  cpgenv(0.0f, (float) NX, 0.0f, (float) NY, 1, 0);

  /* Inverted image: FG < BG (issue #7) */
  cpggray(data, NX, NY, 1, NX, 1, NY, 0.0f, 14.0f, tr);

  cpgend();
  return 0;
}
