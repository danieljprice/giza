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

#define GIZA_DEVICE_IV     0
#define GIZA_DEVICE_NULL   1
#define GIZA_DEVICE_XW     2
#define GIZA_DEVICE_PNG    3
#define GIZA_DEVICE_PDF    4
#define GIZA_DEVICE_VPDF   5
#define GIZA_DEVICE_PS     6
#define GIZA_DEVICE_VPS    7
#define GIZA_DEVICE_EPS    8
#define GIZA_DEVICE_SVG    9

#define GIZA_DEFAULT_PREFIX "giza"

int _giza_get_key_press (int mode, int moveCurs, int nanc, const double *xanc, const double *yanc, double *x, double *y, char *ch);
void _giza_split_device_string (const char *deviceString, char **devType);
int _giza_device_to_int (const char *newDeviceName);
int _giza_int_to_device (int numDevice, char *DeviceName);
void _giza_init_norm (void);
void _giza_expand_clipping (void);
void _giza_restore_clipping (void);
void _giza_init_device_list (char **deviceList);
void _giza_free_device_list (char *deviceList);
void _giza_lowercase(const char *string, char *lowerstring);
void _giza_trim(char *str);
void _giza_get_filename_for_device (char *filename, char *prefix, int pgNum, char *extension, int lastpage);
void _giza_get_specified_size(double paperwidth, double paperheight, int paperunits, int *width, int *height);
