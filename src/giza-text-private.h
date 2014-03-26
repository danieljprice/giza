/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the GPL for specific language governing rights and limitations.
 *
 * The Original code is the giza plotting library.
 *
 * Contributor(s):
 *      James Wetter <wetter.j@gmail.com>
 *      Daniel Price <daniel.price@monash.edu> (main contact)
 */

#define GIZA_SCALE_SUPERS 0.65
#define GIZA_RAISE_SUPERS 0.4
#define GIZA_SCALE_SUBS   0.65
#define GIZA_LOWER_SUBS   0.2
#define GIZA_FONT_LEN     24

void _giza_parse_string (const char *text, double *width, double *height, void(*action)(const char *, double *, double *));
void _giza_get_text_width (double *width, char *text);
void _giza_set_default_font (void);
void _giza_scale_character_size (double scalefac);

void _giza_action_get_size (const char *text, double *width, double *height);
void _giza_action_print (const char *text, double *width, double *height);

void _giza_start_super (void);
void _giza_stop_super (void);
void _giza_start_sub (void);
void _giza_stop_sub (void);
void _giza_switch_font (int fonttype);

