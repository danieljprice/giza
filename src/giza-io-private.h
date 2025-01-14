/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (GPL, see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 5a) of the GPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
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

void _giza_warning (const char *source, const char *message, ...);
void _giza_message (const char *message);
void _giza_error (const char *source, const char *message, ...);
int _giza_prompt_for_device (void);
void _giza_display_devices (void);
int _giza_default_device (void);
void _giza_newpage_prompt (void);
int _giza_get_prompting (void);


