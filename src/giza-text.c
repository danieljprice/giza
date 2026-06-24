/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2014 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING.LESSER file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2 of the LGPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the LGPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-text-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#define GIZA_GLYPH_CACHE_SIZE 32

typedef enum
{
  GIZA_GLYPH_FALLBACK_TEXT,
  GIZA_GLYPH_FALLBACK_MARKER
} giza_glyph_fallback_kind;

typedef struct
{
  giza_font_t *font;
  uint32_t     unicode_value;
  int          has_glyph;
  int          valid;
} giza_glyph_cache_entry_t;

typedef struct
{
  uint32_t               unicode_value;
  giza_glyph_fallback_kind fallback_kind;
  const char            *fallback_text;
  int                    marker_number;
  const char            *fallback_text_alt;
} giza_glyph_fallback_entry_t;

static giza_glyph_fallback_entry_t const giza_glyph_fallback_table[] = {
  { 0x2032, GIZA_GLYPH_FALLBACK_TEXT,   "'",    0, NULL },
  { 0x2033, GIZA_GLYPH_FALLBACK_TEXT,   "\"",   0, NULL },
  { 0x00a9, GIZA_GLYPH_FALLBACK_TEXT,   "(c)",  0, NULL },
  /* degree: superscript o if available, else plain o */
  { 0x00b0, GIZA_GLYPH_FALLBACK_TEXT,   "\u1d52", 0, "o" },
  { 0x2018, GIZA_GLYPH_FALLBACK_TEXT,   "'",    0, NULL },
  { 0x2019, GIZA_GLYPH_FALLBACK_TEXT,   "'",    0, NULL },
  { 0x00d7, GIZA_GLYPH_FALLBACK_TEXT,   "x",    0, NULL },
  { 0x22c5, GIZA_GLYPH_FALLBACK_TEXT,   ".",    0, NULL },
  { 0x00b7, GIZA_GLYPH_FALLBACK_TEXT,   ".",    0, NULL },
  { 0x2212, GIZA_GLYPH_FALLBACK_TEXT,   "-",    0, NULL },
  { 0x00b1, GIZA_GLYPH_FALLBACK_TEXT,   "+/-",  0, NULL },
  { 0x2213, GIZA_GLYPH_FALLBACK_TEXT,   "-/+",  0, NULL },
  { 0x212b, GIZA_GLYPH_FALLBACK_TEXT,   "A",    0, NULL },
  { 0x2609, GIZA_GLYPH_FALLBACK_MARKER, NULL,   9, NULL },
  { 0x2190, GIZA_GLYPH_FALLBACK_MARKER, NULL,  28, NULL },
  { 0x2192, GIZA_GLYPH_FALLBACK_MARKER, NULL,  29, NULL },
  { 0x2191, GIZA_GLYPH_FALLBACK_MARKER, NULL,  30, NULL },
  { 0x2193, GIZA_GLYPH_FALLBACK_MARKER, NULL,  31, NULL },
};

static giza_glyph_cache_entry_t glyph_cache[GIZA_MAX_DEVICES][GIZA_GLYPH_CACHE_SIZE];

static giza_glyph_fallback_entry_t const *
_giza_lookup_glyph_fallback (uint32_t unicode_value)
{
  size_t i;

  for (i = 0; i < sizeof (giza_glyph_fallback_table) / sizeof (giza_glyph_fallback_table[0]); i++)
    if (giza_glyph_fallback_table[i].unicode_value == unicode_value)
      return &giza_glyph_fallback_table[i];
  return NULL;
}

