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
} XW[GIZA_MAX_DEVICES];

#define GIZA_DEFAULT_WIDTH 800
#define GIZA_DEFAULT_HEIGHT 600
#define GIZA_DEVICE_UNITS_PER_MM 3.7054 /* so "width" is 8.5 inches as in ps driver */
#define GIZA_DEVICE_UNITS_PER_PIXEL 1.0 /* device units are pixels */
#define GIZA_DEVICE_INTERACTIVE 1
#define GIZA_XW_MARGIN 20

static void _giza_xevent_loop (int mode, int moveCurs, int nanc, const int *anchorx, const int *anchory, int *x, int *y, char *ch);
static void _giza_expose_xw (XEvent *event);
static void _giza_flush_xw_event_queue (XEvent *event);
static int _giza_errors_xw (Display *display, XErrorEvent *error);

static int giza_xw_id[GIZA_MAX_DEVICES];
static int xid = -1;
/*static int giza_xw_debug = 0;*/

/**
 * Opens an XWindow device for drawing to.
 *
 * Return values:
 *  -0 :- no error.
 *  -1 :- No connection to the X server could be made.
 *  -3 :- No X visual could be created.
 *  -4 :- No window could be created.
 *  -5 :- No cairo surface could be created.
 */
int
_giza_open_device_xw (double width, double height, int units)
{
  Dev[id].deviceUnitsPermm    = GIZA_DEVICE_UNITS_PER_MM;
  Dev[id].deviceUnitsPerPixel = GIZA_DEVICE_UNITS_PER_PIXEL;
  Dev[id].isInteractive       = GIZA_DEVICE_INTERACTIVE;

  /* set all device specific settings */
  if (width > 0. && height > 0. && units > 0)
    {
      _giza_get_specified_size(width,height,units,&Dev[id].width, &Dev[id].height);
    }
  else
    {
      Dev[id].width = GIZA_DEFAULT_WIDTH;
      Dev[id].height = GIZA_DEFAULT_HEIGHT;
    }

  xid += 1;
  giza_xw_id[id] = xid + 1;
  
  /* set the XLib stuff */
  XW[xid].width = Dev[id].width + 2 * GIZA_XW_MARGIN;
  XW[xid].height = Dev[id].height + 2 * GIZA_XW_MARGIN;

  /* open the connection to the sever and check there was no error */
  XW[xid].display = XOpenDisplay (NULL);
  if (!XW[xid].display)
    {
      _giza_error ("_giza_open_device_xw", "Connection to the X server could not be made");
      return 1;
    }

  /* get an identifier for the screen */
  XW[xid].screenptr = DefaultScreenOfDisplay(XW[xid].display);
  XW[xid].screennum = DefaultScreen (XW[xid].display);

  /* set the depth */
  XW[xid].depth = DefaultDepth(XW[xid].display,XW[xid].screennum);

  /* Debugging info */

  /*if (giza_xw_debug)
     {
       XSynchronize(XW[xid].display, True);
       printf("(giza_xw_debug) XW display: %s\n",XDisplayName((char*)XW[xid].display));
       printf("(giza_xw_debug) XW monitor resolution: %d x %d\n",
                              DisplayWidth(XW[xid].display,XW[xid].screennum),
                             DisplayHeight(XW[xid].display,XW[xid].screennum));
    }
  */
  if (XW[xid].depth == 1)
    {
       _giza_error("_giza_open_device_xw","XW depth = 1: no colour possible");
    }
  else
    {
       /*printf("giza_xw_debug: XW colour depth = %d\n",XW[xid].depth);*/
    }

  /* create a visual */
  XW[xid].visual = DefaultVisual (XW[xid].display, XW[xid].screennum);

  if (!XW[xid].visual)
    {
      _giza_error ("_giza_open_device_xw", "Could not get X visual");
      return 3;
    }

  unsigned long white = WhitePixel(XW[xid].display, XW[xid].screennum);

  /* create the window */
  XW[xid].window = XCreateSimpleWindow (XW[xid].display,
				   DefaultRootWindow (XW[xid].display),/* make our new window a child of the entire display */
				   50, 50,	/* origin */
				   XW[xid].width, XW[xid].height, /* size */
				   0, 0, white);
  if (!XW[xid].window)
    {
      _giza_error ("_giza_open_device", "Could not create X window");
      return 4;
    }

  XStoreName (XW[xid].display, XW[xid].window, Dev[id].prefix);
  XSelectInput(XW[xid].display, XW[xid].window, StructureNotifyMask);
  XMapWindow (XW[xid].display, XW[xid].window);

   /* register interest in the delete window message */
  Atom wmDeleteMessage = XInternAtom(XW[xid].display, "WM_DELETE_WINDOW", 1);
  XSetWMProtocols(XW[xid].display, XW[xid].window, &wmDeleteMessage, 1);

  /* register the routine to handle non-fatal X errors */
  XSetErrorHandler( _giza_errors_xw );

  /* create the pixmap */
  XW[xid].pixmap = XCreatePixmap (XW[xid].display, XW[xid].window, (unsigned) XW[xid].width, (unsigned) XW[xid].height, (unsigned) XW[xid].depth);

  /* Create graphics context */
/* XW[xid].gc = XDefaultGCOfScreen(DefaultScreenOfDisplay(XW[xid].display)); */
  /*XW[xid].gc = XCreateGC (XW[xid].display, XW[xid].pixmap, 0, 0);*/
  /* version below works on older X11 distros */
  XW[xid].gc = XDefaultGC (XW[xid].display, XW[xid].screennum);

  /* create Xlib surface in cairo */
  Dev[id].surface = cairo_xlib_surface_create (XW[xid].display, XW[xid].pixmap, XW[xid].visual, XW[xid].width, XW[xid].height);
  /* uncomment below for drawing direct to window
   Dev[id].surface = cairo_xlib_surface_create (XW[xid].display, XW[xid].window, XW[xid].visual, XW[xid].width, XW[xid].height);
   */
  if (!Dev[id].surface)
    {
      _giza_error ("_giza_open_device_xw", "Could not create surface");
      XFreePixmap (XW[xid].display, XW[xid].pixmap);
      return 5;
    }

  Dev[id].defaultBackgroundAlpha = 1.;

  /* Wait for the MapNotify event */

  for(;;) {
      XEvent e;
      XNextEvent(XW[xid].display, &e);
      if (e.type == MapNotify)
	  break;
  }

  return 0;
}

