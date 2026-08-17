/* giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING.LESSER file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2 of the LGPLv3 states that:
 *
 *  a) The work must carry prominent notices stating that you modified
 *  it, and giving a relevant date.
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the LGPL for specific language governing rights and limitations.
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
#include <X11/cursorfont.h>

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
  /* Set when XResizeWindow is issued; cleared once window geometry matches.
   * Stops prepare_draw from treating async resize lag as a user resize. */
  int resize_pending;
  /* User resized the window (ConfigureNotify); replot when mouse button released */
  int resize_dirty;
  /* Cached window background from last successful XAllocColor (ci 0) */
  int bg_cached;
  double bg_r, bg_g, bg_b;
  unsigned long bg_pixel;
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
static void _xw_sync_window_background (void);
static Bool _xw_event_pending (Display *dpy, XEvent *ev, XPointer arg);

static Atom wmDeleteMessage;
static Atom wmProtocols;
/*static int giza_xw_debug = 0;*/

/**
 * XIfEvent predicate: events for our window that the interactive loop handles.
 * ClientMessage (WM_DELETE_WINDOW) has no event-mask bit, so XWindowEvent
 * never returns it — that is why the close button appeared dead on macOS/XQuartz.
 */
static Bool
_xw_event_pending (Display *dpy, XEvent *ev, XPointer arg)
{
  Window w = *(Window *) arg;

  (void) dpy;
  if (ev->xany.window != w)
    return False;

  switch (ev->type)
    {
    case ClientMessage:
    case DestroyNotify:
    case Expose:
    case ConfigureNotify:
    case ButtonPress:
    case ButtonRelease:
    case KeyPress:
    case MotionNotify:
      return True;
    default:
      return False;
    }
}

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
  wmDeleteMessage = XInternAtom(XW[id].display, "WM_DELETE_WINDOW", False);
  wmProtocols = XInternAtom(XW[id].display, "WM_PROTOCOLS", False);
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
 * Keep the X window background pixel in sync with colour index 0.
 * Newly exposed regions (Hollywood expand, drag-resize) then match the
 * plot background instead of the default white pixel used at create time.
 */
static void
_xw_sync_window_background (void)
{
  XColor xc;
  double r, g, b, a;

  if (!XW[id].display || !XW[id].window)
    return;

  giza_get_colour_representation_alpha (0, &r, &g, &b, &a);

  if (XW[id].bg_cached
      && XW[id].bg_r == r && XW[id].bg_g == g && XW[id].bg_b == b)
    {
      XSetWindowBackground (XW[id].display, XW[id].window, XW[id].bg_pixel);
      return;
    }

  xc.red   = (unsigned short) (r * 65535.0 + 0.5);
  xc.green = (unsigned short) (g * 65535.0 + 0.5);
  xc.blue  = (unsigned short) (b * 65535.0 + 0.5);
  xc.flags = DoRed | DoGreen | DoBlue;

  if (!XW[id].colormap)
    XW[id].colormap = DefaultColormap (XW[id].display, XW[id].screennum);

  if (!XAllocColor (XW[id].display, XW[id].colormap, &xc))
    return;

  XW[id].bg_cached = 1;
  XW[id].bg_r = r;
  XW[id].bg_g = g;
  XW[id].bg_b = b;
  XW[id].bg_pixel = xc.pixel;

  XSetWindowBackground (XW[id].display, XW[id].window, xc.pixel);
}

/**
 * Flushes the X device.
 */
void
_giza_flush_device_xw (void)
{
  /* flush the offscreen surface */
  cairo_surface_flush (Dev[id].surface);

  /* ensure expand/Expose margins match colour index 0 */
  _xw_sync_window_background ();

  /* move the offscreen surface to the onscreen one */
  XCopyArea (XW[id].display, XW[id].pixmap, XW[id].window, XW[id].gc, 0, 0, (unsigned) XW[id].width, (unsigned) XW[id].height, 0, 0);

  if (!XFlush (XW[id].display))
    {
      _giza_warning ("_giza_flush_device_xw", "Could not flush X window");
    }

}

/**
 * Query the current X window outer size in pixels.
 */
static void
_xw_query_window_size (unsigned int *wwin, unsigned int *hwin)
{
  int          x_return, y_return;
  Window       root_return;
  unsigned int border_width_return, depth_return;

  XGetGeometry (XW[id].display, XW[id].window, &root_return,
                &x_return, &y_return, wwin, hwin,
                &border_width_return, &depth_return);
}

