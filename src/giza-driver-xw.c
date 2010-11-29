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
#include "giza-window-private.h"
#include "giza-io-private.h"
#include "giza-drivers-private.h"
#include "giza-driver-xw-private.h"
#include "giza-prompting-private.h"
#include "giza-transforms-private.h"
#include "giza-character-size-private.h"
#include "giza-band-private.h"

#ifdef _GIZA_HAS_XW

#include <giza.h>
#include <cairo/cairo-xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

/**
 * Global variables specific to X.
 */
struct GIZA_XWindow
{
  Display *display;
  Pixmap pixmap;
  Window window;
  Screen *screenptr;
  int screennum;
  Visual *visual;
  GC gc;
  XImage *ximage;
  Colormap colormap;
  
  unsigned char *virtualscreen;
  int videoaccesstype;

  int width;
  int height;
  int depth;
  int pixelsize;
  int screensize;
} XW;

#define GIZA_DEFAULT_WIDTH 800
#define GIZA_DEFAULT_HEIGHT 600
#define GIZA_DEVICE_UNITS_PER_MM 3.7054 // so "width" is 8.5 inches as in ps driver
#define GIZA_DEVICE_INTERACTIVE 1
#define GIZA_XW_MARGIN 20

static void _giza_xevent_loop (int mode, int moveCurs, int anchorx, int anchory, int *x, int *y, char *ch);
static void _giza_expose_xw (XEvent *event);
static void _giza_change_size_xw (int width, int height);

/**
 * Opens an XWindow device for drawing to.
 *
 * Return values:
 *  -0 :- no error.
 *  -1 :- No connection to he X server could be made.
 *  -3 :- No X visual could be created.
 *  -4 :- No window could be created.
 *  -5 :- No cairo surface could be created.
 */
int
_giza_open_device_xw ()
{
  Dev.deviceUnitsPermm = GIZA_DEVICE_UNITS_PER_MM;
  Dev.isInteractive = GIZA_DEVICE_INTERACTIVE;

  // set all device specific settings
  if (!_giza_sizeSpecified ())
    {
      Dev.width = GIZA_DEFAULT_WIDTH;
      Dev.height = GIZA_DEFAULT_HEIGHT;
    }    
  else
    {
      Dev.width = (int) (Dev.deviceUnitsPermm * 10. * Dev.widthCM) + 1;
      Dev.height = (int) (Dev.deviceUnitsPermm * 10. * Dev.heightCM) + 1;
    }  

  // set the XLib stuff
  XW.width = Dev.width + 2 * GIZA_XW_MARGIN;
  XW.height = Dev.height + 2 * GIZA_XW_MARGIN;

  // open the connection to the sever and check there was no error
  XW.display = XOpenDisplay (NULL);
  if (!XW.display)
    {
      _giza_error ("_giza_open_device_xw", "Connection to the X server could not be made");
      return 1;
    }

  // set the depth
  XW.depth = 24;

  // get an identifier for the screen
  XW.screenptr = DefaultScreenOfDisplay(XW.display);
  XW.screennum = DefaultScreen (XW.display);

  // create a visual
  XW.visual = DefaultVisual (XW.display, XW.screennum);

  if (!XW.visual)
    {
      _giza_error ("_giza_open_device_xw", "Could not get X visual");
      return 3;
    }

  XW.gc = XDefaultGCOfScreen(DefaultScreenOfDisplay(XW.display));

  // create the window
  XW.window = XCreateSimpleWindow (XW.display,
				   DefaultRootWindow (XW.display),// make our new window a child of the entire XW.display
				   50, 50,	// origin
				   XW.width, XW.height, // size
				   0, 0, 0);
  if (!XW.window)
    {
      _giza_error ("_giza_open_device", "Could not create X window");
      return 4;
    }

  XStoreName (XW.display, XW.window, Dev.prefix);
  XMapWindow (XW.display, XW.window);

  // create the pixmap
  XW.pixmap = XCreatePixmap (XW.display, XW.window, (unsigned) XW.width, (unsigned) XW.height, (unsigned) XW.depth);

  // cairo stuff
  surface = cairo_xlib_surface_create (XW.display, XW.pixmap, XW.visual, XW.width, XW.height);

  if (!surface)
    {
      _giza_error ("_giza_open_device_xw", "Could not create surface");
      return 5;
    }

  return 0;
}

