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
 * Copyright (C) 2010 James Wetter. All rights reserved.
 * Contact: wetter.j@gmail.com
 *
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

#define GIZA_DEFAULT_PREFIX "giza"

char *deviceList;

int _giza_get_key_press (int mode, int moveCurs, double xanc, double yanc, double *x, double *y, char *ch);
void _giza_split_device_string (char *deviceString, char **devType);
int _giza_device_to_int (char *newDeviceName);
int _giza_int_to_device (int numDevice, char *DeviceName);
void _giza_draw_background ();
void _giza_init_norm ();
void _giza_expand_clipping ();
void _giza_restore_clipping ();
