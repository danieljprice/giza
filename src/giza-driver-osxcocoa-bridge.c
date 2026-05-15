/* giza-driver-osxcocoa-bridge.c
 *
 * giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 * Copyright (c) 2026      Shugo Suwazono
 *
 * macOS/Quartz driver bridge (plain C)
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING file for details).
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 *
 * This file is plain C and handles all giza internals (Dev[], id, cairo).
 * It calls ObjC bridge functions declared in giza-driver-osxcocoa-private.h
 * which are implemented in giza-driver-osxcocoa.m.
 *
 * The split exists because giza-private.h defines "extern int id" which
 * clashes with ObjC's "id" type — so .m and giza-private.h must not meet.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __APPLE__
/* Do NOT include cairo-quartz.h here — it pulls in ApplicationServices.h
 * which uses Blocks syntax (^) incompatible with plain C on macOS 26 SDK.
 * Instead, forward-declare only what we need. */
#ifdef _GIZA_HAS_OSXCOCOA

/* Use cairo image surface to avoid CGLayer/CGContext compatibility issues.
 * Pixels are read back and displayed via CGImage in drawRect. */
#include <cairo/cairo.h>
#include "giza-private.h"
#include "giza-window-private.h"
#include "giza-io-private.h"
#include "giza-drivers-private.h"
#include <stdio.h>
#include "giza-driver-osxcocoa-private.h"
#include "giza-transforms-private.h"

#include <giza.h>
#include <stdlib.h>
#include <string.h>

/* ======================================================================== */
/* Constants                                                                 */
/* ======================================================================== */

#define GIZA_OSXCOCOA_DEFAULT_WIDTH    800
#define GIZA_OSXCOCOA_DEFAULT_HEIGHT   600
#define GIZA_OSXCOCOA_UNITS_PER_MM    3.7054
#define GIZA_OSXCOCOA_UNITS_PER_PIXEL 1.0
#define GIZA_OSXCOCOA_MARGIN          20

/* ======================================================================== */
/* Internal: create cairo-quartz surface from CGContext                      */
/* ======================================================================== */

static int
_osxcocoa_create_surface (GizaCGContextRef cgctx, int width, int height)
{
    (void)cgctx;  /* not used with image surface approach */
    Dev[id].surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                  width, height);
    if (!Dev[id].surface ||
        cairo_surface_status(Dev[id].surface) != CAIRO_STATUS_SUCCESS) {
        _giza_error("giza-driver-osxcocoa", "could not create cairo image surface");
        Dev[id].surface = NULL;
        return 1;
    }
    return 0;
}

/* ======================================================================== */
/* Driver entry points                                                        */
/* ======================================================================== */

int
_giza_open_device_osxcocoa (double width, double height, int units)
{
    int wpx, hpx;
    if (width > 1.0 && height > 1.0)
        _giza_get_specified_size(width, height, units, &wpx, &hpx);
    else {
        wpx = GIZA_OSXCOCOA_DEFAULT_WIDTH;
        hpx = GIZA_OSXCOCOA_DEFAULT_HEIGHT;
    }

    /* total window size includes margin */
    int wwin = wpx + 2 * GIZA_OSXCOCOA_MARGIN;
    int hwin = hpx + 2 * GIZA_OSXCOCOA_MARGIN;

    _giza_osxcocoa_start_app_if_needed();

    GizaCGContextRef cgctx = _giza_osxcocoa_open_window(id, wwin, hwin);
    if (!cgctx) {
        _giza_error("_giza_open_device_osxcocoa", "could not open NSWindow");
        return 1;
    }

    if (_osxcocoa_create_surface(cgctx, wwin, hwin) != 0) {
        _giza_osxcocoa_close_window(id);
        return 1;
    }


    Dev[id].width               = wwin;
    Dev[id].height              = hwin;
    Dev[id].deviceUnitsPermm    = GIZA_OSXCOCOA_UNITS_PER_MM;
    Dev[id].deviceUnitsPerPixel = GIZA_OSXCOCOA_UNITS_PER_PIXEL;
    Dev[id].isInteractive       = 1;
    Dev[id].defaultBackgroundAlpha = 1.0;

    return 0;
}