/**
 * Update Dev[id] plot area from full window dimensions (margins excluded).
 */
static void
_xw_sync_device_to_window (unsigned int wwin, unsigned int hwin)
{
  XW[id].width  = wwin;
  XW[id].height = hwin;
  Dev[id].width  = (int) wwin - 2 * GIZA_XW_MARGIN;
  Dev[id].height = (int) hwin - 2 * GIZA_XW_MARGIN;
  if (Dev[id].width < 1)
    Dev[id].width = 1;
  if (Dev[id].height < 1)
    Dev[id].height = 1;

  /* adjust panel size for resized surface */
  _giza_init_norm_xw ();
  Dev[id].panelwidth  = Dev[id].width  / Dev[id].nx;
  Dev[id].panelheight = Dev[id].height / Dev[id].ny;
}

/**
 * Destroy and recreate the X pixmap and cairo surface at the current size.
 */
static void
_xw_recreate_surface (void)
{
  if (!Dev[id].surface)
    return;

  /* create new pixmap and cairo objects before destroying the old ones */
  Pixmap new_pixmap = XCreatePixmap (XW[id].display, XW[id].window,
                                     (unsigned) XW[id].width,
                                     (unsigned) XW[id].height,
                                     (unsigned) XW[id].depth);
  cairo_surface_t *new_surf = cairo_xlib_surface_create (XW[id].display, new_pixmap,
                                                         XW[id].visual,
                                                         XW[id].width, XW[id].height);
  if (!new_surf || cairo_surface_status (new_surf) != CAIRO_STATUS_SUCCESS) {
    _giza_error ("_xw_recreate_surface", "could not create cairo xlib surface");
    XFreePixmap (XW[id].display, new_pixmap);
    return;
  }

  cairo_t *new_ctx = cairo_create (new_surf);
  if (!new_ctx || cairo_status (new_ctx) != CAIRO_STATUS_SUCCESS) {
    _giza_error ("_xw_recreate_surface", "could not create cairo context");
    if (new_ctx)
      cairo_destroy (new_ctx);
    cairo_surface_destroy (new_surf);
    XFreePixmap (XW[id].display, new_pixmap);
    return;
  }

  /* This function is called for each new page, so new page means new pixmap */
  cairo_destroy (Dev[id].context);
  cairo_surface_finish (Dev[id].surface);
  cairo_status_t status = cairo_surface_status (Dev[id].surface);
  if (status != CAIRO_STATUS_SUCCESS)
    _giza_error ("_xw_recreate_surface", cairo_status_to_string (status));

  cairo_surface_destroy (Dev[id].surface);
  XFreePixmap (XW[id].display, XW[id].pixmap);

  XW[id].pixmap = new_pixmap;
  Dev[id].surface = new_surf;
  Dev[id].context = new_ctx;

  /*
   * XCreatePixmap leaves undefined contents (often white). Fill with the
   * plot background immediately so Expose/XCopyArea never blit a white
   * rectangle that is later "chased" by giza_draw_background.
   */
  giza_draw_background ();
}

/**
 * Apply a reported window size: sync Dev, recreate surface (fills background).
 */
static void
_xw_apply_window_size (unsigned int wwin, unsigned int hwin)
{
  _xw_sync_device_to_window (wwin, hwin);
  _xw_recreate_surface ();
  XW[id].resize_pending = 0;
}

/**
 * Non-blocking drain of ConfigureNotify; returns 1 if at least one was seen.
 * On success, wwin and hwin hold the last reported size.
 */
static int
_xw_drain_configure_notify (unsigned int *wwin, unsigned int *hwin)
{
  XEvent e;
  int saw = 0;

  while (XCheckTypedWindowEvent (XW[id].display, XW[id].window,
                                 ConfigureNotify, &e))
    {
      *wwin = (unsigned int) e.xconfigure.width;
      *hwin = (unsigned int) e.xconfigure.height;
      saw = 1;
    }
  return saw;
}

/**
 * Sync X window size to Dev[id] and recreate the cairo surface if needed.
 *
 * Called via _giza_prepare_interactive_draw before paper-size queries and
 * viewport setup (e.g. after the user resizes the window interactively).
 */
