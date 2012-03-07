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
 * Copyright (C) 2010-2012 James Wetter and Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include "giza-private.h"
#include "giza-drivers-private.h"
#include "giza-driver-xw-private.h"
#include "giza-driver-png-private.h"
#include "giza-driver-pdf-private.h"
#include "giza-driver-ps-private.h"
#include "giza-driver-eps-private.h"
#include "giza-driver-svg-private.h"
#include "giza-driver-null-private.h"
#include "giza-io-private.h"
#include "giza-viewport-private.h"
#include "giza-window-private.h"
#include "giza-transforms-private.h"
#include "giza-character-size-private.h"
#include "giza-text-private.h"
#include "giza-set-font-private.h"
#include "giza-colour-private.h"
#include "giza-arrow-style-private.h"
#include "giza-fill-private.h"
#include "giza-line-style-private.h"
#include "giza-band-private.h"
#include <giza.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#define GIZA_DEFAULT_MARGIN 0

static void _giza_set_prefix (const char *prefix);
static int id = 0;

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
 *  -/eps :- Encapsulated Postscript
 *  -/png :- Portable Network Graphics file
 *  -/svg :- Scalable Vector Graphics file
 *  -/pdf :- Portable Document Format
 *  -/vpdf :- Landscape Portable Document Format
 *  -/ps  :- PostScript
 *  -/vps :- Landscape Postscript
 */
int
giza_open_device (const char *newDeviceName, const char *newPrefix)
{

  if (id != 0) {
     _giza_warning("giza_open_device", "previous device was not closed");
     giza_close_device();
  }
  id += 1;

  /* Some general initialisation */
  Dev.pgNum = 0;
  Dev.type = GIZA_DEVICE_IV;
  Dev.defaultBackgroundAlpha = 1.;
  int success = -1;
  giza_set_text_background (-1);
  giza_start_prompting ();

  if (newPrefix)
    {
      _giza_set_prefix (newPrefix);
    }
  else
    {
      _giza_set_prefix (GIZA_DEFAULT_PREFIX);
    }

  /* Determine which type of device to open */
  char firstchar = newDeviceName[0];
  if (firstchar == '?')
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
    case GIZA_DEVICE_SVG:
      success = _giza_open_device_svg (0);
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
  /*  _giza_set_defaults (); */
  _giza_init_arrow_style ();
  _giza_init_line_style ();
  _giza_init_colour_index ();
  /*printf("debug: init font \n");*/

  giza_draw_background ();
  _giza_init_colour_table ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  _giza_init_norm ();
  
  /* font and character height initialisations
     need to come before set_viewport_default */
  _giza_init_character_height (); /* must come BEFORE init_font */
  _giza_init_font ();

  _giza_init_window (); /* call init_window BEFORE set_viewport */
  giza_set_viewport_default ();
  giza_set_line_width (1);

  /*
  printf("debug: init fill \n");
  */
  _giza_init_fill ();
  _giza_init_band_style ();
  _giza_init_save ();
  giza_set_clipping(1);

  /*
  printf("debug: device opened \n");
  */
  return Dev.pgNum;
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
 *  -width         :- Width for the newly opened device
 *  -height        :- Height for the newly opened device
 *  -units         :- Units in which the width/height of the device are specified
 *
 * Units:
 *  -GIZA_UNITS_DEVICE     :- device coords (i.e. pixels on bitmap devices, points on vector devices)
 *  -GIZA_UNITS_PIXELS     :- pixels
 *  -GIZA_UNITS_MM         :- mm
 *  -GIZA_UNITS_INCHES     :- inches
 *  Other values cause an error message and are treated as GIZA_UNITS_DEVICE
 */
int
giza_open_device_size (const char *newDeviceName, const char *newPrefix, double width, double height, int units)
{
  giza_set_paper_size(units,width,height);
  _giza_set_sizeSpecified ();
  return giza_open_device (newDeviceName, newPrefix);
}

/**
 * Device: giza_open_device_size_float
 *
 * Synopsis: Same functionality as giza_open_device_size but takes floats
 */
int
giza_open_device_size_float (const char *newDeviceName, const char *newPrefix, float width, float height, int units)
{
  return giza_open_device_size (newDeviceName, newPrefix, (double) width, (double) height, units);
}

/**
 * Device: giza_flush_device
 *
 * Synopsis: Flushes the currently open device.
 */
void
giza_flush_device (void)
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
    default:
      if (!surface)
        {
          _giza_error ("giza_flush_device", "No device open, cannot flush");
          return;
        } else {
          cairo_surface_flush(surface);
        }
      return;
    }
  return;
}