static int
_giza_utf8_decode_next (const char **input_text, uint32_t *unicode_value)
{
  const unsigned char *bytes = (const unsigned char *) *input_text;

  if (bytes[0] == '\0')
    return 0;

  if (bytes[0] < 0x80)
    {
      *unicode_value = bytes[0];
      *input_text += 1;
      return 1;
    }

  if ((bytes[0] & 0xe0) == 0xc0 && (bytes[1] & 0xc0) == 0x80)
    {
      *unicode_value = ((uint32_t) (bytes[0] & 0x1f) << 6) | (bytes[1] & 0x3f);
      *input_text += 2;
      return 1;
    }

  if ((bytes[0] & 0xf0) == 0xe0
      && (bytes[1] & 0xc0) == 0x80
      && (bytes[2] & 0xc0) == 0x80)
    {
      *unicode_value = ((uint32_t) (bytes[0] & 0x0f) << 12)
                       | ((uint32_t) (bytes[1] & 0x3f) << 6)
                       | (bytes[2] & 0x3f);
      *input_text += 3;
      return 1;
    }

  if ((bytes[0] & 0xf8) == 0xf0
      && (bytes[1] & 0xc0) == 0x80
      && (bytes[2] & 0xc0) == 0x80
      && (bytes[3] & 0xc0) == 0x80)
    {
      *unicode_value = ((uint32_t) (bytes[0] & 0x07) << 18)
                       | ((uint32_t) (bytes[1] & 0x3f) << 12)
                       | ((uint32_t) (bytes[2] & 0x3f) << 6)
                       | (bytes[3] & 0x3f);
      *input_text += 4;
      return 1;
    }

  /* Invalid UTF-8: skip one byte */
  *unicode_value = bytes[0];
  *input_text += 1;
  return 1;
}

static int
_giza_text_needs_glyph_fallback (const char *input_text)
{
  const char *read_ptr = input_text;

  while (*read_ptr != '\0')
    {
      uint32_t unicode_value;
      giza_glyph_fallback_entry_t const *entry;

      if (!_giza_utf8_decode_next (&read_ptr, &unicode_value))
        break;

      entry = _giza_lookup_glyph_fallback (unicode_value);
      if (entry != NULL
          && entry->fallback_kind == GIZA_GLYPH_FALLBACK_TEXT
          && !_giza_font_has_glyph (unicode_value))
        return 1;
    }
  return 0;
}

static const char *
_giza_glyph_fallback_buffer_overflow (const char *input_text, char *output_text,
                                      size_t output_size)
{
  _giza_warning ("_giza_apply_glyph_fallback",
                 "Output buffer too small; text unchanged");
  (void) output_text;
  (void) output_size;
  return input_text;
}

static size_t
_giza_unicode_to_utf8 (uint32_t unicode_value, char *utf8_bytes, size_t utf8_size)
{
  if (unicode_value < 0x80)
    {
      if (utf8_size < 2)
        return 0;
      utf8_bytes[0] = (char) unicode_value;
      utf8_bytes[1] = '\0';
      return 1;
    }

  if (unicode_value < 0x800)
    {
      if (utf8_size < 3)
        return 0;
      utf8_bytes[0] = (char) (0xc0 | (unicode_value >> 6));
      utf8_bytes[1] = (char) (0x80 | (unicode_value & 0x3f));
      utf8_bytes[2] = '\0';
      return 2;
    }

  if (unicode_value < 0x10000)
    {
      if (utf8_size < 4)
        return 0;
      utf8_bytes[0] = (char) (0xe0 | (unicode_value >> 12));
      utf8_bytes[1] = (char) (0x80 | ((unicode_value >> 6) & 0x3f));
      utf8_bytes[2] = (char) (0x80 | (unicode_value & 0x3f));
      utf8_bytes[3] = '\0';
      return 3;
    }

  if (utf8_size < 5)
    return 0;
  utf8_bytes[0] = (char) (0xf0 | (unicode_value >> 18));
  utf8_bytes[1] = (char) (0x80 | ((unicode_value >> 12) & 0x3f));
  utf8_bytes[2] = (char) (0x80 | ((unicode_value >> 6) & 0x3f));
  utf8_bytes[3] = (char) (0x80 | (unicode_value & 0x3f));
  utf8_bytes[4] = '\0';
  return 4;
}

