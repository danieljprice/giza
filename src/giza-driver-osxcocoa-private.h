/* giza-driver-osxcocoa-private.h
 *
 * giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2026      Shugo Suwazono
 *
 * macOS native window driver — shared C/ObjC interface declarations
 * bridging giza-driver-osxcocoa-bridge.c (plain C) and giza-driver-osxcocoa.m (ObjC).
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING file for details).
 *
 * This header is included by both:
 *   giza-driver-osxcocoa.m      (ObjC: implements _giza_osxcocoa_*  functions)
 *   giza-driver-osxcocoa-bridge.c (C: implements _giza_open/close/flush etc.)
 *   giza-drivers.c          (C: dispatch)
 */
#ifndef GIZA_DRIVER_OSXCOCOA_PRIVATE_H
#define GIZA_DRIVER_OSXCOCOA_PRIVATE_H

#ifdef __APPLE__
#define _GIZA_HAS_OSXCOCOA 1

/* CGContextRef is opaque here to avoid pulling in CoreGraphics headers
 * (which use Blocks syntax incompatible with plain C compiler on macOS 26) */
#ifdef __OBJC__
/* In ObjC context CoreGraphics is already included via Cocoa */
typedef CGContextRef GizaCGContextRef;
#else
/* In plain C context avoid pulling in CoreGraphics (Blocks syntax issue) */
typedef void* GizaCGContextRef;
#endif

/* ---- giza driver entry points (implemented in giza-driver-osxcocoa-bridge.c) - */
int  _giza_open_device_osxcocoa     (double width, double height, int units);
void _giza_flush_device_osxcocoa    (void);
void _giza_change_page_osxcocoa     (void);
void _giza_close_device_osxcocoa    (void);
void _giza_init_norm_osxcocoa       (void);
void _giza_expand_clipping_osxcocoa (void);
void _giza_get_key_press_osxcocoa   (int mode, int moveCurs,
                                 int nanc,
                                 const double *xanc, const double *yanc,
                                 double *x, double *y, char *ch);
int  _giza_init_band_osxcocoa       (void);

/* ---- ObjC bridge functions (implemented in giza-driver-osxcocoa.m) ----------- */
void         _giza_osxcocoa_start_app_if_needed (void);
void         _giza_osxcocoa_mark_app_ready       (void);
GizaCGContextRef _giza_osxcocoa_open_window          (int devId, int width, int height);
void         _giza_osxcocoa_flush               (int devId);
void         _giza_osxcocoa_flush_pixels         (int devId,
                                             unsigned char *data,
                                             int width, int height,
                                             int stride);
GizaCGContextRef _giza_osxcocoa_clear_and_get_context(int devId, int width, int height);
void         _giza_osxcocoa_resize_window        (int devId, int width, int height);
void         _giza_osxcocoa_close_window         (int devId);
void         _giza_osxcocoa_wait_for_event       (int devId,
                                             float *x, float *y, char *ch);
void         _giza_osxcocoa_wait_for_event_band  (int devId, int mode,
                                             float xancSurface, float yancSurface,
                                             float *x, float *y, char *ch);

/* ---- resize callback (implemented in giza-driver-osxcocoa-bridge.c) ---------- */
/* Called by GizaView -setFrameSize: */
void _giza_osxcocoa_handle_resize (int devId, int width, int height);

#endif /* __APPLE__ */
#endif /* GIZA_DRIVER_OSXCOCOA_PRIVATE_H */
