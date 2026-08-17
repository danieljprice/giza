/* giza - regression test for page advance and lazy background behaviour
 *
 * Exercises two bugs fixed together in the page/background work:
 *
 * 1. giza_set_environment must advance the physical page when output
 *    already exists (classic PGENV calls PGPAGE before drawing axes).
 *    Without that, repeated PGENV calls overlay every plot on one PNG.
 *
 * 2. giza_histogram must not mark the page as begun before its internal
 *    environment setup runs, or PGHIST on a single /png page is written
 *    as prefix_0001.png instead of prefix.png.
 */

#include <giza.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int
file_exists (const char *path)
{
  return access (path, F_OK) == 0;
}

static void
remove_png_family (const char *prefix)
{
  char path[128];
  const char *suffixes[] = { ".png", "_0000.png", "_0001.png", NULL };
  int i;

  for (i = 0; suffixes[i] != NULL; i++)
    {
      snprintf (path, sizeof (path), "%s%s", prefix, suffixes[i]);
      unlink (path);
    }
}

/* Two PGENV-style calls must yield two hardcopy pages. */
static int
test_pgenv_advances_pages (void)
{
  const char *prefix = "test_pgenv_pages";
  char page0[128];
  char page1[128];
  char single[128];

  remove_png_family (prefix);

  if (giza_open_device ("/png", prefix) <= 0)
    {
      fprintf (stderr, "test_pgenv_advances_pages: could not open /png device\n");
      return 1;
    }

  giza_set_environment (0., 1., 0., 1., 0, 0);
  giza_set_colour_index (1);
  giza_ptext (0.5, 0.5, 0., 0.5, "page 0");

  giza_set_environment (0., 1., 0., 1., 0, 0);
  giza_ptext (0.5, 0.5, 0., 0.5, "page 1");

  giza_close_device ();

  snprintf (page0, sizeof (page0), "%s_0000.png", prefix);
  snprintf (page1, sizeof (page1), "%s_0001.png", prefix);

  if (file_exists (page0) && file_exists (page1))
    return 0;

  snprintf (single, sizeof (single), "%s.png", prefix);
  if (file_exists (single) && !file_exists (page0) && !file_exists (page1))
    {
      fprintf (stderr,
                "test_pgenv_advances_pages: only %s exists "
                "(two PGENV calls overlaid on one page)\n",
                single);
      return 1;
    }

  fprintf (stderr,
            "test_pgenv_advances_pages: expected %s and %s\n",
            page0, page1);
  return 1;
}

/* One PGHIST plot on a fresh device must use the unnumbered filename. */
static int
test_pghist_single_page_name (void)
{
  const char *prefix = "test_pghist_page";
  char single[128];
  char numbered0[128];
  char numbered1[128];
  double data[20];
  int i;

  for (i = 0; i < 20; i++)
    data[i] = 0.05 * (double) i;

  remove_png_family (prefix);

  if (giza_open_device ("/png", prefix) <= 0)
    {
      fprintf (stderr, "test_pghist_single_page_name: could not open /png device\n");
      return 1;
    }

  giza_histogram (20, data, 0., 1., 5, 0);
  giza_close_device ();

  snprintf (single, sizeof (single), "%s.png", prefix);
  snprintf (numbered0, sizeof (numbered0), "%s_0000.png", prefix);
  snprintf (numbered1, sizeof (numbered1), "%s_0001.png", prefix);

  if (!file_exists (single))
    {
      fprintf (stderr,
                "test_pghist_single_page_name: missing %s "
                "(single-page PGHIST should not be numbered)\n",
                single);
      return 1;
    }

  if (file_exists (numbered0) || file_exists (numbered1))
    {
      fprintf (stderr,
                "test_pghist_single_page_name: spurious numbered PNG "
                "(%s or %s exists)\n",
                numbered0, numbered1);
      return 1;
    }

  return 0;
}

int
main (void)
{
  if (test_pgenv_advances_pages () != 0)
    return 1;

  if (test_pghist_single_page_name () != 0)
    return 2;

  return 0;
}
