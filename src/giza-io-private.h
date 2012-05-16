/* giza - a scientific plotting layer built on cairo
 *
 * This file is (or was) part of GIZA, a scientific plotting
 * layer built on cairo.
 * GIZA comes with ABSOLUTELY NO WARRANTY.
 * This is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * Copyright (C) 2010 James Wetter & Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com, daniel.price@sci.monash.edu.au
 *
 */

void _giza_warning (const char *source, const char *message);
void _giza_message (const char *message);
void _giza_error (const char *source, const char *message);
int _giza_prompt_for_device (void);
void _giza_display_devices (void);
int _giza_default_device (void);
void _giza_newpage_prompt (void);
int _giza_get_prompting (void);


