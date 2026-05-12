/* Test cpgpnts (per-point markers), cpgscrn (color by name),
 * cpgqndt/cpgqdt (device type queries)
 */
#include <cpgplot.h>
#include <stdio.h>

int main()
{
  /* Test cpgqndt / cpgqdt before opening device */
  int ndt;
  cpgqndt(&ndt);
  printf("Number of device types: %d\n", ndt);

  int i;
  for (i = 1; i <= ndt; i++) {
    char type[65], descr[65];
    int tlen, dlen, inter;
    cpgqdt(i, type, &tlen, descr, &dlen, &inter);
    printf("  Device %d: type='%s' descr='%s' interactive=%d\n",
           i, type, descr, inter);
  }

  if (cpgopen("test-cpgpnts.png/png") <= 0) return 1;

  cpgenv(0.0, 11.0, 0.0, 4.0, 0, 0);
  cpglab("X", "Y", "cpgpnts + cpgscrn test");

  /* Draw 10 points with different symbols */
  int n = 10;
  float x[10], y[10];
  int symbols[10];
  for (i = 0; i < n; i++) {
    x[i] = (float)(i + 1);
    y[i] = 2.0f;
    symbols[i] = i + 1;  /* symbols 1..10 */
  }

  /* Test cpgscrn with known color */
  int ier;
  cpgscrn(1, "red", &ier);
  printf("cpgscrn('red'): ier=%d (expect 0)\n", ier);

  cpgsci(1);
  cpgpnts(n, x, y, symbols, n);

  /* Change color and draw again offset */
  cpgscrn(1, "blue", &ier);
  printf("cpgscrn('blue'): ier=%d (expect 0)\n", ier);
  for (i = 0; i < n; i++) y[i] = 3.0f;
  cpgpnts(n, x, y, symbols, n);

  /* Test cpgscrn with green */
  cpgscrn(1, "green", &ier);
  printf("cpgscrn('green'): ier=%d (expect 0)\n", ier);
  for (i = 0; i < n; i++) y[i] = 1.0f;
  cpgpnts(n, x, y, symbols, n);

  /* Test cpgscrn with unknown name */
  cpgscrn(1, "nosuchcolor", &ier);
  printf("cpgscrn('nosuchcolor'): ier=%d (expect 1)\n", ier);

  /* Test cpgpnts with ns < n (last symbol repeats) */
  cpgscrn(1, "black", &ier);
  for (i = 0; i < n; i++) { x[i] = (float)(i + 1); y[i] = 0.5f; }
  cpgpnts(n, x, y, symbols, 3);  /* only 3 symbols, rest use symbol[2] */

  cpgend();
  return 0;
}