/**
 * Device: giza_resize_device
 *
 * Synopsis: Flushes the currently open device.
 */
void
_giza_resize_device (int width, int height)
{
  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_change_size_xw (width + 40, height + 40);
      break;
#endif
    }
}

/**
 * Device: giza_change_page
 *
 * Synopsis: Advances the currently open device to the next page, and redraws
 * the background. If no other actions have been performed since the device was
 * opened or the last call to giza_change_page the call is ignored.
 */
void
giza_change_page (void)
{
  if (!_giza_has_drawn ()) {
    /* allow resizing of the device if nothing has been drawn
    int width, height;
    _giza_get_specified_size(&width, &height);
    _giza_resize_device(width, height);
    _giza_change_page_xw();
    */
    /* if nothing has changed, safe to redraw/reset the background colour */
    giza_draw_background ();
    return;
  }

  if (!_giza_check_device_ready ("giza_change_page"))
    return;

  /* save a whole bunch of settings
    (line style, width, colour index etc.) */
  giza_save();

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
    case GIZA_DEVICE_SVG:
      _giza_change_page_svg ();
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
      _giza_close_device_null ();
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

  if (_giza_get_prompting () && Dev.isInteractive)
    {
      _giza_newpage_prompt();
    }

  Dev.pgNum++;

  /* Reset stuff */
  giza_set_viewport (VP.xmin,VP.xmax, VP.ymin, VP.ymax);
  giza_set_window (Win.xmin, Win.xmax, Win.ymin, Win.ymax);

  /* restore the previously saved settings */
  giza_restore();

  _giza_reset_drawn ();
  giza_draw_background ();
  giza_flush_device ();
  return;
}

/**
 * Device: giza_close_device
 *
 * Synopsis: Closes the currently open device. Should always be called before exiting your program
 * as it frees associated memory.
 */
void
giza_close_device (void)
{
  if (!_giza_check_device_ready ("giza_close_device"))
    return;

  if (_giza_get_prompting () && Dev.isInteractive)
    {
      _giza_newpage_prompt();
    }

  /* destroy the cairo context */
  if (context) cairo_destroy(context);

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
    case GIZA_DEVICE_SVG:
      _giza_close_device_svg ();
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

  _giza_reset_deviceOpen ();
  _giza_reset_sizeSpecified ();

  /* Destroy the font */
  _giza_free_font ();
  _giza_free_colour_table ();
  Dev.type = GIZA_DEVICE_IV;
  id = id - 1;

  return;
}

/**
 * Device: giza_query_device
 *
 * Synopsis: Queries various things about the current device.
 *
 * Input:
 *  -querytype :- a string containing the query type
 *
 * Output:
 *  -returnval :- string with result of query
 *
 * The following query types are possible:
 *  "state"    :- returns whether or not a device is open (returns "OPEN" or "CLOSED")
 *  "device"   :- returns device name for current device (e.g. giza.png)
 *  "type"     :- returns device type of the current device (e.g. /png,/xw)
 *  "dev/type" :- returns device name and type for current device (e.g. giza.png/png)
 *  "file"     :- returns filename if device is non-interactive
 *  "user"     :- returns current username
 *  "cursor"   :- returns whether or not device has a cursor (returns "YES" or "NO")
 *  "hardcopy" :- returns whether device is a hardcopy device (returns "YES" or "NO")
 *
 */
