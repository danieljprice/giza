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

#include <cairo/cairo.h>

#define GIZA_TINY 0.1e-30
#define GIZA_ZERO_FLOAT 2.e-38
#define GIZA_ZERO_DOUBLE 3.e-308
#define GIZA_DEG_TO_RAD 0.0174532925

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))

// Structures:
// Store variables relating to the current view-port

// Store variables relating to the current device
struct GIZA_Device
{
  int type;
  int width;
  int height;
  double widthCM;
  double heightCM;
  double deviceUnitsPermm;
  int isInteractive;
  char *prefix;
  int pgNum;
  int CurrentTrans;
} Dev;

cairo_t *context;
cairo_surface_t *surface;


// Store the current settings of giza
struct GIZA_Settings
{
  cairo_font_extents_t fontExtents;
  double fontAngle;
  int buf;
} Sets;

int _giza_check_device_ready (char *source);
void _giza_set_deviceOpen ();
void _giza_reset_deviceOpen ();

int _giza_sizeSpecified ();
int _giza_has_drawn ();
void _giza_set_sizeSpecified ();
void _giza_reset_sizeSpecified ();
void _giza_set_drawn ();
void _giza_reset_drawn ();

int _giza_nint (double x);
void _giza_init_save();