/**
 * Flushes the X device.
 */
void
_giza_flush_device_xw (void)
{
  /* flush the offscreen surface */
  cairo_surface_flush (Dev[id].surface);

  /* move the offscreen surface to the onscreen one */
  XCopyArea (XW[xid].display, XW[xid].pixmap, XW[xid].window, XW[xid].gc, 0, 0, (unsigned) XW[xid].width, (unsigned) XW[xid].height, 0, 0);

  if (!XFlush (XW[xid].display))
    {
      _giza_warning ("_giza_flush_device_xw", "Could not flush X window");
    }

}

/**
 * Advances the X window device to the next page.
 */
void
_giza_change_page_xw (void)
{
  /* interactive logging feature */
  if (Sets.autolog && Dev[id].drawn) _giza_write_log_file(Dev[id].surface);
  
  /* create a new pixmap */
  cairo_destroy(Dev[id].context);
  cairo_surface_finish (Dev[id].surface);
  cairo_status_t status = cairo_surface_status (Dev[id].surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_change_page_xw",cairo_status_to_string(status));

  cairo_surface_destroy (Dev[id].surface);
  XFreePixmap (XW[xid].display, XW[xid].pixmap);

/*  int resize = 0;*/
  if (Dev[id].resize) {
     /* Set the new device size */
     XW[xid].width  = Dev[id].width + 2 * GIZA_XW_MARGIN;
     XW[xid].height = Dev[id].height + 2 * GIZA_XW_MARGIN;

     XResizeWindow(XW[xid].display, XW[xid].window,(unsigned) XW[xid].width,(unsigned) XW[xid].height);  
  }
  
  XW[xid].pixmap = XCreatePixmap (XW[xid].display, XW[xid].window, (unsigned) XW[xid].width, (unsigned) XW[xid].height, (unsigned) XW[xid].depth);

  /* recreate the cairo surface */
  Dev[id].surface = cairo_xlib_surface_create (XW[xid].display, XW[xid].pixmap, XW[xid].visual, XW[xid].width, XW[xid].height);
  Dev[id].context = cairo_create (Dev[id].surface);
}

/**
 * Initialises the normalised device coords matrix
 */
void
_giza_init_norm_xw (void)
{
  cairo_matrix_init (&(Dev[id].Win.normCoords),(double) Dev[id].width, 0, 0, (double) -Dev[id].height,
                     GIZA_XW_MARGIN, Dev[id].height + GIZA_XW_MARGIN);
}

/**
 * Selects which X-Window the device id refers to
 */
int
_giza_select_xw(int devid)
{
  int tmp = giza_xw_id[devid];
  if (!tmp) {
     _giza_error("giza_select_xw","internal error: selected device is not an X-Window");
     return 1;
  } else {
    xid = tmp-1;
    return 0;
  }
}

/**
 * This routine closes the X window device and cleans up associated memory.
 */
void
_giza_close_device_xw (void)
{
  cairo_surface_finish (Dev[id].surface);
  cairo_status_t status = cairo_surface_status (Dev[id].surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_close_device_xw",cairo_status_to_string(status));

  cairo_surface_destroy (Dev[id].surface);
  XFreePixmap (XW[xid].display, XW[xid].pixmap);
  /*XFreeGC(XW[xid].display,XW[xid].gc);*/
  XCloseDisplay (XW[xid].display);
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
_giza_xevent_loop (int mode, int moveCurs, int nanc, const int *anchorx, const int *anchory, int *x, int *y, char *ch)
{
  /* move the cursor to the given position */
  if (moveCurs)
    {
      XWarpPointer (XW[xid].display, None, XW[xid].window, 0, 0, 0, 0, anchorx[nanc-1], anchory[nanc-1]);
    }

  XEvent event;
  XSelectInput (XW[xid].display, XW[xid].window, ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | PointerMotionMask);

  _giza_init_band (mode);

 while(1) {

    /* wait for key press/expose (avoid using XNextEvent as breaks older systems) */
    XWindowEvent(XW[xid].display, XW[xid].window,
       (long) (ExposureMask | KeyPressMask | StructureNotifyMask | ButtonPressMask | PointerMotionMask), &event);
    /*XNextEvent(XW[xid].display, &event);*/

    /* always return x, y values for safety */
    *x = 0;
    *y = 0;

    switch  (event.type) {
    case ClientMessage: /* catch close window event */
      *ch = 'q';
      if (!strcmp( XGetAtomName( XW[xid].display, event.xclient.message_type ), "WM_PROTOCOLS" )) {
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
      if(event.xconfigure.width != XW[xid].width || event.xconfigure.height != XW[xid].height)
	_giza_change_size_xw (event.xconfigure.width, event.xconfigure.height);
      break;
    case ButtonPress:
      {
        *x = event.xbutton.x ;/*- GIZA_XW_MARGIN; */
	*y = event.xbutton.y ;/*- GIZA_XW_MARGIN; */
        switch(event.xbutton.button) {
        case Button1:
           if (event.xbutton.state==1) {
             *ch = GIZA_SHIFT_CLICK;
           } else {
             *ch = GIZA_LEFT_CLICK;
           }
           break;
        case Button2:
           *ch = GIZA_MIDDLE_CLICK;
           break;        
        case Button3:
           *ch = GIZA_RIGHT_CLICK;
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
        while(XCheckWindowEvent(XW[xid].display, XW[xid].window,
                              (long)(PointerMotionMask), &event) == True);

        _giza_refresh_band (mode, nanc, anchorx, anchory, event.xmotion.x, event.xmotion.y);
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
  while(XCheckWindowEvent(XW[xid].display, XW[xid].window,
       (long) (ButtonPressMask | KeyPressMask | PointerMotionMask), event));


  /* Flush all remaining events from the X event queue
  while (XPending(XW[xid].display)) {
     printf("removing pending XW event \n");
     XNextEvent(XW[xid].display, event);
  }
  */
}

/**
 * Deal with the xwindow expose event.
 */
static void
_giza_expose_xw (XEvent *event)
{
  XCopyArea (XW[xid].display, XW[xid].pixmap, XW[xid].window, XW[xid].gc, event->xexpose.x,
	     event->xexpose.y, (unsigned) event->xexpose.width,
	     (unsigned) event->xexpose.height, event->xexpose.x,
	     event->xexpose.y);

/*  XFlush(XW[xid].display); */
}

/**
 * Changes the size of the current window
 */
void
_giza_change_size_xw (int width, int height)
{
  /* Set the new device size */
/*
  Dev[id].width  = width  - 2 * GIZA_XW_MARGIN;
  Dev[id].height = height - 2 * GIZA_XW_MARGIN;

  XW[xid].width  = width;
  XW[xid].height = height;
  
  XResizeWindow(XW[xid].display, XW[xid].window,(unsigned) XW[xid].width,(unsigned) XW[xid].height);

  cairo_xlib_surface_set_size(Dev[id].surface,width,height);
*/
}

/**
 * Expands clipping so the whole surface can be drawn to.
 */
void
_giza_expand_clipping_xw (void)
{
  _giza_set_trans (GIZA_TRANS_IDEN);
  cairo_reset_clip (Dev[id].context);
  cairo_rectangle (Dev[id].context, 0, 0, XW[xid].width, XW[xid].height);
  cairo_clip (Dev[id].context);
}

/**
 * Loops until a key is pressed. At this point the position in world coords of the cursor is returned, along with the key pressed.
 */
void
_giza_get_key_press_xw (int mode, int moveCurs, int nanc, const double *xanc, const double *yanc, 
                        double *x, double *y, char *ch)
{
  int oldTrans = _giza_get_trans ();
  _giza_set_trans (GIZA_TRANS_WORLD);
  double xanci,yanci;

  int i,ix,iy;
  int ixanc[nanc];
  int iyanc[nanc];
  for (i = 0; i < nanc; i++) {
      xanci = xanc[i];
      yanci = yanc[i];
      cairo_user_to_device(Dev[id].context, &xanci, &yanci);
      ixanc[i] = (int) xanci;
      iyanc[i] = (int) yanci;
  }
  _giza_xevent_loop (mode, moveCurs, nanc, ixanc, iyanc, &ix, &iy, ch);

  *x = (double) ix;
  *y = (double) iy;

  cairo_device_to_user (Dev[id].context, x, y);
  _giza_set_trans (oldTrans);
}

/**
 * Creates the surfaces for drawing the band
 */
int
_giza_init_band_xw (void)
{
  /* Set up box so it can draw the box... */
  Band.onscreen = cairo_xlib_surface_create (XW[xid].display, XW[xid].window, XW[xid].visual, XW[xid].width, XW[xid].height);
  Band.box = cairo_create (Band.onscreen);

  /* use grey for band */
  cairo_set_source_rgba (Band.box, 0.5, 0.5, 0.5, 1.0);

  /* Set up restore to remove box */
  Band.restore = cairo_create (Band.onscreen);
  cairo_set_source_surface (Band.restore, Dev[id].surface, 0, 0);
  Band.maxHeight = XW[xid].height;
  Band.maxWidth = XW[xid].width;
  return 1;
}

#endif
