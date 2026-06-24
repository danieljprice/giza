/* giza test — runtime glyph fallback for missing Unicode glyphs */

#include "giza-text-private.h"
#include "giza-private.h"
#include <giza.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int
open_null_times (void)
{
  int device_id;

  device_id = giza_open_device ("/null", "test-glyph-fallback");
  if (device_id <= 0)
    return 0;
  giza_set_font ("Times");
  return 1;
}

static int
test_text_fallback (uint32_t unicode_value, const char *utf8_input,
                    const char *expected_fallback)
{
  char output_text[64];
  char input_text[16];
  const char *result;

  if (strlen (utf8_input) >= sizeof input_text)
    return 0;
  strcpy (input_text, utf8_input);

  if (_giza_font_has_glyph (unicode_value))
    {
      printf ("Skipping U+%04X: glyph present in Times\n", (unsigned) unicode_value);
      return 1;
    }

  result = _giza_apply_glyph_fallback (input_text, output_text, sizeof output_text);
  if (strcmp (result, expected_fallback) != 0)
    {
      fprintf (stderr,
               "U+%04X fallback: got \"%s\", expected \"%s\"\n",
               (unsigned) unicode_value, result, expected_fallback);
      return 0;
    }
  return 1;
}

static int
test_degree_fallback_chained (void)
{
  char output_text[64];
  const char *result;
  const char *expected;

  if (_giza_font_has_glyph (0x00b0))
    {
      printf ("Skipping degree chain: U+00B0 present in Times\n");
      return 1;
    }

  expected = _giza_font_has_glyph (0x1d52) ? "\u1d52" : "o";
  result = _giza_apply_glyph_fallback ("\xc2\xb0", output_text, sizeof output_text);
  if (strcmp (result, expected) != 0)
    {
      fprintf (stderr,
               "U+00B0 chained fallback: got \"%s\", expected \"%s\"\n",
               result, expected);
      return 0;
    }
  return 1;
}

static int
test_marker_fallback (uint32_t unicode_value, const char *utf8_input,
                      int expected_marker)
{
  int marker_number = 0;
  double width = 0.;
  double marker_width = 0.;

  if (_giza_font_has_glyph (unicode_value))
    {
      printf ("Skipping U+%04X marker fallback: glyph present in Times\n",
              (unsigned) unicode_value);
      return 1;
    }

  if (!_giza_try_marker_fallback (utf8_input, &marker_number))
    {
      fprintf (stderr, "Expected marker fallback for U+%04X\n",
               (unsigned) unicode_value);
      return 0;
    }
  if (marker_number != expected_marker)
    {
      fprintf (stderr, "U+%04X: expected marker %d, got %d\n",
               (unsigned) unicode_value, expected_marker, marker_number);
      return 0;
    }

  _giza_get_markerheight (&marker_width);
  marker_width *= 2.;
  _giza_draw_marker_at_pen (0., 0., 1., 0., marker_number, &width);
  if (fabs (width - marker_width) > 1e-10)
    {
      fprintf (stderr,
               "U+%04X marker width: got %g, expected %g\n",
               (unsigned) unicode_value, width, marker_width);
      return 0;
    }
  return 1;
}

static int
test_ascii_unchanged (void)
{
  char output_text[64];
  const char *result;

  result = _giza_apply_glyph_fallback ("ABC", output_text, sizeof output_text);
  if (strcmp (result, "ABC") != 0)
    {
      fprintf (stderr, "ASCII changed: got \"%s\"\n", result);
      return 0;
    }
  return 1;
}

int
main (void)
{
  int failed = 0;

  if (!open_null_times ())
    {
      fprintf (stderr, "Failed to open null device\n");
      return EXIT_FAILURE;
    }

  if (!test_ascii_unchanged ())
    failed++;
  if (!test_text_fallback (0x2032, "\xe2\x80\xb2", "'"))
    failed++;
  if (!test_text_fallback (0x2033, "\xe2\x80\xb3", "\""))
    failed++;
  if (!test_text_fallback (0x00a9, "\xc2\xa9", "(c)"))
    failed++;
  if (!test_degree_fallback_chained ())
    failed++;
  if (!test_text_fallback (0x2018, "\xe2\x80\x98", "'"))
    failed++;
  if (!test_text_fallback (0x2019, "\xe2\x80\x99", "'"))
    failed++;
  if (!test_text_fallback (0x00d7, "\xc3\x97", "x"))
    failed++;
  if (!test_text_fallback (0x22c5, "\xe2\x8b\x85", "."))
    failed++;
  if (!test_text_fallback (0x00b7, "\xc2\xb7", "."))
    failed++;
  if (!test_text_fallback (0x2212, "\xe2\x88\x92", "-"))
    failed++;
  if (!test_text_fallback (0x00b1, "\xc2\xb1", "+/-"))
    failed++;
  if (!test_text_fallback (0x2213, "\xe2\x88\x93", "-/+"))
    failed++;
  if (!test_text_fallback (0x212b, "\xe2\x84\xab", "A"))
    failed++;
  if (!test_marker_fallback (0x2609, "\xe2\x98\x89", 9))
    failed++;
  if (!test_marker_fallback (0x2190, "\xe2\x86\x90", 28))
    failed++;
  if (!test_marker_fallback (0x2192, "\xe2\x86\x92", 29))
    failed++;
  if (!test_marker_fallback (0x2191, "\xe2\x86\x91", 30))
    failed++;
  if (!test_marker_fallback (0x2193, "\xe2\x86\x93", 31))
    failed++;

  giza_close_device ();

  if (failed)
    return EXIT_FAILURE;

  printf ("Glyph fallback test passed (C)\n");
  return EXIT_SUCCESS;
}
