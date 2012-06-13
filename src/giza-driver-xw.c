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
#include <string.h>
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
#define GIZA_DEVICE_UNITS_PER_MM 3.7054 /* so "width" is 8.5 inches as in ps driver */
#define GIZA_DEVICE_UNITS_PER_PIXEL 1.0 /* device units are pixels */
#define GIZA_DEVICE_INTERACTIVE 1
#define GIZA_XW_MARGIN 20

static void _giza_xevent_loop (int mode, int moveCurs, int anchorx, int anchory, int *x, int *y, char *ch);
static void _giza_expose_xw (XEvent *event);
static void _giza_flush_xw_event_queue (XEvent *event);
static int _giza_errors_xw (Display *display, XErrorEvent *error);

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
_giza_open_device_xw (void)
{
  Dev.deviceUnitsPermm    = GIZA_DEVICE_UNITS_PER_MM;
  Dev.deviceUnitsPerPixel = GIZA_DEVICE_UNITS_PER_PIXEL;
  Dev.isInteractive       = GIZA_DEVICE_INTERACTIVE;

  /* set all device specific settings */
  if (_giza_sizeSpecified ())
    {
      _giza_get_specified_size(&Dev.width, &Dev.height);
    }
  else
    {
      Dev.width = GIZA_DEFAULT_WIDTH;
      Dev.height = GIZA_DEFAULT_HEIGHT;
    }

  /* set the XLib stuff */
  XW.width = Dev.width + 2 * GIZA_XW_MARGIN;
  XW.height = Dev.height + 2 * GIZA_XW_MARGIN;

  /* open the connection to the sever and check there was no error */
  XW.display = XOpenDisplay (NULL);
  if (!XW.display)
    {
      _giza_error ("_giza_open_device_xw", "Connection to the X server could not be made");
      return 1;
    }

  /* get an identifier for the screen */
  XW.screenptr = DefaultScreenOfDisplay(XW.display);
  XW.screennum = DefaultScreen (XW.display);

  /* set the depth */
  XW.depth = DefaultDepth(XW.display,XW.screennum);

  /* Debugging info */
  /*
  printf("giza_xw_debug: XW display: %s\n",XDisplayName((char*)XW.display));
  printf("giza_xw_debug: XW monitor resolution: %d x %d\n",
                          DisplayWidth(XW.display,XW.screennum),
                          DisplayHeight(XW.display,XW.screennum));
  */
 /* printf("Connection number is %d\n",XW/); */

  if (XW.depth == 1)
    {
       _giza_error("_giza_open_device_xw","XW depth = 1: no colour possible");
    }
  else
    {
       /*printf("giza_xw_debug: XW colour depth = %d\n",XW.depth);*/
    }

  /* create a visual */
  XW.visual = DefaultVisual (XW.display, XW.screennum);

  if (!XW.visual)
    {
      _giza_error ("_giza_open_device_xw", "Could not get X visual");
      return 3;
    }

  /* create the window */
  XW.window = XCreateSimpleWindow (XW.display,
				   DefaultRootWindow (XW.display),/* make our new window a child of the entire XW.display */
				   50, 50,	/* origin */
				   XW.width, XW.height, /* size */
				   0, 0, 0);
  if (!XW.window)
    {
      _giza_error ("_giza_open_device", "Could not create X window");
      return 4;
    }

  XStoreName (XW.display, XW.window, Dev.prefix);
  XMapWindow (XW.display, XW.window);

   /* register interest in the delete window message */
  Atom wmDeleteMessage = XInternAtom(XW.display, "WM_DELETE_WINDOW", 1);
  XSetWMProtocols(XW.display, XW.window, &wmDeleteMessage, 1);

  /* register the routine to handle non-fatal X errors */
  XSetErrorHandler( _giza_errors_xw );

  /* create the pixmap */
  XW.pixmap = XCreatePixmap (XW.display, XW.window, (unsigned) XW.width, (unsigned) XW.height, (unsigned) XW.depth);

  /* Create graphics context */
/* XW.gc = XDefaultGCOfScreen(DefaultScreenOfDisplay(XW.display)); */
  /*XW.gc = XCreateGC (XW.display, XW.pixmap, 0, 0);*/
  /* version below works on older X11 distros */
  XW.gc = XDefaultGC (XW.display, XW.screennum);

  /* create Xlib surface in cairo */
  surface = cairo_xlib_surface_create (XW.display, XW.pixmap, XW.visual, XW.width, XW.height);
  /* uncomment below for drawing direct to window
   surface = cairo_xlib_surface_create (XW.display, XW.window, XW.visual, XW.width, XW.height);
   */
  if (!surface)
    {
      _giza_error ("_giza_open_device_xw", "Could not create surface");
      return 5;
    }

  Dev.defaultBackgroundAlpha = 1.;

  return 0;
}

