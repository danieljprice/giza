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
#include "giza-features.h"

#ifdef _OPENGL_
#ifndef GIZA_GL_INTERACTIVE_H
#define GIZA_GL_INTERACTIVE_H

#include <GL/gl.h>
#include <GL/glx.h>

void giza_gl_init_interactive (void);
void giza_gl_end_interactive (void);
#endif
#endif
