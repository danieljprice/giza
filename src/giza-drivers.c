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

#include "giza-private.h"
#include "giza-drivers-private.h"
#include "giza-driver-xw-private.h"
#include "giza-driver-png-private.h"
#include "giza-driver-pdf-private.h"
#include "giza-driver-ps-private.h"
#include "giza-driver-eps-private.h"
#include "giza-driver-null-private.h"
#include "giza-io-private.h"
#include "giza-viewport-private.h"
#include "giza-window-private.h"
#include "giza-transforms-private.h"
#include "giza-character-size-private.h"
#include "giza-text-private.h"
#include "giza-set-font-private.h"
#include "giza-colour-index-private.h"
#include "giza-colour-table-private.h"
#include "giza-arrow-style-private.h"
#include "giza-line-style-private.h"
#include "giza-band-private.h"
#include <giza.h>
#include <string.h>
#include <stdlib.h>

#define GIZA_DEFAULT_MARGIN 0

static void _giza_init_device_list ();
static void _giza_free_device_list ();
static void _giza_set_prefix (char *prefix);
static void _giza_free_prefix ();

/* 
 * NOTE: Should clean up surfaces etc if device open was not successful.
 * Opens a device for drawing to.
 */
/**
 * Device: giza_open_device
 *
 * Synopsis: Opens a new device to be drawn to. Must be called before
 * any drawing can take place.
 *
 * Input:
 *  -newDeviceName :- Specifies the name and type of device to be opened.
 *                    See below for details.
 *  -newPrefix     :- Specifies the default prefix to be used for file names.
 *
 * Return value:
 *  -0 :- if no error has occurred
 *  -1 :- if an error occurred
 *
 * Available devices:
 *  -?    :- User selects device
 *  -/xw  :- X-window
 *  -/pdf :- Portable Document Format
 *  -/vpdf :- Landscape Portable Document Format
 *  -/ps  :- PostScript
 *  -/vps :- Landscape PostScript
 *  -/png :- Portable Network Graphics file
 *  -/eps :- EncapsulatedPostScript
 */
int
giza_open_device (char *newDeviceName, char *newPrefix)
{
  /* Some general initialisation */
  Dev.pgNum = 0;
  Dev.type = GIZA_DEVICE_IV;
  int success = -1;
  _giza_init_device_list ();
  giza_set_text_background (-1);

  if (newPrefix)
    {
      _giza_set_prefix (newPrefix);
    }
  else
    {
      _giza_set_prefix (GIZA_DEFAULT_PREFIX);
    }

  /* Determine which type of device to open */
  if (!strcmp (newDeviceName, "?"))
    Dev.type = _giza_prompt_for_device ();
  else
    {
      char *devTypeStr;
      _giza_split_device_string (newDeviceName, &devTypeStr);
      Dev.type = _giza_device_to_int (devTypeStr);
    }

  /* Determine which driver is required */
  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      success = _giza_open_device_xw ();
      break;
#endif
    case GIZA_DEVICE_PNG:
      success = _giza_open_device_png ();
      break;
    case GIZA_DEVICE_PDF:
      success = _giza_open_device_pdf (0);
      break;
    case GIZA_DEVICE_VPDF:
      success = _giza_open_device_pdf(1);
      break;
    case GIZA_DEVICE_PS:
      success = _giza_open_device_ps (0);
      break;
    case GIZA_DEVICE_VPS:
      success = _giza_open_device_ps (1);
      break;
    case GIZA_DEVICE_NULL:
      success = _giza_open_device_null ();
      break;
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      success = _giza_open_device_eps (0);
      break;
#endif
    case GIZA_DEVICE_IV:
    default:
      _giza_error ("giza_open_device", "Unknown device, device not opened");
      return -1;
    }

  /* DJP: set the size in cm: note that this may not be 
   *      identical to that specified in open_device_size
   *      because the actual device size must be a whole number
   *      of pixels
   */
  Dev.widthCM  = 0.1 * Dev.width / Dev.deviceUnitsPermm;
  Dev.heightCM = 0.1 * Dev.height / Dev.deviceUnitsPermm;

  /* check that the surface was created */
  if (success)
    return -1;

  /* bind the created surface to our context */
  context = cairo_create (surface);
  if (!context)
    {
      _giza_error ("giza_open_device", "Could not create cairo surface.");
      return -1;
    }

  /* some final initialisation */
  _giza_set_deviceOpen ();
  //  _giza_set_defaults ();
  _giza_init_arrow_style ();
  _giza_init_line_style ();
  _giza_init_colour_index ();
  _giza_draw_background ();
  _giza_init_colour_table ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  _giza_init_norm ();
  giza_set_viewport_default ();
  _giza_init_window ();
  giza_set_line_width (1);
  _giza_init_character_height ();
  _giza_init_font ();
  _giza_init_fill ();
  _giza_init_band_style ();
  _giza_init_save ();

  return 0;
}

