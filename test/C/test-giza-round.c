/* giza test — giza_round tick-interval rounding */

#include <giza.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  double input;
  double expect;
  int    expect_nsub;
} round_case_t;

static int
almost_equal (double a, double b)
{
  double scale = fmax (fabs (a), fabs (b));
  if (scale < 1.)
    scale = 1.;
  return fabs (a - b) < 1e-10 * scale;
}

int
main (void)
{
  static round_case_t const cases[] = {
    /* Representative inputs and expected outputs */
    { 8.7,  10.,  5 },
    { -0.4, -0.5, 5 },
    /* Boundary and decade cases */
    { 0.4,  0.5,  5 },
    { 0.5,  0.5,  5 },
    { 1.0,  2.0,  2 },
    { 2.0,  2.0,  2 },
    { 2.5,  5.0,  5 },
    { 5.0,  5.0,  5 },
    { 5.5,  10.,  5 },
    { 9.5,  10.,  5 },
    { 10.,  20.,  2 },
    { 500., 500., 5 },
    { 501., 1000., 5 },
    { 950., 1000., 5 },
    { 1000., 2000., 2 },
    { 1001., 2000., 2 },
    { 2000., 2000., 2 },
    { 5000., 5000., 5 },
    { 0., 0., 2 },
  };
  size_t i;
  int failed = 0;

  for (i = 0; i < sizeof (cases) / sizeof (cases[0]); i++)
    {
      int nsub;
      double got = giza_round (cases[i].input, &nsub);

      if (!almost_equal (got, cases[i].expect) || nsub != cases[i].expect_nsub)
        {
          fprintf (stderr,
                   "giza_round(%g): got %g (nsub=%d), expected %g (nsub=%d)\n",
                   cases[i].input, got, nsub, cases[i].expect,
                   cases[i].expect_nsub);
          failed++;
        }
    }

  return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
