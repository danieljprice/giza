#include "giza-features.h"
#include "giza-driver-xw-private.h"

#ifdef _OPENGL_
#ifndef GIZA_GL_PRIVATE_H
#define GIZA_GL_PRIVATE_H
#include "giza-gl.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct
{
  // The translation of the camera
  GLfloat camPos[3];
  // The rotation of the camera
  GLfloat camRot[16];
  // The inverse rotation of the camera
  GLfloat camRotInv[16];
  // The projection matrix
  GLfloat projection[16];
  // The context (if drawing to an xwindow)
  GLXContext *context;
  // The x visual infomation
  XVisualInfo *vinfo;
  // Remember if interactive mode is active
  int interactive;
} giza_gl_settings_t;

giza_gl_settings_t gl_sets;


#endif
#endif