void
_giza_prepare_draw_xw (void)
{
  unsigned int width_return, height_return;

  _xw_query_window_size (&width_return, &height_return);

  /* already in sync — nothing to do */
  if ((unsigned int) XW[id].width == width_return
      && (unsigned int) XW[id].height == height_return)
    {
      XW[id].resize_pending = 0;
      return;
    }

  /* Programmatic paper-size change: XResizeWindow is asynchronous. Keep the
   * requested size until ConfigureNotify (or matching geometry) arrives;
   * otherwise prepare_draw would revert Dev and wipe the new background. */
  if (Dev[id].resize || XW[id].resize_pending)
    {
      unsigned int cfg_w = width_return, cfg_h = height_return;

      if (_xw_drain_configure_notify (&cfg_w, &cfg_h))
        {
          /* WM answered (possibly with a constrained size) — adopt it */
          _xw_query_window_size (&cfg_w, &cfg_h);
          _xw_apply_window_size (cfg_w, cfg_h);
          return;
        }
      /* Still lagging behind our XResizeWindow request */
      return;
    }

  _xw_apply_window_size (width_return, height_return);
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
  unsigned int width_return, height_return;

  /* Enquire current geometry to see if it's changed */
  _xw_query_window_size (&width_return, &height_return);
  /* interactive logging feature */
  if (Sets.autolog && Dev[id].drawn) _giza_write_log_file(Dev[id].surface);

  if (Dev[id].resize) {
     /* Set the new device size */
     XW[id].width  = Dev[id].width + 2 * GIZA_XW_MARGIN;
     XW[id].height = Dev[id].height + 2 * GIZA_XW_MARGIN;

     /* Match window bg before resize so newly exposed margins are not white */
     _xw_sync_window_background ();

     /* Request window to be resized (async; see resize_pending in prepare_draw) */
     XResizeWindow(XW[id].display, XW[id].window, (unsigned) XW[id].width, (unsigned) XW[id].height);
     XW[id].resize_pending = 1;
  } else if( (unsigned int)XW[id].width!=width_return || (unsigned int)XW[id].height!=height_return ) {
      /* Oh. Someone probably resized the XWindow behind our backs. Handle that here */
      _xw_sync_device_to_window (width_return, height_return);
  }

  _xw_recreate_surface ();

  /*
   * Push the blank (background-coloured) page to the window now so that
   * an animated XResizeWindow expand shows black/white margins matching
   * ci 0, not an uninitialised or stale white frame.
   */
  if (Dev[id].resize || XW[id].resize_pending)
    {
      cairo_surface_flush (Dev[id].surface);
      XCopyArea (XW[id].display, XW[id].pixmap, XW[id].window, XW[id].gc,
                 0, 0, (unsigned) XW[id].width, (unsigned) XW[id].height, 0, 0);
      XFlush (XW[id].display);
    }
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
 * Update: do not recreate the cairo pixmap whilst waiting for key press
 *         (neither does PGPLOT) — Expose keeps copying the old pixel map so
 *         the previous plot stays visible during a drag-resize.  When a motion
 *         callback is set, return 'r' after the user releases the
 *         mouse so the application can replot at the final size.
 */
static void
_giza_xevent_loop (int mode, int moveCurs, int nanc, const int *anchorx, const int *anchory, int *x, int *y, char *ch)
{
  long event_mask = ExposureMask | KeyPressMask | ButtonPressMask
                  | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

  /* move the cursor to the given position */
  if (moveCurs)
    {
      XWarpPointer (XW[id].display, None, XW[id].window, 0, 0, 0, 0, anchorx[nanc-1], anchory[nanc-1]);
    }

  XEvent event;
  XSelectInput (XW[id].display, XW[id].window, event_mask);

  _giza_init_band (mode);
  _giza_expand_clipping_xw();

  /* show a live crosshair cursor while waiting for input, as PGPLOT does */
  Cursor livecursor = XCreateFontCursor (XW[id].display, XC_crosshair);
  XDefineCursor (XW[id].display, XW[id].window, livecursor);

 while(1) {

    /*
     * Wait for the next interactive event.  Prefer XIfEvent over XWindowEvent:
     * the latter cannot return ClientMessage (no mask bit), so WM_DELETE_WINDOW
     * from the close button never woke the loop.
     */
    XIfEvent (XW[id].display, &event, _xw_event_pending, (XPointer) &XW[id].window);

    /* always return x, y values for safety */
    *x = 0;
    *y = 0;
    switch  (event.type) {
    case ClientMessage: /* red close-box / WM_DELETE_WINDOW → same as 'q' */
       if (event.xclient.message_type == wmProtocols
           && event.xclient.format == 32
           && (Atom) event.xclient.data.l[0] == wmDeleteMessage) {
          *ch = 'q';
          _giza_destroy_band (mode);
          _giza_flush_xw_event_queue (&event);
          _giza_reset_clipping_xw ();
          XUndefineCursor (XW[id].display, XW[id].window);
          XFreeCursor (XW[id].display, livecursor);
          XFlush (XW[id].display);
          return;
       }
       break;
    case DestroyNotify:
      *ch = 'q';
      _giza_destroy_band (mode);
      _giza_reset_clipping_xw ();
      XFreeCursor (XW[id].display, livecursor);
      return;
    case Expose: /* redraw */
      _giza_expose_xw (&event);
      break;
    case ConfigureNotify:
      {
        unsigned int cfg_w, cfg_h;
        int old_w = XW[id].width;
        int old_h = XW[id].height;

        cfg_w = (unsigned int) event.xconfigure.width;
        cfg_h = (unsigned int) event.xconfigure.height;
        _xw_drain_configure_notify (&cfg_w, &cfg_h);
        _xw_query_window_size (&cfg_w, &cfg_h);

        /* Echo of our own XResizeWindow (already matching) — no replot */
        if ((int) cfg_w == old_w && (int) cfg_h == old_h)
          {
            XW[id].resize_pending = 0;
            break;
          }

        /*
         * Programmatic / pending paper-size change: adopt final geometry later
         * via prepare_draw; never schedule an automatic 'r' from this path.
         */
        if (XW[id].resize_pending || Dev[id].resize)
          break;

        /*
         * User drag-resize: do not recreate/clear the cairo pixmap here.
         * Expose keeps blitting the old pixel map until the mouse is released;
         * then return 'r' (if a motion callback is set) so the client can
         * replot and prepare_draw syncs to the final size.
         */
        if (Dev[id].motion_callback != NULL)
          XW[id].resize_dirty = 1;
        break;
      }
    case ButtonRelease:
      /* End of resize drag: button up after ConfigureNotify */
      if (XW[id].resize_dirty && Dev[id].motion_callback != NULL)
        {
          XW[id].resize_dirty = 0;
          *x = event.xbutton.x;
          *y = event.xbutton.y;
          *ch = 'r';
          _giza_destroy_band (mode);
          _giza_flush_xw_event_queue (&event);
          _giza_reset_clipping_xw ();
          XUndefineCursor (XW[id].display, XW[id].window);
          XFreeCursor (XW[id].display, livecursor);
          XFlush (XW[id].display);
          return;
        }
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

          XW[id].resize_dirty = 0;
          _giza_destroy_band (mode);
          _giza_flush_xw_event_queue(&event);
          _giza_reset_clipping_xw();
          XUndefineCursor (XW[id].display, XW[id].window);
          XFreeCursor (XW[id].display, livecursor);
          XFlush (XW[id].display);
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
        XW[id].resize_dirty = 0;
        _giza_destroy_band (mode);
        _giza_flush_xw_event_queue(&event);
        _giza_reset_clipping_xw();
        XUndefineCursor (XW[id].display, XW[id].window);
        XFreeCursor (XW[id].display, livecursor);
        XFlush (XW[id].display);
        return;
      }
    case MotionNotify:
      {
        /* discard all except the last pointer motion event */
        while(XCheckWindowEvent(XW[id].display, XW[id].window,
                              (long)(PointerMotionMask), &event) == True);

        /*
         * After a WM resize, ButtonRelease often never reaches the client.
         * Motion with no buttons held means the user has let go — replot.
         */
        if (XW[id].resize_dirty && Dev[id].motion_callback != NULL
            && !(event.xmotion.state & (Button1Mask | Button2Mask | Button3Mask)))
          {
            XW[id].resize_dirty = 0;
            *x = event.xmotion.x;
            *y = event.xmotion.y;
            *ch = 'r';
            _giza_destroy_band (mode);
            _giza_flush_xw_event_queue (&event);
            _giza_reset_clipping_xw ();
            XUndefineCursor (XW[id].display, XW[id].window);
            XFreeCursor (XW[id].display, livecursor);
            XFlush (XW[id].display);
            return;
          }

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
        break;
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
