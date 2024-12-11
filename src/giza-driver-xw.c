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
  int in_use;
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

static Atom wmDeleteMessage;
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
 *  -7 :- Internal error: the current giza device (Dev[id])'s
 *        entry in the giza-to-xw-mapping was not freed up correctly
 */
int
_giza_open_device_xw (double width, double height, int units)
{
  /* GIZA_XWindow contains pointers so much be initialized properly */
  static int didInit = 0;
  if( !didInit ) {
      /* All device structs get zeroed out */
      memset( &XW[0], 0x0, GIZA_MAX_DEVICES * sizeof(struct GIZA_XWindow) );
      didInit = 1;
  }

  /* From now on we store Dev[id]'s XWindow specific stuff in the XW[id]
   * entry. Make sure that entry is marked unused */
  if( XW[id].in_use ) {
      _giza_error ("_giza_open_device_xw", "Internal error: XW[%d] is still marked as in-use", id);
      return 7;
  }

  /* Start with a clean slate for that device and mark it as being in use*/
  memset( &XW[id], 0x0, sizeof(struct GIZA_XWindow) );
  XW[id].in_use  = 1;

  /* open the connection to the sever and check there was no error */
  XW[id].display = XOpenDisplay (NULL);
  if (!XW[id].display)
    {
      _giza_error ("_giza_open_device_xw", "Cannot launch X window (use ssh -Y if remote; install XQuartz if on Mac)");
      return 1;
    }

  /* get an identifier for the screen */
  XW[id].screenptr = DefaultScreenOfDisplay(XW[id].display);
  XW[id].screennum = DefaultScreen (XW[id].display);

  /* create a visual */
  XW[id].visual = DefaultVisual (XW[id].display, XW[id].screennum);

  if (!XW[id].visual)
    {
      _giza_error ("_giza_open_device_xw", "Could not get X visual");
      return 3;
    }


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

  /* set the XLib stuff */
  XW[id].width = Dev[id].width + 2 * GIZA_XW_MARGIN;
  XW[id].height = Dev[id].height + 2 * GIZA_XW_MARGIN;

  /* set the depth */
  XW[id].depth = DefaultDepth(XW[id].display,XW[id].screennum);

  /* Debugging info */

  /*if (giza_xw_debug)
     {
       XSynchronize(XW[id].display, True);
       printf("(giza_xw_debug) XW display: %s\n",XDisplayName((char*)XW[id].display));
       printf("(giza_xw_debug) XW monitor resolution: %d x %d\n",
                              DisplayWidth(XW[id].display,XW[id].screennum),
                             DisplayHeight(XW[id].display,XW[id].screennum));
    }
  */
  if (XW[id].depth == 1)
    {
       _giza_error("_giza_open_device_xw","XW depth = 1: no colour possible");
    }
  else
    {
       /*printf("giza_xw_debug: XW colour depth = %d\n",XW[id].depth);*/
    }


  unsigned long white = WhitePixel(XW[id].display, XW[id].screennum);

  /* create the window */
  XW[id].window = XCreateSimpleWindow (XW[id].display,
                               DefaultRootWindow (XW[id].display),/* make our new window a child of the entire display */
                               50, 50,       /* origin */
                               XW[id].width, XW[id].height, /* size */
                               0, 0, white);
  if (!XW[id].window)
    {
      _giza_error ("_giza_open_device", "Could not create X window");
      return 4;
    }

  XStoreName (XW[id].display, XW[id].window, Dev[id].prefix);
  XSelectInput(XW[id].display, XW[id].window, StructureNotifyMask);
  XMapWindow (XW[id].display, XW[id].window);

   /* register interest in the delete window message */
  wmDeleteMessage = XInternAtom(XW[id].display, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(XW[id].display, XW[id].window, &wmDeleteMessage, 1);

  /* register the routine to handle non-fatal X errors */
  XSetErrorHandler( _giza_errors_xw );

  /* create the pixmap */
  XW[id].pixmap = XCreatePixmap (XW[id].display, XW[id].window, (unsigned) XW[id].width, (unsigned) XW[id].height, (unsigned) XW[id].depth);

  /* Create graphics context */
/* XW[id].gc = XDefaultGCOfScreen(DefaultScreenOfDisplay(XW[id].display)); */
  /*XW[id].gc = XCreateGC (XW[id].display, XW[id].pixmap, 0, 0);*/
  /* version below works on older X11 distros */
  XW[id].gc = XDefaultGC (XW[id].display, XW[id].screennum);

  /* create Xlib surface in cairo */
  Dev[id].surface = cairo_xlib_surface_create (XW[id].display, XW[id].pixmap, XW[id].visual, XW[id].width, XW[id].height);
  if (!Dev[id].surface)
    {
      _giza_error ("_giza_open_device_xw", "Could not create surface");
      return 5;
    }

  Dev[id].defaultBackgroundAlpha = 1.;

  /* Wait for the MapNotify event */
  for(;;) {
      XEvent e;
      XNextEvent(XW[id].display, &e);
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
  XCopyArea (XW[id].display, XW[id].pixmap, XW[id].window, XW[id].gc, 0, 0, (unsigned) XW[id].width, (unsigned) XW[id].height, 0, 0);

  if (!XFlush (XW[id].display))
    {
      _giza_warning ("_giza_flush_device_xw", "Could not flush X window");
    }

}

/**
 * Advances the X window device to the next page.
 * If resize was set upon function entry, resize the window accordingly.
 * If, otoh, we detect the window was resized, take appropriate action and
 * mark the device as resized.
 */
void
_giza_change_page_xw (void)
{
  int          x_return, y_return;
  Window       root_return;
  unsigned int width_return, height_return, border_width_return, depth_return;

  /* Enquire current geometry to see if it's changed */
  XGetGeometry(XW[id].display, XW[id].window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);
  /* interactive logging feature */
  if (Sets.autolog && Dev[id].drawn) _giza_write_log_file(Dev[id].surface);

  if (Dev[id].resize) {
     /* Set the new device size */
     XW[id].width  = Dev[id].width + 2 * GIZA_XW_MARGIN;
     XW[id].height = Dev[id].height + 2 * GIZA_XW_MARGIN;

     /* Request window to be resized */
     XResizeWindow(XW[id].display, XW[id].window, (unsigned) XW[id].width, (unsigned) XW[id].height);
  } else if( (unsigned int)XW[id].width!=width_return || (unsigned int)XW[id].height!=height_return ) {
      /* Oh. Someone probably resized the XWindow behind our backs. Handle that here */
      XW[id].width  = Dev[id].width  = width_return;
      XW[id].height = Dev[id].height = height_return;

      /* take care of margin - if there's room for that */
      if( Dev[id].width > 2*GIZA_XW_MARGIN )
          Dev[id].width -= 2*GIZA_XW_MARGIN;
      if( Dev[id].height > 2*GIZA_XW_MARGIN )
          Dev[id].height -= 2*GIZA_XW_MARGIN;
      /* Do stuff needed to reflect changed window size, notably adjust panel size in case of resized surface*/
      _giza_init_norm();
      Dev[id].panelwidth  = Dev[id].width  / Dev[id].nx;
      Dev[id].panelheight = Dev[id].height / Dev[id].ny;
  }

  /* This function is called for each new page, so new page means new pixmap */
  cairo_destroy(Dev[id].context);
  cairo_surface_finish (Dev[id].surface);
  cairo_status_t status = cairo_surface_status (Dev[id].surface);
  if (status != CAIRO_STATUS_SUCCESS)
     _giza_error("giza_change_page_xw",cairo_status_to_string(status));

  cairo_surface_destroy (Dev[id].surface);
  XFreePixmap (XW[id].display, XW[id].pixmap);

  /* New page means new pixmap */
  XW[id].pixmap = XCreatePixmap (XW[id].display, XW[id].window, (unsigned) XW[id].width, (unsigned) XW[id].height, (unsigned) XW[id].depth);

  /* New pixmap means new cairo surface */
  Dev[id].surface = cairo_xlib_surface_create (XW[id].display, XW[id].pixmap, XW[id].visual, XW[id].width, XW[id].height);
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
 * This routine closes the X window device and cleans up associated memory.
 */
void
_giza_close_device_xw (void)
{
  if( Dev[id].surface ) {
      cairo_surface_finish (Dev[id].surface);
      cairo_status_t status = cairo_surface_status (Dev[id].surface);
      if (status != CAIRO_STATUS_SUCCESS)
         _giza_error("giza_close_device_xw",cairo_status_to_string(status));
      cairo_surface_destroy (Dev[id].surface);
  }
  /* Release resources only if they seem to be there */
  if( XW[id].display ) {
      if( XW[id].window )
          XDestroyWindow( XW[id].display, XW[id].window );
      if( XW[id].pixmap )
          XFreePixmap (XW[id].display, XW[id].pixmap);
      /*XFreeGC(XW[id].display,XW[id].gc);*/
      XCloseDisplay (XW[id].display);
  }
  /* Erase state of current XW device to pristine state */
  memset( &XW[id], 0x0, sizeof(struct GIZA_XWindow) );
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
 * Loops indefinitely, redrawing the window as necessary until a key is pressed.
 * Returns the x and y position of the cursor (in device coords) and the key pressed.
 *
 * Update: do not /actively/ honour resizing of the window (neither does
 *         PGPLOT) whilst waiting for key press.
 */
static void
_giza_xevent_loop (int mode, int moveCurs, int nanc, const int *anchorx, const int *anchory, int *x, int *y, char *ch)
{
  /* move the cursor to the given position */
  if (moveCurs)
    {
      XWarpPointer (XW[id].display, None, XW[id].window, 0, 0, 0, 0, anchorx[nanc-1], anchory[nanc-1]);
    }

  XEvent event;
  XSelectInput (XW[id].display, XW[id].window, ExposureMask | KeyPressMask | ButtonPressMask | PointerMotionMask | StructureNotifyMask );

  _giza_init_band (mode);
  _giza_expand_clipping_xw();

 while(1) {

    /* wait for key press/expose (avoid using XNextEvent as breaks older systems) */
    XWindowEvent(XW[id].display, XW[id].window,
       (long) (ExposureMask | KeyPressMask | ButtonPressMask | PointerMotionMask | StructureNotifyMask), &event);
    /*XNextEvent(XW[id].display, &event);*/

    /* always return x, y values for safety */
    *x = 0;
    *y = 0;
    switch  (event.type) {
    case ClientMessage: /* catch close window event */
      *ch = 'q';
       if ((Atom)event.xclient.data.l[0] == wmDeleteMessage) {
          return;
       }
       break;
    case DestroyNotify:
      *ch = 'q';
      return;
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
          _giza_reset_clipping_xw();
         return;
       };

       break;
      }
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
        _giza_reset_clipping_xw();
        return;
      }
    case MotionNotify:
      {
        /* discard all except the last pointer motion event */
        while(XCheckWindowEvent(XW[id].display, XW[id].window,
                              (long)(PointerMotionMask), &event) == True);

        /* if a callback function is set to do things while the cursor is moving, call it */
        if (Dev[id].motion_callback != NULL) {
           double xpt = (double) event.xmotion.x;
           double ypt = (double) event.xmotion.y;
           /* make sure the transform is to world coords, because arbitrary drawing
              can happen in the callback routine, which may change the transform */
           _giza_set_trans (GIZA_TRANS_WORLD);
           cairo_device_to_user (Dev[id].context, &xpt, &ypt);
           Dev[id].motion_callback(&xpt, &ypt, &mode);
        }

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
  while(XCheckWindowEvent(XW[id].display, XW[id].window,
       (long) (ButtonPressMask | KeyPressMask | PointerMotionMask), event));


  /* Flush all remaining events from the X event queue
  while (XPending(XW[id].display)) {
     printf("removing pending XW event \n");
     XNextEvent(XW[id].display, event);
  }
  */
}

/**
 * Deal with the xwindow expose event.
 */
static void
_giza_expose_xw (XEvent *event)
{
  XCopyArea (XW[id].display, XW[id].pixmap, XW[id].window, XW[id].gc, event->xexpose.x,
            event->xexpose.y, (unsigned) event->xexpose.width,
            (unsigned) event->xexpose.height, event->xexpose.x,
            event->xexpose.y);

/*  XFlush(XW[id].display); */
}

/**
 * Expands clipping so the whole surface can be drawn to.
 */
void
_giza_expand_clipping_xw (void)
{
  _giza_set_trans (GIZA_TRANS_IDEN);
  cairo_reset_clip (Dev[id].context);
  cairo_rectangle (Dev[id].context, 0, 0, XW[id].width, XW[id].height);
  cairo_clip (Dev[id].context);
}

/**
 * Restores clipping of the plotting surface
 */
void
_giza_reset_clipping_xw (void)
{
  /* Restore clipping */
  giza_set_viewport (Dev[id].VP.xmin, Dev[id].VP.xmax, Dev[id].VP.ymin, Dev[id].VP.ymax);
  _giza_set_trans (GIZA_TRANS_WORLD);

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
  Band.onscreen = cairo_xlib_surface_create (XW[id].display, XW[id].window, XW[id].visual, XW[id].width, XW[id].height);
  Band.box = cairo_create (Band.onscreen);

  /* use grey for band */
  cairo_set_source_rgba (Band.box, 0.5, 0.5, 0.5, 1.0);

  /* Set up restore to remove box (DP: 2/12/24 we now restore from pixmap instead of window to fix issue on Mac OS)*/
  Band.onscreen = cairo_xlib_surface_create (XW[id].display, XW[id].pixmap, XW[id].visual, XW[id].width, XW[id].height);
  Band.restore = cairo_create (Band.onscreen);

  cairo_set_source_surface (Band.restore, Dev[id].surface, 0, 0);
  Band.maxHeight = XW[id].height;
  Band.maxWidth = XW[id].width;
  return 1;
}

#endif
