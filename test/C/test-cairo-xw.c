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
#include <cairo/cairo-xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

struct XWstuff
{
  Display *display;
  Pixmap pixmap;
  Window window;
  Screen *screenptr;
  int screennum;
  Visual *visual;

  int width;
  int height;
  int depth;
} XW;

cairo_t *context;
cairo_surface_t *surface;

void
test_openclose_cairo_xw (void)
{
  /* basic Xwindow initialisation */
  XW.width = 500;
  XW.height = 200;
  XW.display = XOpenDisplay (NULL);
  XW.screennum = DefaultScreen (XW.display);
  XW.depth = DefaultDepth(XW.display,XW.screennum);
  XW.visual = DefaultVisual (XW.display, XW.screennum);
  XW.window = XCreateSimpleWindow (XW.display,
				   DefaultRootWindow (XW.display),
				   50, 50,	/* origin */
				   XW.width, XW.height, /* size */
				   0, 0, 0);
  XMapWindow (XW.display, XW.window);
  
  /* now cairo stuff */
  surface = cairo_xlib_surface_create (XW.display, XW.window, XW.visual, XW.width, XW.height);
  context = cairo_create (surface);
  cairo_set_source_surface(context,surface,0,0);
  cairo_paint (context);
  cairo_destroy(context);
  cairo_surface_destroy (surface);

  /* clean up */
  printf("start CloseDisplay\n");
  XCloseDisplay (XW.display);
  printf("  end CloseDisplay\n");
}


int main(void) {

  printf("libcairo v%s \n",CAIRO_VERSION_STRING);
  test_openclose_cairo_xw();
  test_openclose_cairo_xw();
 
  return 0;
}