static int
_giza_glyph_cache_lookup (uint32_t unicode_value, int *has_glyph)
{
  size_t i;

  for (i = 0; i < GIZA_GLYPH_CACHE_SIZE; i++)
    {
      giza_glyph_cache_entry_t *entry = &glyph_cache[id][i];

      if (entry->valid
          && entry->font == Dev[id].Font
          && entry->unicode_value == unicode_value)
        {
          *has_glyph = entry->has_glyph;
          return 1;
        }
    }
  return 0;
}

static void
_giza_glyph_cache_store (uint32_t unicode_value, int has_glyph)
{
  static size_t next_slot[GIZA_MAX_DEVICES];
  giza_glyph_cache_entry_t *entry;

  entry = &glyph_cache[id][next_slot[id] % GIZA_GLYPH_CACHE_SIZE];
  entry->font          = Dev[id].Font;
  entry->unicode_value = unicode_value;
  entry->has_glyph     = has_glyph;
  entry->valid         = 1;
  next_slot[id]++;
}

void
_giza_glyph_cache_invalidate (void)
{
  size_t i;

  for (i = 0; i < GIZA_GLYPH_CACHE_SIZE; i++)
    glyph_cache[id][i].valid = 0;
}

int
_giza_font_has_glyph (uint32_t unicode_value)
{
  char utf8_bytes[8];
  size_t utf8_length;
  cairo_scaled_font_t *scaled_font;
  cairo_glyph_t *glyphs = NULL;
  int glyph_count = 0;
  cairo_status_t status;
  int has_glyph;

  if (unicode_value < 128)
    return 1;

  if (!_giza_check_device_ready ("_giza_font_has_glyph"))
    return 1;

  if (_giza_glyph_cache_lookup (unicode_value, &has_glyph))
    return has_glyph;

  utf8_length = _giza_unicode_to_utf8 (unicode_value, utf8_bytes, sizeof utf8_bytes);
  if (utf8_length == 0)
    return 1;

  scaled_font = cairo_get_scaled_font (Dev[id].context);
  status = cairo_scaled_font_text_to_glyphs (scaled_font,
                                             0., 0.,
                                             utf8_bytes, (int) utf8_length,
                                             &glyphs, &glyph_count,
                                             NULL, NULL,
                                             NULL);
  if (status != CAIRO_STATUS_SUCCESS || glyph_count <= 0)
    has_glyph = 0;
  else
    has_glyph = (glyphs[0].index != 0);

  if (glyphs != NULL)
    cairo_glyph_free (glyphs);

  _giza_glyph_cache_store (unicode_value, has_glyph);
  return has_glyph;
}

static const char *
_giza_resolve_text_fallback (giza_glyph_fallback_entry_t const *entry)
{
  uint32_t replacement_unicode;
  const char *rest;

  if (entry->fallback_text_alt == NULL)
    return entry->fallback_text;

  rest = entry->fallback_text;
  if (_giza_utf8_decode_next (&rest, &replacement_unicode)
      && _giza_font_has_glyph (replacement_unicode))
    return entry->fallback_text;

  return entry->fallback_text_alt;
}

int
_giza_try_marker_fallback (const char *input_text, int *marker_number)
{
  uint32_t unicode_value;
  const char *rest;
  giza_glyph_fallback_entry_t const *entry;

  if (input_text == NULL || marker_number == NULL)
    return 0;

  rest = input_text;
  if (!_giza_utf8_decode_next (&rest, &unicode_value))
    return 0;
  if (*rest != '\0')
    return 0;

  entry = _giza_lookup_glyph_fallback (unicode_value);
  if (entry == NULL || entry->fallback_kind != GIZA_GLYPH_FALLBACK_MARKER)
    return 0;
  if (_giza_font_has_glyph (unicode_value))
    return 0;

  *marker_number = entry->marker_number;
  return 1;
}

