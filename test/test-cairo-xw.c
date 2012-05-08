#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <X11/Xutil.h>

struct XWstuff
{
  Display *display;
  Pixmap pixmap;
  Window window;
  Screen *screenptr;
  int screennum;
  Visual *visual;
  GC gc;
  Colormap colormap;

  int width;
  int height;
  int depth;
} XW;

cairo_t *context;
cairo_surface_t *surface;

void
test_openclose_cairo_xw (void)
{
  printf("open\n");
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
  
  /* clean up */
  if (context) cairo_destroy(context);
  cairo_surface_destroy (surface);
  printf("close\n");
  XCloseDisplay (XW.display);
  printf("closeXW\n");
}


int main(void) {

  test_openclose_cairo_xw();
  test_openclose_cairo_xw();
 
  return 0;
}
