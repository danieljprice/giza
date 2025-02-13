/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2015 Daniel Price
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

#include "giza-private.h"
#include "giza-drivers-private.h"
#include "giza-driver-xw-private.h"
#include "giza-driver-png-private.h"
#include "giza-driver-mp4-private.h"
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
static void _giza_close_device_unchecked (void);
static void _giza_init_device_struct(giza_device_t*);

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
 *          Note: this is the EXTERNAL device number, running from 1..Ndev
 *          The internal device numbers are array indices into the Dev[...]
 *          array and run from 0..Ndev-1
 *
 * Available devices:
 *  -?    :- User selects device
 *  -/xw  :- X-window
 *  -/eps :- Encapsulated Postscript
 *  -/png :- Portable Network Graphics file
 *  -/mp4 :- Mpeg4 movie [requires ffmpeg support]
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
 * See Also: giza_open_device
 */
int
giza_open_device_size (const char *newDeviceName, const char *newPrefix, double width, double height, int units)
{
  static int didInit = 0;

  if( !didInit ) {
         giza_device_t* pDev;
      for(pDev = &Dev[0]; pDev < &Dev[GIZA_MAX_DEVICES]; pDev++)
          _giza_init_device_struct( pDev );
       didInit = 1;
  }

  if( !newDeviceName || !strlen(newDeviceName) ) {
      _giza_error("giza_open_device", newDeviceName==NULL ? "(nullptr devicename)" : "empty device name");
      return -1;
  }

  /* request to open a new device.
   * see if we can find an unused entry in our list of devices */
  int   newId;

  for( newId=0; newId<GIZA_MAX_DEVICES; newId++ )
      if( Dev[newId].deviceOpen==0 )
          break;
  if( newId>=GIZA_MAX_DEVICES ) {
      _giza_error("giza_open_device", "No more free devices (%d in use). Close some first.", GIZA_MAX_DEVICES);
      return -1;
  }

  /* Some general initialisation.
   * Everything was already zeroed out + type was already == GIZA_DEVICE_IV */

  /* Change the "current device id" to the newly allocated device
   * such that routines can initialize it properly.
   * Note that if something fails we have to put back the
   * previous current device id */
  const int prevId = id;

  id = newId;

  _giza_init();

  _giza_set_prefix( newPrefix!=0 ? newPrefix : GIZA_DEFAULT_PREFIX );

  Dev[id].number_format = GIZA_NUMBER_FORMAT_AUTO;

  /* Determine which type of device to open */
  char firstchar = newDeviceName[0];
  if (firstchar == '?')
    {
      Dev[id].type = _giza_prompt_for_device ();
    }
  else
    {
      char const *devTypeStr;
      _giza_split_device_string (newDeviceName, &devTypeStr);
      Dev[id].type = _giza_device_to_int (devTypeStr);
    }

  /* Determine which driver is required */
  int success;

  switch (Dev[id].type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      success = _giza_open_device_xw (width, height, units);
      break;
#endif
    case GIZA_DEVICE_MP4:
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
      success = 666;
      break;
    }

  /* check that the surface was created */
  if (success!=0 || Dev[id].surface==0) {
      /* Let giza_close_device_unchecked() reset the contents of the
       * Dev[...] that we tried to initialize */
      _giza_close_device_unchecked ();
      /* And put back the old device */
      id = prevId;
      return -1;
  }
  /* bind the created surface to our Dev[id].context */
  Dev[id].context = cairo_create (Dev[id].surface);
  if (!Dev[id].context)
    {
      _giza_error ("giza_open_device", "Could not create cairo context.");
      /* call the unchecked close device function because this one
       * hasn't been fully constructed yet */
      _giza_close_device_unchecked();
      /* and put back previously active device */
      id = prevId;
      return -1;
    }

  /* Do the rest of the initialization.
   * These routines check for Dev[id].deviceOpen so we must set that */
  Dev[id].deviceOpen = 1;
  Dev[id].defaultBackgroundAlpha = 1.;
  Dev[id].motion_callback = NULL;

  giza_set_text_background (-1);
  giza_start_prompting ();
  _giza_init_subpanel ();
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
  return id+1;
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
 * Synopsis: Returns the (external) id of the currently selected device
 *
 * Output:
 *  -devid, as returned by giza_open_device
 *
 * See Also: giza_open_device, giza_get_device_id
 */
void
giza_get_device_id (int *devid)
{
  *devid = id + 1;
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
 * See Also: giza_get_device_id, giza_open_device
 */
void
giza_select_device (int devid)
{
    /* attempt to map external device id to internal id */
    int tmpid = _giza_get_internal_id(devid);

    if( tmpid<0 || tmpid>=GIZA_MAX_DEVICES ) {
        _giza_error ("giza_select_device", "Invalid device number %d selected", devid);
        return;
    }
    /* need to check if device is open */
    if (!Dev[tmpid].deviceOpen) {
        _giza_error ("giza_select_device", "Invalid/closed device %d selected", devid);
        return;
    }
    /* The device can be selected */
    id = tmpid;
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
 * Device: giza_change_page
 *
 * Synopsis: Advances the currently open device to the next page, and redraws
 * the background. If no other actions have been performed since the device was
 * opened or the last call to giza_change_page the call is ignored.
 *
 * See Also: giza_subpanel, giza_set_panel
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

  /* PGPLOT keeps a flag per device ("pgplot.inc", "PGADVS(PGMAXD)"
   * (PGMAXD = maximum number of PGPLOT devices open at the same time)
   * to be able to skip asking for the first page. This flag is set
   * in PGOPEN and cleared in PGPAGE
   */
  if (Dev[id].prompting && Dev[id].isInteractive && !Dev[id].firstPage)
    {
      _giza_newpage_prompt();
    }
  /* OK skipped the first 'ask' */
  Dev[id].firstPage = 0;

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
    case GIZA_DEVICE_MP4:
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

  /* Only increase pagenumber if there was content */
  if (Dev[id].drawn)
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
 * Device: giza_close_devices
 *
 * Synopsis: Close all open devices (to be called from PGEND())
 */
void
giza_close_devices (void)
{
  for(id = 0; id<GIZA_MAX_DEVICES; id++)
      if( Dev[id].deviceOpen )
          giza_close_device ();
  /* force-unload the font cache */
  _giza_unload_font_cache ();
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
  /* we've done our sanity checks so clean up! */
  _giza_close_device_unchecked();

  /* if no open devices remain, force-unload the font cache.
   * it will be rebuilt as soon as new devices will be opened */
  unsigned int nOpen = 0;
  giza_device_t* p;
  for(p=&Dev[0]; p<&Dev[GIZA_MAX_DEVICES]; p++)
      if( p->deviceOpen )
          nOpen++;
  if( nOpen==0 )
    _giza_unload_font_cache ();
}


/**
 * In order to be able to share the cleaning-up code between
 * giza_close_device() and giza_open_device_size() [when something fails
 * during the construction of a new device] the actual cleaning up code
 * is put in this internal function.
 *
 * This function deletes the context and calls a type-specific close routine
 * to delete the surface. Some other resources (e.g. font) are released as
 * well. After that the contents of Dev[id] are returned to a known, default
 * state: all zeroes and .type set to GIZA_DEVICE_IV.
 *
 * giza_close_device() can do its sanity checks as it is an API function and
 * then call the _unchecked() version to do the heavy lifting;
 * giza_open_device_size() can call this one directly as it knows it doesn't
 * have to do any sanity checks.
 */
void
_giza_close_device_unchecked (void) {
  /* destroy the cairo context */
  if (Dev[id].context)
      cairo_destroy(Dev[id].context);

  switch (Dev[id].type)
    {
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      _giza_close_device_xw ();
      break;
#endif
    case GIZA_DEVICE_MP4:
      _giza_close_device_mp4(1);
      break;
    case GIZA_DEVICE_PNG:
      _giza_close_device_png(1);
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
      /* unrecognized device type => don't do anything but finish other cleanup*/
       break;
    }

  /* Destroy the font */
  _giza_free_font ();
  _giza_free_colour_table ();

  /* Return device struct contents to known empty state */
  _giza_init_device_struct( &Dev[id] );
}

/**
 * Resets the contents of a giza_device_t to a known state:
 * the whole struct is zeroed out and .type is set to GIZA_DEVICE_IV.
 *
 * Note: this approach was chosen over having a static/const/global
 * giza_device_t using a compond struct initializer like this:
 *    static const giza_device_t prototype = { .type = GIZA_DEVICE_IV };
 * since the opinions differ a bit on exactly which C standard does exactly
 * what with members that are not named in the initializer list.
 * See https://stackoverflow.com/questions/330793/how-to-initialize-a-struct-in-accordance-with-c-programming-language-standards
 *
 * Input:
 *  -ptrDev :- a pointer to a giza_device_t
 */
void
_giza_init_device_struct(giza_device_t* ptrDev) {
    memset(ptrDev, 0x0, sizeof(giza_device_t));
    ptrDev->type = GIZA_DEVICE_IV;
}

/**
 * Device: giza_query_device
 *
 * Synopsis: Queries various things about the current device.
 *
 * Input:
 *  -querytype :- a string containing the query type
 *  -rlen      :- integer containing the length of the return buffer
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
giza_query_device (const char *querytype, char *returnval, int* rlen)
{
  int       ierr = 0;
  char      devType[12];
  const int max_chars = *rlen - 1;

  if (max_chars<=0)
    {
        _giza_warning("giza_query_device", "destination string says it has %d characters available querying %s", max_chars, querytype);
        return 1;
    }
  /* Query device type */

  if (!strcasecmp(querytype,"type"))
     {
       if (!_giza_int_to_device(Dev[id].type,returnval, max_chars))
         {
           ierr = 1;
         }
     }
  /* Query whether or not device has cursor */
  else if (!strcasecmp(querytype,"cursor"))
    {
      strncpy(returnval, Dev[id].isInteractive ? "YES" : "NO", max_chars);
    }
  /* Query whether or not device is hard copy or not */
  else if (!strcasecmp(querytype,"hardcopy"))
    {
      strncpy(returnval, Dev[id].isInteractive ? "NO" : "YES", max_chars);
    }
  /* Query whether or not device is open or not */
  else if (!strcasecmp(querytype,"state"))
    {
      strncpy(returnval, Dev[id].deviceOpen ? "OPEN" : "CLOSED", max_chars);
    }
  /* Query current user */
  else if (!strcasecmp(querytype,"user"))
    {
       strncpy(returnval,getlogin(),max_chars);
    }
  /* Query current device name */
  else if (!strcasecmp(querytype,"device"))
    {
       strncpy(returnval,Dev[id].prefix,max_chars);
    }
  /* Query current device/type */
  else if (!strcasecmp(querytype,"dev/type"))
    {
       strncpy(returnval,Dev[id].prefix,max_chars);
       if (!_giza_int_to_device(Dev[id].type,devType,sizeof(devType)))
         {
           ierr = 1;
           strncat(returnval,devType,4*sizeof(char));
         }
    }
  /* Query current filename (as entered by user) */
  else if (!strcasecmp(querytype,"file"))
    {
       if (!Dev[id].isInteractive)
         {
           strncpy(returnval,Dev[id].prefix,max_chars);
         }
       else
         {
           strncpy(returnval, " ", max_chars);
         }
    }
  returnval[max_chars] = '\0'; /* make sure string is null-terminated */
  return ierr;
}

/**
 * Device: giza_set_motion_callback
 *
 * Synopsis: set a callback function to be called
 *           during cursor movement (e.g. to print things).
 *           Function should be of the form
 *           void func(double *x, double *y)
 *
 * Input:
 *  -func  :- The subroutine to be called
 *
 * See Also: giza_end_motion_callback
 *
 */
int
giza_set_motion_callback (void (*func)(double *x, double *y, int *mode))
{
  if (!_giza_check_device_ready ("giza_set_motion_callback"))
    return 1;

  /* assign the motion pointer callback */
  Dev[id].motion_callback = func;

  return 0;
}

/**
 * Device: giza_end_motion_callback
 *
 * Synopsis: Free the motion callback pointer
 *
 * See Also: giza_set_motion_callback
 *
 */
int
giza_end_motion_callback (void)
{
  if (!_giza_check_device_ready ("giza_end_motion_callback"))
    return 1;

  if (Dev[id].motion_callback != NULL)
     {
       Dev[id].motion_callback = NULL;
       return 0;
     }
  else
     {
       return 1; /* return error if already null */
     }

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
_giza_split_device_string (const char *deviceString, char const **devType)
{
  if (deviceString == NULL)
    return;

  *devType = strrchr (deviceString, (int) '/');

  /* Can either set the device name as 'myfile/png'
     or with a file extension 'myfile.png'
     try the latter either if no '/' found,
     or if the string after the '/' contains a dot
     as in 'dir/myfile.png'
   */
  if (*devType == NULL || strrchr(*devType,(int) '.') != NULL)
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
      _giza_trim(devName);
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
  /* trim spaces from the device string */
  _giza_trim(devName);

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
  else if (!strcmp(devName, "/mp4"))
    newDevice = GIZA_DEVICE_MP4;
  else if (!strcmp(devName, "/svg"))
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
      else if (!strcmp(devName, ".mp4"))
        newDevice = GIZA_DEVICE_MP4;
      else if (!strcmp(devName, ".svg"))
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
_giza_int_to_device (int numDevice, char *DeviceName, int rval)
{

 int       ierr = 0;
 const int max_chars = rval - 1;
 switch (numDevice)
    {
    case GIZA_DEVICE_NULL:
      strncpy(DeviceName,"/null",max_chars);
      break;
    case GIZA_DEVICE_PDF:
      strncpy(DeviceName,"/pdf",max_chars);
      break;
    case GIZA_DEVICE_VPDF:
      strncpy(DeviceName,"/vpdf",max_chars);
      break;
    case GIZA_DEVICE_PNG:
      strncpy(DeviceName,"/png",max_chars);
      break;
    case GIZA_DEVICE_MP4:
      strncpy(DeviceName, "/mp4", max_chars);
      break;
    case GIZA_DEVICE_SVG:
      strncpy(DeviceName, "/svg", max_chars);
      break;
    case GIZA_DEVICE_PS:
      strncpy(DeviceName,"/ps",max_chars);
      break;
    case GIZA_DEVICE_VPS:
      strncpy(DeviceName,"/vps",max_chars);
      break;
#ifdef _GIZA_HAS_XW
    case GIZA_DEVICE_XW:
      strncpy(DeviceName,"/xw",max_chars);
      break;
#endif
#ifdef _GIZA_HAS_EPS
    case GIZA_DEVICE_EPS:
      strncpy(DeviceName,"/eps",max_chars);
      break;
#endif
    default:
    /* Do not print an error here as this is an internal routine:
       Instead, make sure the error is handled in the calling routine */
      strncpy(DeviceName," ",max_chars);
      ierr = 1;
    }
    DeviceName[max_chars] = '\0'; /* make sure string is null-terminated */
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
  strcat (*deviceList, "\t/mp4 or file.mp4   (mpeg4 movie [requires ffmpeg installation])\n");
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
void
_giza_set_prefix (const char *prefix)
{
  if (strlen(prefix) > sizeof(Dev[id].prefix))
     {
        _giza_error("giza_set_prefix","device name exceeds maximum string length");
     }
  strncpy (Dev[id].prefix, prefix, sizeof(Dev[id].prefix));
  _giza_trim(Dev[id].prefix);
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
  char lprefix[strlen(prefix) + 1];
  char lextens[strlen(extension) + 1];
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