const char *
_giza_apply_glyph_fallback (const char *input_text, char *output_text, size_t output_size)
{
  const char *read_ptr;
  char *write_ptr;
  size_t remaining;

  if (input_text == NULL || output_text == NULL || output_size == 0)
    return input_text;

  if (input_text[0] == '\0')
    {
      output_text[0] = '\0';
      return output_text;
    }

  if (!_giza_text_needs_glyph_fallback (input_text))
    return input_text;

  read_ptr   = input_text;
  write_ptr  = output_text;
  remaining  = output_size;

  while (*read_ptr != '\0')
    {
      uint32_t unicode_value;
      const char *char_start = read_ptr;
      giza_glyph_fallback_entry_t const *entry;

      if (!_giza_utf8_decode_next (&read_ptr, &unicode_value))
        break;

      entry = _giza_lookup_glyph_fallback (unicode_value);
      if (entry != NULL
          && entry->fallback_kind == GIZA_GLYPH_FALLBACK_TEXT
          && !_giza_font_has_glyph (unicode_value))
        {
          const char *replacement_text;
          size_t fallback_length;

          replacement_text = _giza_resolve_text_fallback (entry);
          fallback_length = strlen (replacement_text);

          if (fallback_length + 1 > remaining)
            return _giza_glyph_fallback_buffer_overflow (input_text, output_text,
                                                           output_size);
          memcpy (write_ptr, replacement_text, fallback_length);
          write_ptr += fallback_length;
          remaining -= fallback_length;
        }
      else
        {
          size_t char_length = (size_t) (read_ptr - char_start);

          if (char_length + 1 > remaining)
            return _giza_glyph_fallback_buffer_overflow (input_text, output_text,
                                                         output_size);
          memcpy (write_ptr, char_start, char_length);
          write_ptr += char_length;
          remaining -= char_length;
        }
    }

  if (remaining == 0)
    return _giza_glyph_fallback_buffer_overflow (input_text, output_text,
                                                 output_size);

  *write_ptr = '\0';
  return output_text;
}

void
_giza_draw_marker_at_pen (double pen_x, double pen_y,
                          double cos_angle, double sin_angle,
                          int marker_number,
                          double *width)
{
  double marker_height;

  _giza_get_markerheight (&marker_height);

  if (*width < 0.)
    {
      double dx = marker_height;
      double dy = -0.5 * marker_height;

      _giza_draw_symbol_device (pen_x + dx * cos_angle + dy * sin_angle,
                                pen_y + dx * sin_angle + dy * cos_angle,
                                marker_number);
      cairo_move_to (Dev[id].context,
                     pen_x + 2. * dx * cos_angle,
                     pen_y + 2. * dx * sin_angle);
    }
  else
    *width = *width + 2. * marker_height;
}

static int
_giza_action_marker_fallback (const char *text, double *width, double *height)
{
  int marker_number;
  double pen_x, pen_y;
  double cos_angle, sin_angle;
  int measuring;

  if (!_giza_try_marker_fallback (text, &marker_number))
    return 0;

  measuring = (*width >= 0.);
  cairo_get_current_point (Dev[id].context, &pen_x, &pen_y);
  cos_angle = cos (Dev[id].fontAngle);
  sin_angle = sin (Dev[id].fontAngle);
  _giza_draw_marker_at_pen (pen_x, pen_y, cos_angle, sin_angle,
                            marker_number, width);
  if (measuring)
    {
      double marker_height;

      _giza_get_markerheight (&marker_height);
      if (marker_height > *height)
        *height = marker_height;
    }
  return 1;
}

static void
_giza_check_cairo_text_status (const char *routine, const char *text)
{
  cairo_status_t status = cairo_status (Dev[id].context);

  if (status == CAIRO_STATUS_SUCCESS)
    return;

  _giza_warning (routine, "cairo text error for '%s': %s",
                 text, cairo_status_to_string (status));
}