/**
 * Flushes the X device.
 */
void
_giza_flush_device_xw ()
{
  // flush the offscreen surface
  cairo_surface_flush (surface);

  // move the offscreen surface to the onscreen one
  XCopyArea (XW.display, XW.pixmap, XW.window, XW.gc, 0, 0, (unsigned) XW.width, (unsigned) XW.height, 0, 0);

  if (!XFlush (XW.display))
    {
      _giza_warning ("_giza_flush_device_xw", "Could not flush X window");
    }
}

/**
 * Advances the X window device to the next page.
 *
 * If prompting is on the old page is displayed until
 * a key is pressed or a mouse is clicked inside the X window.
 * Otherwise the old page is immediately lost.
 * 
 */
void
_giza_change_page_xw ()
{
  if (_giza_get_prompting ())
    {
      int x, y;
      char ch;
      _giza_xevent_loop (0, 0, 0, 0, &x, &y, &ch);
    }

  // create a new pixmap
  Pixmap old;
  old = XW.pixmap;
  XW.pixmap = XCreatePixmap (XW.display, XW.window, (unsigned) XW.width, (unsigned) XW.height, (unsigned) XW.depth);
  XFreePixmap (XW.display, old);

  // recreate the cairo surface and context
  cairo_surface_destroy (surface);
  surface = cairo_xlib_surface_create (XW.display, XW.pixmap, XW.visual, XW.width, XW.height);

  // Make the new context the same as the old one
  int oldLineCap = cairo_get_line_cap (context);
  int oldCi;
  char fontFam[20];
  giza_get_colour_index (&oldCi);
  giza_get_font (fontFam, 20);
  context = cairo_create (surface);
  _giza_draw_background_xw ();
  _giza_init_norm ();
  _giza_init_character_height ();
  giza_set_colour_index (oldCi);
  giza_set_line_cap (oldLineCap);
  giza_set_font (fontFam);
 }

/**
 * Initialises the normalised device coords matrix
 */
void
_giza_init_norm_xw ()
{
  cairo_matrix_init (&(Win.normCoords), Dev.width, 0, 0, -Dev.height, GIZA_XW_MARGIN,
		     Dev.height + GIZA_XW_MARGIN);
}

/**
 * NOTE: This needs to be redone!
 * redraws the background colour.
 */
void
_giza_draw_background_xw ()
{
  cairo_save (context);
  cairo_reset_clip (context);
  giza_set_colour_index (0);
  cairo_paint (context);
  cairo_restore (context);
}

/**
 * This routine closes the X window device and cleans up associated memory.
 *
 * If propting is turned on this will not occur until a key is pressed
 * or a mouse is clicked inside the X window.
 */
void
_giza_close_device_xw ()
{
  if (_giza_get_prompting ())
    {
      int x, y;
      char ch;
      _giza_xevent_loop (0, 0, 0, 0, &x, &y, &ch);
    }

  XCloseDisplay (XW.display);
  cairo_surface_destroy (surface);
}

/**
 * Loops indefinitely, redrawing and resizing the window as necessary until a key is pressed.
 * Returns the x and y position of the cursor (in device coords) and the key pressed.
 */
