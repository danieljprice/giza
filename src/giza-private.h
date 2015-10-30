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

#include <cairo/cairo.h>

#define GIZA_TINY 0.1e-30
#define GIZA_ZERO_FLOAT 2.e-30
#define GIZA_ZERO_DOUBLE 3.e-300
#define GIZA_DEG_TO_RAD 0.0174532925
#define GIZA_MAX_DEVICES 128
#define GIZA_MAX_DEVSTRING 256

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))

/* Structures: */

/* giza window structure */
typedef struct
{
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  cairo_matrix_t userCoords;
  cairo_matrix_t normCoords;
} giza_window_t;

/* giza viewport structure */
typedef struct
{
  double xmin;
  double xmax;
  double ymin;
  double ymax;
} giza_viewport_t;

typedef struct
{
  int fs;
  double angle;
  double cutback;
} giza_arrow_t;

/* Store variables relating to the current device */
typedef struct
{
  int type;
  int width;
  int height;
  double deviceUnitsPermm;
  double deviceUnitsPerPixel;
  int isInteractive;
  char prefix[GIZA_MAX_DEVSTRING];
  int pgNum;
  int CurrentTrans;
  double defaultBackgroundAlpha;
  int deviceOpen;  /* Indicates if a device is open and ready to be drawn to. */
  int drawn;       /* Flag if anything has been drawn yet */
  int resize;      /* Flag that device has been resized */
  int prompting;   /* Turn prompting on/off for interactive device */
  int buf;         /* Buffering on/off */
  /* viewport and window settings for this device */
  giza_viewport_t VP;
  giza_window_t Win;
  /* sub-panel info */
  int nx;         /* number of sub-panels */
  int ny;
  int ix;     /* which panel we are in */
  int iy;
  int altpanelorder;
  double panelwidth;
  double panelheight;
  /* graphics settings for this device */
  int fs;
  double hatch_angle;
  double hatch_spacing;
  double hatch_phase;
  double lw;
  double ls;
  double ch;
  int text_background;
  int clip;
  giza_arrow_t Arrow;
  /* cairo objects and settings */
  cairo_t *context;
  cairo_surface_t *surface;
  cairo_font_extents_t fontExtents;
  double fontAngle;
} giza_device_t;

extern giza_device_t Dev[GIZA_MAX_DEVICES];
extern int id;

/* Struct containing global settings for giza */
typedef struct
{
  int autolog;
} giza_settings_t;

extern giza_settings_t Sets;

int _giza_check_device_ready (char *source);
void _giza_init_character_height (void);
void _giza_scale_character_size (double scalefac);
int _giza_nint (double x);
int _giza_equal (double x1, double x2);
void _giza_init_save (void);
void _giza_set_font (char *font, cairo_font_slant_t slant, cairo_font_weight_t weight);
double _giza_set_in_range (double val, double val1, double val2);
void _giza_get_markerheight (double *mheight);
void _giza_draw_symbol_device (double xd, double yd, int symbol);
void _giza_set_alpha (double alpha);
void _giza_get_alpha (double *alpha);
void _giza_get_log_file (char *string, int len);
void _giza_write_log_file (cairo_surface_t *surface);
void _giza_init(void);