/* ---------------------------------------------------------------------- */

void
_giza_flush_device_osxcocoa (void)
{
    if (!Dev[id].surface) return;
    cairo_surface_flush(Dev[id].surface);
    unsigned char *data   = cairo_image_surface_get_data(Dev[id].surface);
    int            width  = cairo_image_surface_get_width(Dev[id].surface);
    int            height = cairo_image_surface_get_height(Dev[id].surface);
    int            stride = cairo_image_surface_get_stride(Dev[id].surface);
    _giza_osxcocoa_flush_pixels(id, data, width, height, stride);
}

/* ---------------------------------------------------------------------- */

void
_giza_change_page_osxcocoa (void)
{
    int wwin = Dev[id].width;
    int hwin = Dev[id].height;

    /* destroy old cairo objects */
    if (Dev[id].context) {
        cairo_destroy(Dev[id].context);
        Dev[id].context = NULL;
    }
    if (Dev[id].surface) {
        cairo_surface_finish(Dev[id].surface);
        cairo_surface_destroy(Dev[id].surface);
        Dev[id].surface = NULL;
    }

    /* clear backing layer and get new CGContext */
    GizaCGContextRef cgctx = _giza_osxcocoa_clear_and_get_context(id, wwin, hwin);
    if (!cgctx) return;

    if (_osxcocoa_create_surface(cgctx, wwin, hwin) != 0) return;
    Dev[id].context = cairo_create(Dev[id].surface);
}

/* ---------------------------------------------------------------------- */

void
_giza_close_device_osxcocoa (void)
{
    _giza_flush_device_osxcocoa();
    /* surface+context freed by _giza_close_device_unchecked() */
    _giza_osxcocoa_close_window(id);
}

/* ---------------------------------------------------------------------- */

void
_giza_init_norm_osxcocoa (void)
{
    cairo_matrix_init(&(Dev[id].Win.normCoords),
                      (double)  Dev[id].width,  0.0,
                      0.0, (double) -Dev[id].height,
                      (double)  GIZA_OSXCOCOA_MARGIN,
                      (double) (Dev[id].height - GIZA_OSXCOCOA_MARGIN));
    _giza_set_trans(GIZA_TRANS_NORM);
}

/* ---------------------------------------------------------------------- */

void
_giza_expand_clipping_osxcocoa (void)
{
    _giza_set_trans(GIZA_TRANS_IDEN);
    cairo_reset_clip(Dev[id].context);
    cairo_rectangle(Dev[id].context, 0, 0, Dev[id].width, Dev[id].height);
    cairo_clip(Dev[id].context);
}

/* ---------------------------------------------------------------------- */

void
_giza_get_key_press_osxcocoa (int mode, int moveCurs,
                          int nanc,
                          const double *xanc, const double *yanc,
                          double *x, double *y, char *ch)
{
    (void)mode; (void)moveCurs; (void)nanc; (void)xanc; (void)yanc;

    int oldTrans = _giza_get_trans();
    _giza_set_trans(GIZA_TRANS_WORLD);

    float fx, fy;
    _giza_osxcocoa_wait_for_event(id, &fx, &fy, ch);

    *x = (double)fx;
    *y = (double)fy;
    cairo_device_to_user(Dev[id].context, x, y);

    _giza_set_trans(oldTrans);
}

/* ---------------------------------------------------------------------- */

int
_giza_init_band_osxcocoa (void)
{
    return 0;   /* rubber-band not yet implemented */
}

/* ---------------------------------------------------------------------- */

/*
 * _giza_osxcocoa_handle_resize — 表示スケーリング方式では surface を再生成しない。
 * setFrameSize: は drawRect でのスケーリングに任せる。
 * この関数は参照が残っている場合のために stub として保持。
 */
void
_giza_osxcocoa_handle_resize (int devId, int width, int height)
{
    (void)devId; (void)width; (void)height;
    /* no-op: display scaling is handled in GizaView -drawRect: */
}

#endif /* _GIZA_HAS_OSXCOCOA */
#endif /* __APPLE__ */
