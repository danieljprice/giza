#include <X11/Xlib.h>

int main () 
{

  Display *display;
  display = XOpenDisplay(NULL);

  XCloseDisplay(display);
  return 1;
}