int
giza_query_device (const char *querytype, char *returnval)
{
  int ierr;
  ierr = 0;
  char devType[12];

  /* Query device type */

  if (!strcmp(querytype,"type"))
     {
       if (!_giza_int_to_device(Dev.type,returnval))
         {
           ierr = 1;
         }
     }
  /* Query whether or not device has cursor */
  else if (!strcmp(querytype,"cursor"))
    {
      if (Dev.isInteractive)
        {
          strncpy(returnval,"YES",sizeof(returnval));
        }
      else
        {
          strncpy(returnval,"NO",sizeof(returnval));
        }
    }
  /* Query whether or not device is hard copy or not */
  else if (!strcmp(querytype,"hardcopy"))
    {
      if (Dev.isInteractive)
        {
          strncpy(returnval,"NO",sizeof(returnval));
        }
      else
        {
          strncpy(returnval,"YES",sizeof(returnval));
        }
    }
  /* Query whether or not device is open or not */
  else if (!strcmp(querytype,"state"))
    {
       if (_giza_get_deviceOpen())
         {
            strncpy(returnval,"OPEN",sizeof(returnval));         
         }
       else
         {
            strncpy(returnval,"CLOSED",sizeof(returnval));
         }    
    }
  /* Query current user */
  else if (!strcmp(querytype,"user"))
    {
       strncpy(returnval,getlogin(),sizeof(returnval));
    }
  /* Query current device name */
  else if (!strcmp(querytype,"device"))
    {
       strncpy(returnval,Dev.prefix,sizeof(returnval));
    }
  /* Query current device/type */
  else if (!strcmp(querytype,"dev/type"))
    {
       strncpy(returnval,Dev.prefix,sizeof(returnval));
       if (!_giza_int_to_device(Dev.type,devType))
         {
           ierr = 1;
           strncat(returnval,devType,4*sizeof(char));
         }
    }
  /* Query current filename (as entered by user) */
  else if (!strcmp(querytype,"file"))
    {
       if (!Dev.isInteractive)
         {
           strncpy(returnval,Dev.prefix,sizeof(returnval));
         }
    }

  return ierr;
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
    case GIZA_DEVICE_SVG:
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
_giza_split_device_string (const char *deviceString, char **devType)
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
  int lendev  = strlen(deviceString);  /* long-winded to avoid */
  int lentype = strlen(*devType);      /* icc compile warnings */
  int nameLength = lendev - lentype;
  /* Ignore no name (use default) */
  if (nameLength != 0)
    {
      char devName[nameLength + 1];
      strncpy (devName, deviceString, (size_t) nameLength);
      devName[nameLength] = '\0';
      _giza_set_prefix (devName);
    }
}

/**
 * An internal function that converts a device string to its integer representation
 */