/**
 * Device: giza_open_device_size
 *
 * Synopsis: Similar to giza_open_device, but allows one to specify the size of the device in centimeters
 *
 * Input:
 *  -newDeviceName :- Specifies the type of device to be opened.
 *                    See below for details.
 *  -newPrefix     :- Specifies the default prefix to be used for file names.
 *  -width         :- Width for the newly opened device in cms.
 *  -height        :- Height for the newly opened device in cms.
 */
int
giza_open_device_size (char *newDeviceName, char *newPrefix, double width, double height)
{
  Dev.widthCM = width;
  Dev.heightCM = height;
  _giza_set_sizeSpecified ();
  return giza_open_device (newDeviceName, newPrefix);
}

/**
 * Device: giza_open_device_size_float
 *
 * Synopsis: Same functionality as giza_open_device_size but takes floats
 */
int
giza_open_device_size_float (char *newDeviceName, char *newPrefix, float width, float height)
{
  return giza_open_device_size (newDeviceName, newPrefix, (double) width, (double) height);
}

/**
 * Device: giza_flush_device
 *
 * Synopsis: Flushes the currently open device.
 */
void
giza_flush_device ()
{
  if (!_giza_check_device_ready ("giza_flush_device"))
    return;

  _giza_set_drawn ();

  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_flush_device_xw ();
      break;
#endif
    case GIZA_DEVICE_PNG:
      _giza_flush_device_png ();
      break;
    case GIZA_DEVICE_PDF:
    case GIZA_DEVICE_VPDF:
      _giza_flush_device_pdf ();
      break;
    case GIZA_DEVICE_PS:
    case GIZA_DEVICE_VPS:
      _giza_flush_device_ps ();
      break;
    case GIZA_DEVICE_NULL:
      _giza_flush_device_null ();
      break;
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      _giza_flush_device_eps ();
      break;
#endif
    default:
      _giza_error ("giza_flush_device", "No device open, cannot flush");
      return;
    }
  return;
}

/**
 * Device: giza_change_page
 *
 * Synopsis: Advances the currently open device to the next page, and redraws
 * the background. If no other actions have been performed since the device was
 * opened or the last call to giza_change_page the call is ignored.
 */
void
giza_change_page ()
{
  if (!_giza_has_drawn ())
    return;
  if (!_giza_check_device_ready ("giza_change_page"))
    return;

  double oldCh, oldLw;
  giza_get_character_height (&oldCh);
  giza_get_line_width (&oldLw);

  int oldCi;
  giza_get_colour_index (&oldCi);

  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_change_page_xw ();
      break;
#endif
    case GIZA_DEVICE_PNG:
      _giza_change_page_png ();
      break;
    case GIZA_DEVICE_PDF:
    case GIZA_DEVICE_VPDF:
      _giza_change_page_pdf ();
      break;
    case GIZA_DEVICE_PS:
    case GIZA_DEVICE_VPS:
      _giza_change_page_ps ();
      break;
    case GIZA_DEVICE_NULL:
      _giza_open_device_null ();
      break;
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      _giza_change_page_eps ();
      break;
#endif
    default:
      _giza_error ("giza_change_page", "No device open");
      return;
    }
  Dev.pgNum++;
  giza_set_viewport (VP.xmin,VP.xmax, VP.ymin, VP.ymax);
  giza_set_window (Win.xmin, Win.xmax, Win.ymin, Win.ymax);
  giza_set_character_height (oldCh);
  giza_set_line_width (oldLw);
  giza_set_colour_index (oldCi);
  _giza_reset_drawn ();
  //_giza_draw_background ();
  return;
}

/**
 * Device: giza_close_device
 *
 * Synopsis: Closes the currently open device. Should always be called before exiting you program
 * as it frees associated memory.
 */
void
giza_close_device ()
{
  if (!_giza_check_device_ready ("giza_close_device"))
    return;

  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_close_device_xw ();
      break;
#endif
    case GIZA_DEVICE_PNG:
      _giza_close_device_png ();
      break;
    case GIZA_DEVICE_PDF:
    case GIZA_DEVICE_VPDF:
      _giza_close_device_pdf ();
      break;
    case GIZA_DEVICE_PS:
    case GIZA_DEVICE_VPS:
      _giza_close_device_ps ();
      break;
    case GIZA_DEVICE_NULL:
      _giza_close_device_null ();
      break;
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      _giza_close_device_eps ();
      break;
#endif
    default:
       return;
    }

  cairo_destroy (context);
  _giza_reset_deviceOpen ();
  _giza_reset_sizeSpecified ();

  // Destroy the font
  _giza_free_device_list ();
  _giza_free_font ();
  _giza_free_colour_table ();
  _giza_free_prefix ();
  Dev.type = GIZA_DEVICE_IV;
  return;
}

