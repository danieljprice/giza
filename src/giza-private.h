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
 * Copyright (C) 2010-2011 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include <cairo/cairo.h>

#define GIZA_TINY 0.1e-30
#define GIZA_ZERO_FLOAT 2.e-30
#define GIZA_ZERO_DOUBLE 3.e-300
#define GIZA_DEG_TO_RAD 0.0174532925

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))

/* Structures: */
/* Store variables relating to the current view-port */

/* Store variables relating to the current device */
struct GIZA_Device
{
  int type;
  int width;
  int height;
  double widthCM;
  double heightCM;
  double deviceUnitsPermm;
  double deviceUnitsPerPixel;
  int isInteractive;
  char prefix[30];
  int pgNum;
  int CurrentTrans;
  double defaultBackgroundAlpha;
} Dev;

cairo_t *context;
cairo_surface_t *surface;


/* Store the current settings of giza */
struct GIZA_Settings
{
  cairo_font_extents_t fontExtents;
  double fontAngle;
  int buf;
} Sets;

int _giza_check_device_ready (char *source);
void _giza_set_deviceOpen (void);
void _giza_reset_deviceOpen (void);

int _giza_sizeSpecified (void);
int _giza_has_drawn (void);
void _giza_set_sizeSpecified (void);
void _giza_reset_sizeSpecified (void);
void _giza_set_drawn (void);
void _giza_reset_drawn (void);
void _giza_init_character_height (void);
void _giza_scale_character_size (double scalefac);
int _giza_nint (double x);
int _giza_equal (double x1, double x2);
void _giza_init_save (void);
void _giza_set_font (char *font, cairo_font_slant_t slant, cairo_font_weight_t weight);
double _giza_set_in_range (double val, double val1, double val2);