int
_giza_device_to_int (const char *newDeviceName)
{
  int newDevice;

  /* copy the string and make it lower case */
  char devName[strlen (newDeviceName) + 1];

  int i;
  for (i = 0; i < (signed ) strlen (newDeviceName); ++i)
    {
      devName[i] = (char) tolower (newDeviceName[i]);
    }
  devName[i] = '\0';

  if (!strcmp (devName, "/null"))
    newDevice = GIZA_DEVICE_NULL;
#ifdef _GIZA_HAS_XW
  else if (!strcmp (devName, "/xw")
        || !strcmp (devName, "/xwin"))
    newDevice = GIZA_DEVICE_XW;
#endif
  else if (!strcmp (devName, "/png"))
    newDevice = GIZA_DEVICE_PNG;
  else if (!strcmp (devName, "/svg"))
    newDevice = GIZA_DEVICE_SVG;
  else if (!strcmp (devName, "/pdf"))
    newDevice = GIZA_DEVICE_PDF;
  else if (!strcmp (devName, "/ps")
        || !strcmp (devName, "/cps")
        || !strcmp (devName, "/postscript"))
    newDevice = GIZA_DEVICE_PS;
  else if (!strcmp (devName, "/vpdf"))
    newDevice = GIZA_DEVICE_VPDF;
  else if (!strcmp (devName, "/vps"))
    newDevice = GIZA_DEVICE_VPS;
#ifdef _GIZA_HAS_EPS
  else if (!strcmp (devName, "/eps"))
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
    case GIZA_DEVICE_SVG:
      strncpy(DeviceName,"/svg",sizeof(DeviceName));
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
void
giza_print_device_list (void)
{
   _giza_display_devices();
}

/**
 * NOTE: Should use if defs to only add the devices that are available.
 * Allocates memory for the device list and sets it to a list of available devices.
 */
void
_giza_init_device_list (char **deviceList)
{
  *deviceList = malloc (500 * sizeof(char)); /* be CAREFUL here to make sure the string is big enough */

  *deviceList[0] = '\0';
#ifdef _GIZA_HAS_XW
  strcat (*deviceList, "Interactive devices:\n\t/xw\t(X Window)\n");
#endif
  strcat (*deviceList, "Non-interactive file formats:\n");
  strcat (*deviceList, "\t/png\t(Portable network graphics file)\n");
  strcat (*deviceList, "\t/svg\t(Scalable vector graphics file)\n");
#ifdef _GIZA_HAS_EPS
  strcat (*deviceList, "\t/eps\t(Encapsulated Postscript, one file per page)\n");
#endif
  strcat (*deviceList, "\t/pdf\t(Portable document format file)\n");
  strcat (*deviceList, "\t/vpdf\t(Portable document format file portrait)\n");
  strcat (*deviceList, "\t/ps\t(Postscript file, multiple pages per file)\n");
  strcat (*deviceList, "\t/vps\t(Postscript file portrait, multiple pages per file)\n");
  strcat (*deviceList, "\t/null\t(Null device)\n");
}

/**
 * Frees the memory alloacted for the deviceList
 */
void
_giza_free_device_list (char *deviceList)
{
  if (deviceList)
    {
      free (deviceList);
    }
}

/**
 * Initialises the normalised device coords matrix.
 */
void
_giza_init_norm (void)
{
  if (!_giza_check_device_ready ("_giza_init_norm"))
    return;

  double xx,yy,x0,y0;

  switch (Dev.type)
    {
#ifdef _GIZA_HAS_XW
    /*
     *  X-Windows device is different because it has additional margins around the giza window
     *  (this should in principle be for *all* interactive devices...)
     */
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

      xx = (double) Dev.width - 2.*GIZA_DEFAULT_MARGIN;
      yy = 2.*GIZA_DEFAULT_MARGIN - (double) Dev.height;
      x0 = GIZA_DEFAULT_MARGIN;
      y0 = (double) Dev.height - GIZA_DEFAULT_MARGIN;
      cairo_matrix_init (&(Win.normCoords), xx, 0, 0, yy, x0, y0);
      _giza_set_trans (GIZA_TRANS_NORM);
      break;
    }
}


/**
 * Expands clipping so that the whole surface can be drawn to.
 */
void
_giza_expand_clipping (void)
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
_giza_set_prefix (const char *prefix)
{
  strncpy (Dev.prefix, prefix, sizeof(Dev.prefix));
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
        success = _giza_init_band_xw ();
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

/**
 * Internal utility to convert a string to lowercase
 */
void _giza_lowercase(const char *string, char *lowerstring)
{
   int  i = 0;

   while ( string[i] )
   {
      lowerstring[i] = (char) tolower(string[i]);
      i++;
   }
   lowerstring[i] = '\0';
   return;
}


/**
 * Internal utility to trim strings
 * NB: lots of examples on the web return char*
 *     but this is pointless as they all modify the
 *     input string
 */
void _giza_trim(char *str) {
   int len = strlen(str);
   int i;
   for (i=0; i<len && isspace(str[i]); i++, str++);
   for (i=len-1; i>=0 && isspace(str[i]); str[i]=0, i--);
   return;
}


/**
 * Internal utility to construct device filenames
 * given the prefix, the device extension and page number
 */
void _giza_get_filename_for_device (char *filename, char *prefix, int pgNum, char *extension)
{

  /* this stuff is instead of using strcasestr
   * (giza should not use non-standard extensions)
   */
  char lprefix[strlen(prefix)];
  char lextens[strlen(extension)];
  _giza_lowercase(prefix,lprefix);
  _giza_lowercase(extension,lextens);

  char *prefixtrim = prefix;
  char *ext = extension;
  _giza_trim(prefixtrim);
  _giza_trim(ext);

  /*printf(" got prefix = \"%s\" extension=\"%s\" \n",prefixtrim,ext);*/

  if (!strstr(lprefix,lextens)) {
  /* Add the device extension if prefix string does not already contain it */
     if (pgNum == 0) {
        sprintf (filename, "%s%s", prefixtrim, ext);
     } else {
        sprintf (filename, "%s_%04d%s", prefixtrim, pgNum, ext);
     }
  } else {
  /* Do not add the device extension if the prefix already contains it  */
     if (pgNum == 0) {
        sprintf (filename, "%s", prefixtrim);
     } else {
        /*
         * Here we need a number, but it should come BEFORE the device extension
         * so find the first occurrence of . in the filename, and insert number
         * before this position.
         */
        char *firstpart = strsep (&prefixtrim,".");
        sprintf (filename, "%s_%04d%s", firstpart, pgNum, ext);
     }
  }
}