/**
 * Device: giza_query_device
 *
 * Synopsis: Queries various things about the current device.
 */
int
giza_query_device (char *querytype, char *returnval)
{
  int ierr;
  ierr = 0;

  /* Query device type */

  if (!strcmp(querytype,"type"))
     {
     if (!_giza_int_to_device(Dev.type,returnval)) { ierr = 1; }
     }

  /* Query whether or not device has cursor */

  else if (!strcmp(querytype,"cursor"))
     if (Dev.isInteractive)
     {
        strncpy(returnval,"YES",sizeof(returnval));
     }
     else
     {
        strncpy(returnval,"NO",sizeof(returnval));   
     }

  /* Query whether or not device is hard copy or not */

  else if (!strcmp(querytype,"hardcopy"))
     if (Dev.isInteractive)
     {
        strncpy(returnval,"NO",sizeof(returnval));
     }
     else
     {
        strncpy(returnval,"YES",sizeof(returnval));   
     }

/*  else if (!strcmp(querytype,"file")) */

  return ierr;
}

/**
 * Redraws the background of the currently open device
 */
void
_giza_draw_background ()
{
  if (!_giza_check_device_ready ("_giza_draw_background"))
    return;

  int oldCi;
  giza_get_colour_index (&oldCi);
  
  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_draw_background_xw ();
      break;
#endif
    case GIZA_DEVICE_PNG:
      _giza_draw_background_png ();
      break;
    case GIZA_DEVICE_PDF:
    case GIZA_DEVICE_VPDF:
      _giza_draw_background_pdf ();
      break;
    case GIZA_DEVICE_PS:
    case GIZA_DEVICE_VPS:
      _giza_draw_background_ps ();
      break;
    case GIZA_DEVICE_NULL:
      _giza_draw_background_null ();
      break;
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      _giza_draw_background_eps ();
      break;
#endif
    default:
      _giza_error ("giza_draw_background", "No device open, cannot draw background");
      break;
    }

  giza_set_colour_index (oldCi);
  
  return;
}

/**
 * Returns the cursor position and key press after a key press.
 *
 * Input:
 *  -mode :- Sets the type of band to be used.
 *  -moveCurs :- if 1 the cursor is moved to (x, y).
 *               if 0 the cursor is not moved.
 *  -xanc :- the x-coord of the anchor point (world coord).
 *  -yanc :- the y-coord of the anchor point (world coord).
 *  -x  :- Gets set to the x world coord of the cursor.
 *  -y  :- Gets set to the y world coord of the cursor.
 *  -ch :- Gets set to the character pressed.
 *
 * Return value:
 *  -1 :- If the device has no cursor
 *  -0 :- If the call was successful
 *
 * Modes:
 *  -0 :- No band is drawn.
 */
int
_giza_get_key_press (int mode, int moveCurs, double xanch, double yanch, double *x, double *y, char *ch)
{
  if (!_giza_check_device_ready ("_giza_get_key_press"))
    return 1;

  switch (Dev.type)
    {
    case GIZA_DEVICE_NULL:
    case GIZA_DEVICE_PDF:
    case GIZA_DEVICE_VPDF:
    case GIZA_DEVICE_PNG:
    case GIZA_DEVICE_PS:
    case GIZA_DEVICE_VPS:
      _giza_warning ("giza_get_key_press", "Current device does not support a cursor, returning x = 0, y = 0, ch = a");
      return 1;
      break;
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_get_key_press_xw (mode, moveCurs, xanch, yanch, x, y, ch);
      return 0;
      break;
#endif
    case GIZA_DEVICE_IV:
    default:
      _giza_error ("giza_get_key_press", "Unknown device");
      return 1;
    }
}

/*
 * Splits deviceString into name and type,
 * and overwrites the prefix
 */
void
_giza_split_device_string (char *deviceString, char **devType)
{
  if (deviceString == NULL)
    return;

  *devType = strrchr (deviceString, (int) '/');

  /* An invalid string */
  if (*devType == NULL)
    {
      *devType = deviceString;
      return;
    }

  /* Set the name */
  int nameLength = strlen (deviceString) - strlen (*devType);
  /* Ignore no name (use defualt) */
  if (nameLength != 0)
    {
      char devName[nameLength + 1];
      strncpy (devName, deviceString, (size_t) nameLength);
      devName[nameLength] = '\0';
      _giza_set_prefix (devName);
    }
}