/**
 * Flushes the X device.
 */
void
_giza_flush_device_xw (void)
{
  /* flush the offscreen surface */
  cairo_surface_flush (surface);
  /* move the offscreen surface to the onscreen one */
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
_giza_change_page_xw (void)
{
  /* create a new pixmap */
  cairo_destroy(context);
  cairo_surface_finish (surface);
  cairo_status_t status = cairo_surface_status (surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_change_page_xw",cairo_status_to_string(status));

  cairo_surface_destroy (surface);
  XFreePixmap (XW.display, XW.pixmap);
  XW.pixmap = XCreatePixmap (XW.display, XW.window, (unsigned) XW.width, (unsigned) XW.height, (unsigned) XW.depth);

  /* recreate the cairo surface */
  surface = cairo_xlib_surface_create (XW.display, XW.pixmap, XW.visual, XW.width, XW.height);
  context = cairo_create (surface);
}

/**
 * Initialises the normalised device coords matrix
 */
void
_giza_init_norm_xw (void)
{
  cairo_matrix_init (&(Win.normCoords),(double) Dev.width, 0, 0, (double) -Dev.height, GIZA_XW_MARGIN,
		     Dev.height + GIZA_XW_MARGIN);
}

/**
 * This routine closes the X window device and cleans up associated memory.
 *
 * If prompting is turned on this will not occur until a key is pressed
 * or a mouse is clicked inside the X window.
 */
void
_giza_close_device_xw (void)
{
  cairo_surface_finish (surface);
  cairo_status_t status = cairo_surface_status (surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_close_device_xw",cairo_status_to_string(status));

  cairo_surface_destroy (surface);
  XFreePixmap (XW.display, XW.pixmap);
  /*XFreeGC(XW.display,XW.gc);*/
  XCloseDisplay (XW.display);
}

static int _giza_errors_xw (Display *display, XErrorEvent *xwerror)
{
  char text[82];
  int length = (int) sizeof(text);
  XGetErrorText(display,xwerror->error_code,text,length);
  _giza_error("giza_xw",text);
  return 0;
}


/**
 * Loops indefinitely, redrawing and resizing the window as necessary until a key is pressed.
 * Returns the x and y position of the cursor (in device coords) and the key pressed.
 */
static void
_giza_xevent_loop (int mode, int moveCurs, int anchorx, int anchory, int *x, int *y, char *ch)
{
  /* move the cursor to the given position */
  if (moveCurs)
    {
      XWarpPointer (XW.display, None, XW.window, 0, 0, 0, 0, anchorx, anchory);
    }

  XEvent event;
  XSelectInput (XW.display, XW.window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | PointerMotionMask);

  _giza_init_band (mode);
  while (1)  {

    /* wait for key press/expose */
    XNextEvent(XW.display, &event);

    /* always return x, y values for safety */
    *x = 0;
    *y = 0;

    switch  (event.type) {
    case ClientMessage: /* catch close window event */
      *ch = 'q';
      if (!strcmp( XGetAtomName( XW.display, event.xclient.message_type ), "WM_PROTOCOLS" )) {
         return;
      }
    case Expose: /* redraw */
      _giza_expose_xw (&event);
      break;
    case KeyPress: /* return pos and char */
      {
	char buffer[10];
	KeySym key;
	int nret;

	/* get the ascii of the pressed key */
	nret = XLookupString((XKeyEvent *)&event, buffer,
			     (int) (sizeof(buffer)/sizeof(char)), &key, NULL);

	/* ignore irrelevant keys */
	if(nret==1 && (key < XK_Shift_L || key > XK_Hyper_R)) {
	  *x = event.xkey.x ;/*- GIZA_XW_MARGIN; */
	  *y = event.xkey.y ;/*- GIZA_XW_MARGIN; */

	  if(key)
	    *ch = buffer[0];

          _giza_destroy_band (mode);
          _giza_flush_xw_event_queue(&event);
	  return;
	};

	break;
      }
    case ConfigureNotify: /* check if the window has been resized */
      if(event.xconfigure.width != XW.width || event.xconfigure.height != XW.height)
	_giza_change_size_xw (event.xconfigure.width, event.xconfigure.height);
      break;
    case ButtonPress:
      {
        *x = event.xbutton.x ;/*- GIZA_XW_MARGIN; */
	*y = event.xbutton.y ;/*- GIZA_XW_MARGIN; */
        switch(event.xbutton.button) {
        case Button1:
           *ch = GIZA_LEFT_CLICK;
           break;
        case Button2:
           *ch = GIZA_RIGHT_CLICK;
           break;        
        case Button3:
           *ch = GIZA_MIDDLE_CLICK;
           break;        
        case 4: /* use integers in case ButtonN not defined for N>6 */
           *ch = GIZA_SCROLL_UP;
           break;        
        case 5:
           *ch = GIZA_SCROLL_DOWN;
           break;        
        case 6:
           *ch = GIZA_SCROLL_LEFT;
           break;        
        case 7:
           *ch = GIZA_SCROLL_RIGHT;
           break;        
        default:
           *ch = GIZA_OTHER_CLICK;
           break;
        }
        _giza_destroy_band (mode);
        _giza_flush_xw_event_queue(&event);
	return;
      }
    case MotionNotify:
      {
        /* discard all except the last pointer motion event */
        while(XCheckWindowEvent(XW.display, XW.window,
                              (long)(PointerMotionMask), &event) == True);

        _giza_refresh_band (mode, anchorx, anchory, event.xmotion.x, event.xmotion.y);
        _giza_flush_xw_event_queue(&event);
      }
    default:
      break;
    }

  }

/*
 * Note: we never get to here (return is after ButtonPress)
  _giza_destroy_band (mode);
*/
}

/**
 * expunge the Xwindow event queue
 */

static void
_giza_flush_xw_event_queue (XEvent *event)
{
  /*
   * Discard un-handled ButtonPress, KeyPress and MotionNotify events
   * without blocking.
   */
  while(XCheckWindowEvent(XW.display, XW.window,
       (long) (ButtonPressMask | KeyPressMask | PointerMotionMask), event));


  /* Flush all remaining events from the X event queue
  while (XPending(XW.display)) {
     printf("removing pending XW event \n");
     XNextEvent(XW.display, event);
  }
  */
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
  /* Set the new device size */
  Dev.width  = width  - 2 * GIZA_XW_MARGIN;
  Dev.height = height - 2 * GIZA_XW_MARGIN;

  XW.width  = width;
  XW.height = height;

  cairo_xlib_surface_set_size(surface,width,height);
}

/**
 * Expands clipping so the whole surface can be drawn to.
 */
void
_giza_expand_clipping_xw (void)
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

  int ix,iy;
  int ixanc = (int) xanc, iyanc = (int) yanc;
  _giza_xevent_loop (mode, moveCurs, ixanc, iyanc, &ix, &iy, ch);

  *x = (double) ix;
  *y = (double) iy;

  cairo_device_to_user (context, x, y);
  _giza_set_trans (oldTrans);
}

/**
 * Creates the surfaces for drawing the band
 */
int
_giza_init_band_xw (void)
{
      /* Set up box so it can draw the box... */
      Band.onscreen = cairo_xlib_surface_create (XW.display, XW.window, XW.visual, XW.width, XW.height);
      Band.box = cairo_create (Band.onscreen);

      /* use grey for band */
      cairo_set_source_rgba (Band.box, 0.5, 0.5, 0.5, 1.0);

      /* Set up restore to remove box */
      Band.restore = cairo_create (Band.onscreen);
      cairo_set_source_surface (Band.restore, surface, 0, 0);
      Band.maxHeight = XW.height;
      Band.maxWidth = XW.width;
      return 1;
}

#endif