static void
_giza_xevent_loop (int mode, int moveCurs, int anchorx, int anchory, int *x, int *y, char *ch)
{
  //anchorx += GIZA_XW_MARGIN;
  //anchory += GIZA_XW_MARGIN;
  XEvent event;
  XSelectInput (XW.display, XW.window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | PointerMotionMask);

  _giza_init_band (mode);
  while (1)  {
    // wait for key press/expose
    XNextEvent(XW.display, &event);

    switch  (event.type) {
    case Expose: // redraw
      _giza_expose_xw (&event);
      break;
    case KeyPress: // return pos and char
      {
	char buffer[10];
	KeySym key;
	int nret;
      
	// get the ascii of the pressed key
	nret = XLookupString((XKeyEvent *)&event, buffer,
			     (int) (sizeof(buffer)/sizeof(char)), &key, NULL);
	
	// ignore irrelavent keys
	if(nret==1 && (key < XK_Shift_L || key > XK_Hyper_R)) {
	  *x = event.xkey.x ;//- GIZA_XW_MARGIN;
	  *y = event.xkey.y ;//- GIZA_XW_MARGIN;
	  
	  if(key)
	    *ch = buffer[0];

          _giza_destroy_band (mode);
	  return;
	};
      
	break;
      }
    case ConfigureNotify: // check if the window has been resized
      if(event.xconfigure.width != XW.width || event.xconfigure.height != XW.height)
	_giza_change_size_xw (event.xconfigure.width, event.xconfigure.height);
      break;
    case ButtonPress:
      {
        *x = event.xbutton.x ;//- GIZA_XW_MARGIN;
	*y = event.xbutton.y ;//- GIZA_XW_MARGIN;
	*ch = GIZA_LEFT_CLICK;
        _giza_destroy_band (mode);
	return;
      }
    case MotionNotify:
      {
        _giza_refresh_band (mode, anchorx, anchory, event.xmotion.x, event.xmotion.y);
      }
    default:
      break;
    }
    
  }

  _giza_destroy_band (mode);
}

/**
 * Deal with the xwindow expose event.
 */
static void
_giza_expose_xw (XEvent *event)
{
  XCopyArea (XW.display, XW.pixmap, XW.window, XW.gc, event->xexpose.x,
	     event->xexpose.y, (unsigned) event->xexpose.width, 
	     (unsigned) event->xexpose.height, event->xexpose.x,
	     event->xexpose.y);
  XFlush(XW.display);
}

/**
 * Changes the size of the current window
 */
void
_giza_change_size_xw (int width, int height)
{
  // Set the new device size
  Dev.width = width - 2 * GIZA_XW_MARGIN;
  Dev.height = height - 2 * GIZA_XW_MARGIN;

  XW.width = width;
  XW.height = height;
}

/**
 * Expands clipping so the whole surface can be drawn to.
 */
void
_giza_expand_clipping_xw ()
{
  _giza_set_trans (GIZA_TRANS_IDEN);
  cairo_reset_clip (context);
  cairo_rectangle (context, 0, 0, XW.width, XW.height);
  cairo_clip (context);
}

/**
 * Loops until a key is pressed. At this point the position in world coords of the cursor is returned, along with the key pressed.
 */
void
_giza_get_key_press_xw (int mode, int moveCurs, double xanc, double yanc, double *x, double *y, char *ch)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_user_to_device (context, &xanc, &yanc);
  int ix, iy;
  _giza_xevent_loop (mode, moveCurs, xanc, yanc, &ix, &iy, ch);

  *x = (double) ix;
  *y = (double) iy;
  
  cairo_device_to_user (context, x, y);
  _giza_set_trans (oldTrans);
}

/**
 * Creates the surfaces for drawing the band
 */
int
_giza_init_band_xw (int mode)
{
      // Set up box so it can draw the box...
      Band.onscreen = cairo_xlib_surface_create (XW.display, XW.window, XW.visual, XW.width, XW.height);
      Band.box = cairo_create (Band.onscreen);
      // How should we choose this colour??
      cairo_set_source_rgb (Band.box, 0., 0., 0.);

      // Set up restore to remove box
      Band.restore = cairo_create (Band.onscreen);
      cairo_set_source_surface (Band.restore, surface, 0, 0);
      Band.maxHeight = XW.height;
      Band.maxWidth = XW.width;
      return 1;
}

#endif