/**
 * An internal function that converts a device string to it integer representation
 */
int
_giza_device_to_int (char *newDeviceName)
{
  int newDevice;

  if (!strcmp (newDeviceName, "/null"))
    newDevice = GIZA_DEVICE_NULL;
#ifdef _GIZA_HAS_XW
  else if (!strcmp (newDeviceName, "/xw"))
    newDevice = GIZA_DEVICE_XW;
#endif
  else if (!strcmp (newDeviceName, "/png"))
    newDevice = GIZA_DEVICE_PNG;
  else if (!strcmp (newDeviceName, "/pdf"))
    newDevice = GIZA_DEVICE_PDF;
  else if (!strcmp (newDeviceName, "/ps"))
    newDevice = GIZA_DEVICE_PS;
  else if (!strcmp (newDeviceName, "/vpdf"))
    newDevice = GIZA_DEVICE_VPDF;
  else if (!strcmp (newDeviceName, "/vps"))
    newDevice = GIZA_DEVICE_VPS;
  else if (!strcmp (newDeviceName, "/vps"))
    newDevice = GIZA_DEVICE_VPS;
#ifdef _GIZA_HAS_EPS
  else if (!strcmp (newDeviceName, "/eps"))
    newDevice = GIZA_DEVICE_EPS;
#endif
  else
    {
      newDevice = GIZA_DEVICE_IV;
    }

  return newDevice;
}

/**
 * An internal function that the integer representation back to the device string
 */
int
_giza_int_to_device (int numDevice, char *DeviceName)
{
 
 int ierr;
 ierr = 0;
 switch (numDevice)
    {
    case GIZA_DEVICE_NULL:
      strncpy(DeviceName,"/null",sizeof(DeviceName));
      break;
    case GIZA_DEVICE_PDF:
      strncpy(DeviceName,"/pdf",sizeof(DeviceName));
      break;
    case GIZA_DEVICE_VPDF:
      strncpy(DeviceName,"/vpdf",sizeof(DeviceName));
      break;
    case GIZA_DEVICE_PNG:
      strncpy(DeviceName,"/png",sizeof(DeviceName));
      break;
    case GIZA_DEVICE_PS:
      strncpy(DeviceName,"/ps",sizeof(DeviceName));
      break;
    case GIZA_DEVICE_VPS:
      strncpy(DeviceName,"/vps",sizeof(DeviceName));
      break;
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      strncpy(DeviceName,"/xw",sizeof(DeviceName));
      break;
#endif
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      strncpy(DeviceName,"/eps",sizeof(DeviceName));
      break;
#endif
    default:
    /* Do not print an error here as this is an internal routine:
       Instead, make sure the error is handled in the calling routine */
      strncpy(DeviceName," ",sizeof(DeviceName));
      ierr = 1;
    }
    return ierr;
}

/**
 * Prints a list of currently available devices to stdout
 */
void giza_print_device_list ()
{
   if (!deviceList) _giza_init_device_list ();
   _giza_display_devices();
}

/**
 * NOTE: Should use if defs to only add the devices that are available.
 * Allocates memory for the device list and sets it to a list of available devices.
 */
static void
_giza_init_device_list ()
{
  deviceList = malloc (326 * sizeof(char));
#ifdef _GIZA_HAS_XW
  strcat (deviceList, "Interactive devices:\n\t/xw\t(X Window)\n");
#endif
  strcat (deviceList, "Non-interactive file formats:\n\t/png\t(Portable network graphics file)\n\t/pdf\t(Portable document format file)\n\t/vpdf\t(Portable document format file portrait)\n\t/ps\t(PostScript file)\n\t/vps\t(PostScript file portrait)\n");
#ifdef _GIZA_HAS_EPS
  strcat (deviceList, "\t/eps\t(Encapsulated PostScript file)\n");
#endif
  strcat (deviceList, "\t/null\t(Null device)\n");
}

/**
 * Frees the memory alloacted for the deviceList
 */
static void
_giza_free_device_list ()
{
  if (deviceList)
    {
      //      free (deviceList);
    }
}

/**
 * Initialises the normalised device coords matrix.
 */
