/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2015 Daniel Price
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
#include "giza-stroke-private.h"
#include "giza-subpanel-private.h"
#include "giza-text-background-private.h"
#include <giza.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#define GIZA_DEFAULT_MARGIN 0

static void _giza_set_prefix (const char *prefix);
static int _giza_get_internal_id(int devid);
static int _giza_device_id = 0;

/* global settings */
giza_settings_t Sets;
int id;
/* per-device settings */
giza_device_t Dev[GIZA_MAX_DEVICES];

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
 *  -<=0 :- an error has occurred
 *  -1   :- id of current device
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
 *
 *  For hardcopy devices, the device type can also be recognised
 *  automatically by specifying the filename
 *  e.g: giza_open_device("plot.png","giza")
 *
 * See Also: giza_select_device, giza_get_device_id, giza_close_device, giza_query_device
 */
int
giza_open_device (const char *newDeviceName, const char *newPrefix)
{
  return giza_open_device_size (newDeviceName, newPrefix, 0., 0., 0);
}

/**
 * Device: giza_open_device_size
 *
 * Synopsis: Similar to giza_open_device, but allows one to specify the size of the device
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
 *
 * See also: giza_open_device
 */
int
giza_open_device_size (const char *newDeviceName, const char *newPrefix, double width, double height, int units)
{
  _giza_device_id += 1;
  id = _giza_get_internal_id(_giza_device_id);
  
  /* Some general initialisation */
  Dev[id].pgNum = 0;
  Dev[id].type = GIZA_DEVICE_IV;
  Dev[id].defaultBackgroundAlpha = 1.;
  Dev[id].deviceOpen = 1;
  int success = -1;
  giza_set_text_background (-1);
  giza_start_prompting ();
  _giza_init();

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
    {
    Dev[id].type = _giza_prompt_for_device ();
    }
  else
    {
      char *devTypeStr;
      _giza_split_device_string (newDeviceName, &devTypeStr);
      Dev[id].type = _giza_device_to_int (devTypeStr);
    }

  /* Determine which driver is required */
  switch (Dev[id].type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      success = _giza_open_device_xw (width, height, units);
      break;
#endif
    case GIZA_DEVICE_PNG:
      success = _giza_open_device_png (width, height, units);
      break;
    case GIZA_DEVICE_PDF:
      success = _giza_open_device_pdf (width, height, units, 0);
      break;
    case GIZA_DEVICE_VPDF:
      success = _giza_open_device_pdf(width, height, units, 1);
      break;
    case GIZA_DEVICE_PS:
      success = _giza_open_device_ps (width, height, units, 0);
      break;
    case GIZA_DEVICE_VPS:
      success = _giza_open_device_ps (width, height, units, 1);
      break;
    case GIZA_DEVICE_SVG:
      success = _giza_open_device_svg (width, height, units, 0);
      break;
    case GIZA_DEVICE_NULL:
      success = _giza_open_device_null (width, height, units);
      break;
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      success = _giza_open_device_eps (width, height, units, 0);
      break;
#endif
    case GIZA_DEVICE_IV:
    default:
      _giza_error ("giza_open_device", "Unknown device, device not opened");
      return -1;
    }

  /* check that the surface was created */
  if (success) {
     _giza_device_id = _giza_device_id - 1;
     id = _giza_get_internal_id(_giza_device_id);
     return -1;
  }
  /* bind the created surface to our Dev[id].context */
  Dev[id].context = cairo_create (Dev[id].surface);
  if (!Dev[id].context)
    {
      _giza_error ("giza_open_device", "Could not create cairo surface.");
      giza_close_device();
      return -1;
    }

  /* some final initialisation */
  _giza_init_subpanel ();
  Dev[id].drawn = 0;
  Dev[id].resize = 0;
  Dev[id].fontAngle = 0.;
  _giza_init_arrow_style ();
  _giza_init_line_style ();
  _giza_init_colour_index ();

  giza_draw_background ();
  _giza_init_colour_table ();
  _giza_set_trans (GIZA_TRANS_IDEN);
  _giza_init_norm ();
  
  /* font and character height initialisations
     need to come before set_viewport_default */
  _giza_init_character_height (); /* must come BEFORE init_font */
  _giza_init_font ();
  _giza_init_text_background();

  _giza_init_window (); /* call init_window BEFORE set_viewport */
  giza_set_viewport_default ();
  giza_set_line_width (1);

  _giza_init_fill ();
  _giza_init_band_style ();
  _giza_init_save ();
  giza_set_clipping(1);
  /*_giza_stroke();*/
  
  return _giza_device_id;
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
 * For internal book-keeping
 * To the outside world, the device ids run from 1->Ndevices
 * whereas within giza the referencing (id) is from 0->Ndevices-1
 */
int _giza_get_internal_id(int devid)
{
  return devid - 1;
}

/**
 * Device: giza_get_device_id
 *
 * Synopsis: Returns the id of the currently selected device
 *
 * Output:
 *  -devid, as returned by giza_open_device
 *
 * See also: giza_open_device, giza_get_device_id
 */
void
giza_get_device_id (int *devid)
{
  *devid = _giza_device_id;
  return;
}

/**
 * Device: giza_select_device
 *
 * Synopsis: Select between the currently open devices
 *
 * Input:
 *  -devid :- device id, as returned by giza_open_device
 *
 * See also: giza_get_device_id, giza_open_device
 */
void
giza_select_device (int devid)
{
  if (devid > 0 && devid <= GIZA_MAX_DEVICES) {
    /* need to check if device is open */
    int tmpid = _giza_get_internal_id(devid);
    if (!Dev[tmpid].deviceOpen) 
      {
        _giza_error ("giza_select_device", "Invalid/closed device selected");

      } else {

        _giza_device_id = devid;
        id = _giza_get_internal_id(_giza_device_id);

      }

    /*
     * select between currently open windows
     * for interactive devices 
     */
    switch (Dev[id].type)
      {
#ifdef _GIZA_HAS_XW
      case GIZA_DEVICE_XW:
        _giza_select_xw (id);
        break;
#endif
      }
  } else {
    _giza_error ("giza_select_device", "Invalid device selected");
  }
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

  Dev[id].drawn = 1;
  
  if (!Dev[id].buf) /* do not flush if buffering is on */
    {
     switch (Dev[id].type)
       {
#ifdef _GIZA_HAS_XW
       case GIZA_DEVICE_XW:
         _giza_flush_device_xw ();
         break;
#endif
       default:
         if (!Dev[id].surface)
           {
             _giza_error ("giza_flush_device", "No device open, cannot flush");
             return;
           } else {
             cairo_surface_flush(Dev[id].surface);
           }
         return;
       }
    }
  return;
}

/**
 * Device: giza_resize_device
 *
 * Synopsis: resizes the currently open device.
 */
void
_giza_resize_device (int width, int height)
{
  switch (Dev[id].type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_change_size_xw (width + 40, height + 40);
      _giza_init_norm ();
      double ch;
      giza_get_character_height(&ch);
      giza_set_character_height(ch);
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
 *
 * See also: giza_subpanel, giza_set_panel
 */
void
giza_change_page (void)
{

  /* If sub-panels are used, just change panel if we are not in the last panel */
  if (Dev[id].nx > 1 || Dev[id].ny > 1) {
     int newpage;
     _giza_advance_panel(&newpage);
     if (!newpage) return;
  }

   /* allow resizing of the device if nothing has been drawn */
  if (!Dev[id].drawn && !Dev[id].resize) {

    /* if nothing has changed, safe to redraw/reset the background colour */
    giza_draw_background ();
    
    /* do nothing */
    return;
  }

  if (!_giza_check_device_ready ("giza_change_page"))
    return;

  /* save a whole bunch of settings
    (line style, width, colour index etc.) */
  giza_save();

  switch (Dev[id].type)
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
      _giza_change_page_null ();
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

  if (Dev[id].resize) 
    {
      _giza_init_norm();
    }

  if (Dev[id].prompting && Dev[id].isInteractive && !Dev[id].resize)
    {
      _giza_newpage_prompt();
    }

  Dev[id].pgNum++;

  /* Reset stuff */
  giza_set_panel(1,1); /* also calls set_viewport */
  /*giza_set_viewport (Dev[id].VP.xmin,Dev[id].VP.xmax, 
                     Dev[id].VP.ymin, Dev[id].VP.ymax);*/
  giza_set_window (Dev[id].Win.xmin, Dev[id].Win.xmax,
                   Dev[id].Win.ymin, Dev[id].Win.ymax);

  /* restore the previously saved settings */
  giza_restore();

  Dev[id].drawn = 0;
  Dev[id].resize = 0;
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

  if (Dev[id].prompting && Dev[id].isInteractive)
    {
      cairo_surface_finish (Dev[id].surface);
      _giza_newpage_prompt();
    }

  /* destroy the cairo context */
  if (Dev[id].context) cairo_destroy(Dev[id].context);

  switch (Dev[id].type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_close_device_xw ();
      break;
#endif
    case GIZA_DEVICE_PNG:
      _giza_close_device_png (1);
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

  Dev[id].deviceOpen = 0;

  /* Destroy the font */
  _giza_free_font ();
  _giza_free_colour_table ();
  Dev[id].type = GIZA_DEVICE_IV;
  _giza_device_id = _giza_device_id - 1;
  id = _giza_get_internal_id(_giza_device_id);

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
       if (!_giza_int_to_device(Dev[id].type,returnval))
         {
           ierr = 1;
         }
     }
  /* Query whether or not device has cursor */
  else if (!strcmp(querytype,"cursor"))
    {
      if (Dev[id].isInteractive)
        {
          strncpy(returnval,"YES",sizeof(returnval)-1);
        }
      else
        {
          strncpy(returnval,"NO",sizeof(returnval)-1);
        }
    }
  /* Query whether or not device is hard copy or not */
  else if (!strcmp(querytype,"hardcopy"))
    {
      if (Dev[id].isInteractive)
        {
          strncpy(returnval,"NO",sizeof(returnval)-1);
        }
      else
        {
          strncpy(returnval,"YES",sizeof(returnval)-1);
        }
    }
  /* Query whether or not device is open or not */
  else if (!strcmp(querytype,"state"))
    {
       if (Dev[id].deviceOpen)
         {
            strncpy(returnval,"OPEN",sizeof(returnval)-1);         
         }
       else
         {
            strncpy(returnval,"CLOSED",sizeof(returnval)-1);
         }    
    }
  /* Query current user */
  else if (!strcmp(querytype,"user"))
    {
       strncpy(returnval,getlogin(),sizeof(returnval)-1);
    }
  /* Query current device name */
  else if (!strcmp(querytype,"device"))
    {
       strncpy(returnval,Dev[id].prefix,sizeof(returnval)-1);
    }
  /* Query current device/type */
  else if (!strcmp(querytype,"dev/type"))
    {
       strncpy(returnval,Dev[id].prefix,sizeof(returnval)-1);
       if (!_giza_int_to_device(Dev[id].type,devType))
         {
           ierr = 1;
           strncat(returnval,devType,4*sizeof(char));
         }
    }
  /* Query current filename (as entered by user) */
  else if (!strcmp(querytype,"file"))
    {
       if (!Dev[id].isInteractive)
         {
           strncpy(returnval,Dev[id].prefix,sizeof(returnval)-1);
         }
    }
  returnval[sizeof(returnval)-1] = '\0'; /* make sure string is null-terminated */
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
_giza_get_key_press (int mode, int moveCurs, int nanc, const double *xanch, const double *yanch, double *x, double *y, char *ch)
{
  if (!_giza_check_device_ready ("_giza_get_key_press"))
    return 1;

  switch (Dev[id].type)
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
      _giza_get_key_press_xw (mode, moveCurs, nanc, xanch, yanch, x, y, ch);
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
      /* look for file extension (.blah) at end of filename 
       * no need to check details of this as will not be
       * recognised as a device if not valid hardcopy file extension
       * and this is only done if / not present
       */
      *devType = strrchr (deviceString, (int) '.');
      if (*devType == NULL)
         {
           *devType = deviceString;
           return;
         }
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
        || !strcmp (devName, "/xs")
        || !strcmp (devName, "/xserve")
        || !strcmp (devName, "/xwindow")
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
/*
 * For hardcopy devices, allow the user to just
 * specify the device extension if no /dev is present
 */
      if (!strcmp (devName, ".png"))
        newDevice = GIZA_DEVICE_PNG;
      else if (!strcmp (devName, ".svg"))
        newDevice = GIZA_DEVICE_SVG;
      else if (!strcmp (devName, ".pdf"))
        newDevice = GIZA_DEVICE_PDF;
      else if (!strcmp (devName, ".ps"))
        newDevice = GIZA_DEVICE_PS;
#ifdef _GIZA_HAS_EPS
      else if (!strcmp (devName, ".eps"))
        newDevice = GIZA_DEVICE_EPS;
#endif
      else {
         newDevice = GIZA_DEVICE_IV;
      }
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
      strncpy(DeviceName,"/null",sizeof(DeviceName)-1);
      break;
    case GIZA_DEVICE_PDF:
      strncpy(DeviceName,"/pdf",sizeof(DeviceName)-1);
      break;
    case GIZA_DEVICE_VPDF:
      strncpy(DeviceName,"/vpdf",sizeof(DeviceName)-1);
      break;
    case GIZA_DEVICE_PNG:
      strncpy(DeviceName,"/png",sizeof(DeviceName)-1);
      break;
    case GIZA_DEVICE_SVG:
      strncpy(DeviceName,"/svg",sizeof(DeviceName)-1);
      break;
    case GIZA_DEVICE_PS:
      strncpy(DeviceName,"/ps",sizeof(DeviceName)-1);
      break;
    case GIZA_DEVICE_VPS:
      strncpy(DeviceName,"/vps",sizeof(DeviceName)-1);
      break;
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      strncpy(DeviceName,"/xw",sizeof(DeviceName)-1);
      break;
#endif
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      strncpy(DeviceName,"/eps",sizeof(DeviceName)-1);
      break;
#endif
    default:
    /* Do not print an error here as this is an internal routine:
       Instead, make sure the error is handled in the calling routine */
      strncpy(DeviceName," ",sizeof(DeviceName)-1);
      ierr = 1;
    }
    DeviceName[sizeof(DeviceName)-1] = '\0'; /* make sure string is null-terminated */
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
  *deviceList = malloc (1000 * sizeof(char)); /* be CAREFUL here to make sure the string is big enough */

  *deviceList[0] = '\0';
#ifdef _GIZA_HAS_XW
  strcat (*deviceList, "Interactive devices:\n\t/xw\t(X Window)\n");
#endif
  strcat (*deviceList, "Non-interactive file formats:\n");
  strcat (*deviceList, "\t/png or file.png   (Portable network graphics file)\n");
  strcat (*deviceList, "\t/svg or file.svg   (Scalable vector graphics file)\n");
#ifdef _GIZA_HAS_EPS
  strcat (*deviceList, "\t/eps or file.eps   (Encapsulated Postscript, one file per page)\n");
#endif
  strcat (*deviceList, "\t/pdf or file.pdf   (Portable document format file)\n");
  strcat (*deviceList, "\t/vpdf              (Portable document format file portrait)\n");
  strcat (*deviceList, "\t/ps  or file.ps    (Postscript file, multiple pages per file)\n");
  strcat (*deviceList, "\t/vps               (Postscript file portrait, multiple pages per file)\n");
  strcat (*deviceList, "\t/null              (Null device)\n");
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

  switch (Dev[id].type)
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

      xx = (double) Dev[id].width - 2.*GIZA_DEFAULT_MARGIN;
      yy = 2.*GIZA_DEFAULT_MARGIN - (double) Dev[id].height;
      x0 = GIZA_DEFAULT_MARGIN;
      y0 = (double) Dev[id].height - GIZA_DEFAULT_MARGIN;
      cairo_matrix_init (&(Dev[id].Win.normCoords), xx, 0, 0, yy, x0, y0);
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
  switch (Dev[id].type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_expand_clipping_xw ();
      break;
#endif
    default:
      _giza_set_trans (GIZA_TRANS_IDEN);
      cairo_reset_clip (Dev[id].context);
      cairo_rectangle (Dev[id].context, 0, 0, Dev[id].width, Dev[id].height);
      cairo_clip (Dev[id].context);
      break;
    }
}

/**
 * Allocates the array for Dev[id].prefix and sets it to prefix
 */
static void
_giza_set_prefix (const char *prefix)
{
  if (strlen(prefix) > sizeof(Dev[id].prefix))
     {
        _giza_error("giza_set_prefix","device name exceeds maximum string length");
     }
  strncpy (Dev[id].prefix, prefix, sizeof(Dev[id].prefix));
}

/*
 * Creates the surfaces for drawing the band
 */
int
_giza_init_band (int mode)
{
  if (mode == 0) return 0;

  int success = 1;
  switch (Dev[id].type)
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
  double lwDevice = Band.lw * Dev[id].deviceUnitsPermm * 0.25;
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
void _giza_get_filename_for_device (char *filename, char *prefix, int pgNum, char *extension,
                                    int lastpage)
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

  if (!strstr(lprefix,lextens)) {
  /* Add the device extension if prefix string does not already contain it */
     if (pgNum == 0 && lastpage != 0) {
        sprintf (filename, "%s%s", prefixtrim, ext);
     } else {
        sprintf (filename, "%s_%04d%s", prefixtrim, pgNum, ext);
     }
  } else {
  /* Do not add the device extension if the prefix already contains it  */
     if (pgNum == 0 && lastpage != 0) {
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