void _giza_action_get_size (const char *text, double *width, double *height)
{
  char fallback_buffer[256];
  const char *text_to_measure;

  if (strlen (text) <= 0)
    return;

  if (_giza_action_marker_fallback (text, width, height))
    return;

  text_to_measure = _giza_apply_glyph_fallback (text, fallback_buffer,
                                                sizeof fallback_buffer);

  {
    double newHeight, dummy;
    cairo_get_current_point (Dev[id].context, &dummy, &newHeight);
    cairo_text_extents_t extents;
    cairo_text_extents (Dev[id].context, text_to_measure, &extents);
    _giza_check_cairo_text_status ("giza_text_extents", text_to_measure);

    dummy = newHeight;
    *width += extents.x_advance;
    newHeight = -newHeight - extents.y_bearing;
    if (newHeight > *height)
      *height = newHeight;
  }
}

void _giza_action_print (const char *text, double *width, double *height)
{
  char fallback_buffer[256];
  const char *text_to_draw;

  if (_giza_action_marker_fallback (text, width, height))
    {
      *width  = -1.;
      *height = -1.;
      return;
    }

  text_to_draw = _giza_apply_glyph_fallback (text, fallback_buffer,
                                             sizeof fallback_buffer);

  if (strlen (text_to_draw) > 0)
    {
      cairo_show_text (Dev[id].context, text_to_draw);
      _giza_check_cairo_text_status ("giza_show_text", text_to_draw);
    }
  *width  = -1.;
  *height = -1.;
}

/**
 * Moves the current position to continue writing in superscript.
 */
void
_giza_start_super (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  double x , y, height = Dev[id].fontExtents.ascent * GIZA_RAISE_SUPERS;

  x = height * sin (Dev[id].fontAngle);
  y = -height * cos (Dev[id].fontAngle);

  cairo_rel_move_to (Dev[id].context, x, y);

  /* DJP: we scale the character height relative to it's current size
   *    rather than setting the absolute height -- this preserves
   *    the font matrix and hence any rotation etc. that has been set
   */
  _giza_scale_character_size (GIZA_SCALE_SUPERS);

  _giza_set_trans (oldTrans);
}

/**
 * If currently writing superscript returns to normal position.
 */
void
_giza_stop_super (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  _giza_scale_character_size (1. / GIZA_SCALE_SUPERS);

  double x , y, height = Dev[id].fontExtents.ascent * GIZA_RAISE_SUPERS;

  x = -height * sin (Dev[id].fontAngle);
  y = height * cos (Dev[id].fontAngle);

  cairo_rel_move_to (Dev[id].context, x, y);
  _giza_set_trans (oldTrans);
}

/**
 * Moves the current position to continue writing in subscript.
 *
 * Must be in GIZA_TRANS_IDEN.
 */
void
_giza_start_sub (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  double x , y, height = -Dev[id].fontExtents.ascent * GIZA_LOWER_SUBS;

  x = height * sin (Dev[id].fontAngle);
  y = -height * cos (Dev[id].fontAngle);

  cairo_rel_move_to (Dev[id].context, x, y);

  _giza_scale_character_size (GIZA_SCALE_SUBS);
  _giza_set_trans (oldTrans);
}

/**
 * If currently writing subscript returns to normal position.
 *
 * Must be in GIZA_TRANS_IDEN.
 */
void
_giza_stop_sub (void)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  _giza_scale_character_size (1. / GIZA_SCALE_SUBS);

  double x , y, height = -Dev[id].fontExtents.ascent * GIZA_LOWER_SUBS;

  x = -height * sin (Dev[id].fontAngle);
  y = height * cos (Dev[id].fontAngle);

  cairo_rel_move_to (Dev[id].context, x, y);
  _giza_set_trans (oldTrans);
}

/**
 * Switches font family during text write
 *
 */
void
_giza_switch_font (int fonttype)
{
  int len = GIZA_FONT_LEN;
  char giza_font[len];
  switch(fonttype)
    {
    case 5:
      giza_get_font(giza_font,len);
      giza_set_font_bold(giza_font);
      break;
    case 4:
      giza_set_font("Cursive");
      break;
    case 3:
      giza_get_font(giza_font,len);
      giza_set_font_italic(giza_font);
      break;
    case 2:
      giza_set_font("Times");
      break;
    case 1:
      giza_set_font("Arial");
      break;
    }
}