void
_giza_init_norm ()
{
  if (!_giza_check_device_ready ("_giza_init_norm"))
    return;

  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_init_norm_xw ();
      break;
#endif
    default:
    /*
     * The cairo transformations are:
     *
     * x' = xx*x + xy*y + x0
     * y' = yx*x + yy*y + y0
     *
     * where the call to matrix is (&mat,xx,yx,xy,yy,x0,y0)
     *
     * but note that y goes from 0 (top) to 1 (bottom) by default, so we reverse this.
     *
     * DJP: Here we can allow small margins in the transformation
     *      to normalised device coords to avoid clipping at the edge
     */
      cairo_matrix_init (&(Win.normCoords), Dev.width-2.*GIZA_DEFAULT_MARGIN, 0, 0, -Dev.height+2.*GIZA_DEFAULT_MARGIN, GIZA_DEFAULT_MARGIN,
			 Dev.height-GIZA_DEFAULT_MARGIN);
      _giza_set_trans (GIZA_TRANS_NORM);
      break;
    }
}


/**
 * Expands clipping so that the whole surface can be drawn to.
 */
void
_giza_expand_clipping ()
{
    if (!_giza_check_device_ready ("_giza_expand_clipping"))
    return;
  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_expand_clipping_xw ();
      break;
#endif
    default:
      _giza_set_trans (GIZA_TRANS_IDEN);
      cairo_reset_clip (context);
      cairo_rectangle (context, 0, 0, Dev.width, Dev.height);
      cairo_clip (context);
      break;
    }
}

/**
 * Allocates the array for Dev.prefix and sets it to prefix
 */
static void
_giza_set_prefix (char *prefix)
{
  int len = strlen (prefix);
  Dev.prefix = malloc ((len + 1) * sizeof (char));
  strcpy (Dev.prefix, prefix);
}

/**
 * Frees the memory associated with Dev.prefix
 */
static void
_giza_free_prefix ()
{
  free (Dev.prefix);
}

/*
 * Creates the surfaces for drawing the band
 */
int
_giza_init_band (int mode)
{
  if (mode == 0) return 0;

  int success = 1;
  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
      case GIZA_DEVICE_XW:
        success = _giza_init_band_xw (mode);
        break;
#endif
      default:
        _giza_error ("_giza_init_band", "band not implemented for this device");
	break;
    }
  _giza_set_line_style (Band.ls, Band.box);
  double lwDevice = Band.lw * Dev.deviceUnitsPermm * 0.25;
  cairo_set_line_width (Band.box, lwDevice);

  return success;
}

/*
 * Draws over the old band and draws the new one.
 */
void
_giza_refresh_band (int mode, int x1, int y1, int x2, int y2)
{
  if (mode == 0) return;

  // Draw over the old band
  cairo_paint (Band.restore);
 
/* 
  // set clipping
  int topleftx = x1 - 10;
  int toplefty = y1 - 10;
  int bottomrightx = x2 + 10;
  int bottomrighty = y2 + 10;
  if (x2 - x1 <= 0)
    {
      topleftx = x2 - 10;
      bottomrightx = x1 + 10;
    }
  if (y2 - y1 <= 0)
    {
      toplefty = y2 - 5;
      bottomrighty = y1 + 5;
    }
  cairo_rectangle (Band.box, x1, y1, x2 - x1, y2 - y1);
  cairo_rectangle (Band.restore, x1, y1, x2 - x1, y2 - y1);
  cairo_reset_clip (Band.box);
  cairo_reset_clip (Band.restore);
  cairo_clip (Band.box);
  cairo_clip (Band.restore);
 */
  switch (mode)
    {
      case 1: // Straight line
       // Draw the band
	cairo_move_to (Band.box, x1, y1);
	cairo_line_to (Band.box, x2, y2);
	cairo_stroke (Band.box);
	break;
      case 2: // empty rectangle
        // Draw the band
        cairo_rectangle (Band.box, x1, y1, x2 - x1, y2 - y1);
        cairo_stroke (Band.box);

	break;
      case 3: // Two horizontal lines
        cairo_move_to (Band.box, 0, y1);
        cairo_line_to (Band.box, Band.maxWidth, y1);
        cairo_move_to (Band.box, 0, y2);
        cairo_line_to (Band.box, Band.maxWidth, y2);
        cairo_stroke (Band.box);
	break;
      default:
        _giza_warning ("_giza_refresh_band", "invalid mode");
	break;
    }
}

/*
 * Cleans up the stuff associated with the band
 */
void
_giza_destroy_band (int mode)
{
  if (mode == 0) return;

  // Free memory
  cairo_destroy (Band.restore);
  cairo_destroy (Band.box);
  cairo_surface_destroy (Band.onscreen);

  // remove the last band
  giza_flush_device ();
}
