#include "giza-gl-private.h"

#ifdef _OPENGL_
#include "giza-gl.h"
#include "giza-drivers-private.h"
#include "giza-driver-xw-private.h"
#include "giza-private.h"
#include "giza.h"
#include "giza-io-private.h"

/**
 * GL: giza_gl_init_interactive
 *
 * Synopsis: Initialises the interactive opengl rendering mode.
 *           The current giza device must be an XWindow.
 */
void
giza_gl_init_interactive (void)
{
  // Visual attributes
  static int attributes[] =
  {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 8,
  GLX_GREEN_SIZE, 8,
  GLX_BLUE_SIZE, 8,
  GLX_DEPTH_SIZE, 16,
  0};

  if (Dev.type != GIZA_DEVICE_XW)
    {
      _giza_error ("giza_gl_init_interactive",
		   "Current giza device is not an X Window. Interactive OpenGL mode not initialised.");
      return;
    }

  // Get the visual info
  gl_sets.vinfo = glxChooseVisual (XW.display, DefaultScreen (XW.display), attributes);
}

void
giza_gl_end_interactive (void)
{
}
#endif
