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
 * (LGPL, see COPYING.LESSER file for details).
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
#include "giza-band-private.h"

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
/* Internal: cairo image surface helpers                                   */
/* ======================================================================== */

/**
 * Full window size (plot area plus margins), matching XW[id].width/height.
 */
static void
_osxcocoa_surface_size (int *wwin, int *hwin)
{
    *wwin = Dev[id].width  + 2 * GIZA_OSXCOCOA_MARGIN;
    *hwin = Dev[id].height + 2 * GIZA_OSXCOCOA_MARGIN;
}

/**
 * Allocate a new cairo image surface for the plot buffer.
 *
 * Return value: 0 on success, 1 on failure.
 */
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

/**
 * If the user resized the NSWindow, update Dev[id] plot size to match /xw.
 *
 * Return value: 1 if Dev[id] was updated, 0 if already in sync.
 */
static int
_osxcocoa_sync_device_to_view (void)
{
    int viewW, viewH;
    int surfW = 0, surfH = 0;

    _giza_osxcocoa_get_view_size(id, &viewW, &viewH);
    if (viewW <= 0 || viewH <= 0)
        return 0;

    /* Compare NSWindow size to the cairo buffer (full window incl. margins) */
    if (Dev[id].surface) {
        surfW = cairo_image_surface_get_width(Dev[id].surface);
        surfH = cairo_image_surface_get_height(Dev[id].surface);
    } else {
        _osxcocoa_surface_size(&surfW, &surfH);
    }

    /* minimum surface fits plot area (1px) plus both margins */
    {
        int minSurf = 2 * GIZA_OSXCOCOA_MARGIN + 1;
        int expectedSurfW = viewW < minSurf ? minSurf : viewW;
        int expectedSurfH = viewH < minSurf ? minSurf : viewH;

        if (surfW == expectedSurfW && surfH == expectedSurfH)
            return 0;
    }

    Dev[id].width  = viewW - 2 * GIZA_OSXCOCOA_MARGIN;
    Dev[id].height = viewH - 2 * GIZA_OSXCOCOA_MARGIN;
    if (Dev[id].width < 1)
        Dev[id].width = 1;
    if (Dev[id].height < 1)
        Dev[id].height = 1;

    /* adjust panel size for resized surface */
    _giza_init_norm_osxcocoa();
    Dev[id].panelwidth  = Dev[id].width  / Dev[id].nx;
    Dev[id].panelheight = Dev[id].height / Dev[id].ny;
    return 1;
}

/**
 * Destroy and recreate the cairo image surface at the current plot size.
 */
static void
_osxcocoa_recreate_surface (void)
{
    int wwin, hwin;
    cairo_surface_t *old_surf = Dev[id].surface;
    cairo_t         *old_ctx  = Dev[id].context;

    _osxcocoa_surface_size(&wwin, &hwin);

    /* resize backing layer first; keep old cairo objects if this fails */
    GizaCGContextRef cgctx = _giza_osxcocoa_clear_and_get_context(id, wwin, hwin);
    if (!cgctx) return;

    cairo_surface_t *new_surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                           wwin, hwin);
    if (!new_surf ||
        cairo_surface_status(new_surf) != CAIRO_STATUS_SUCCESS) {
        _giza_error("giza-driver-osxcocoa", "could not create cairo image surface");
        if (new_surf)
            cairo_surface_destroy(new_surf);
        return;
    }

    cairo_t *new_ctx = cairo_create(new_surf);
    if (!new_ctx || cairo_status(new_ctx) != CAIRO_STATUS_SUCCESS) {
        _giza_error("giza-driver-osxcocoa", "could not create cairo context");
        if (new_ctx)
            cairo_destroy(new_ctx);
        cairo_surface_destroy(new_surf);
        return;
    }

    /* destroy old cairo objects */
    if (old_ctx)
        cairo_destroy(old_ctx);
    if (old_surf) {
        cairo_surface_finish(old_surf);
        cairo_surface_destroy(old_surf);
    }

    Dev[id].surface = new_surf;
    Dev[id].context = new_ctx;
}

/* ======================================================================== */
/* Driver entry points                                                        */
/* ======================================================================== */

int
_giza_open_device_osxcocoa (double width, double height, int units)
{
    int wwin, hwin;

    Dev[id].deviceUnitsPermm    = GIZA_OSXCOCOA_UNITS_PER_MM;
    Dev[id].deviceUnitsPerPixel = GIZA_OSXCOCOA_UNITS_PER_PIXEL;
    Dev[id].isInteractive       = 1;
    Dev[id].defaultBackgroundAlpha = 1.0;

    /* Match /xw: Dev[id].width/height is the plot area; surface adds margins */
    if (width > 0. && height > 0. && units > 0)
        _giza_get_specified_size(width, height, units, &Dev[id].width, &Dev[id].height);
    else {
        Dev[id].width  = GIZA_OSXCOCOA_DEFAULT_WIDTH;
        Dev[id].height = GIZA_OSXCOCOA_DEFAULT_HEIGHT;
    }

    _osxcocoa_surface_size(&wwin, &hwin);

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

    return 0;
}

/* ---------------------------------------------------------------------- */

/**
 * Sync NSWindow size to Dev[id] and recreate the cairo surface if needed.
 *
 * Called via _giza_prepare_interactive_draw from giza_get_paper_size and
 * giza_set_viewport (before splash queries aspect ratio in setpage2).
 */
void
_giza_osxcocoa_prepare_draw (void)
{
    if (_osxcocoa_sync_device_to_view())
        _osxcocoa_recreate_surface();
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
    if (Dev[id].resize)
        _giza_osxcocoa_resize_window(id,
                                   Dev[id].width  + 2 * GIZA_OSXCOCOA_MARGIN,
                                   Dev[id].height + 2 * GIZA_OSXCOCOA_MARGIN);
    else
        /* user may have resized the NSWindow behind our backs */
        _osxcocoa_sync_device_to_view();

    /* new page means new cairo surface (like /xw pixmap recreation) */
    _osxcocoa_recreate_surface();
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
    /* Same layout as _giza_init_norm_xw: norm coords map to the plot area */
    cairo_matrix_init(&(Dev[id].Win.normCoords),
                      (double) Dev[id].width, 0.0,
                      0.0, (double) -Dev[id].height,
                      (double) GIZA_OSXCOCOA_MARGIN,
                      (double) (Dev[id].height + GIZA_OSXCOCOA_MARGIN));
    _giza_set_trans(GIZA_TRANS_NORM);
}

/* ---------------------------------------------------------------------- */

void
_giza_expand_clipping_osxcocoa (void)
{
    int wwin, hwin;

    _osxcocoa_surface_size(&wwin, &hwin);
    _giza_set_trans(GIZA_TRANS_IDEN);
    cairo_reset_clip(Dev[id].context);
    cairo_rectangle(Dev[id].context, 0, 0, wwin, hwin);
    cairo_clip(Dev[id].context);
}

/* ---------------------------------------------------------------------- */

void
_giza_get_key_press_osxcocoa (int mode, int moveCurs,
                          int nanc,
                          const double *xanc, const double *yanc,
                          double *x, double *y, char *ch)
{
    (void) moveCurs;
    (void) nanc;

    int oldTrans = _giza_get_trans();
    _giza_set_trans(GIZA_TRANS_WORLD);

    float fx, fy;
    /* attach + wait + detach on main thread to avoid deadlock */
    if (mode > 0) {
        /* Convert anchor world coords to device (surface) coords for band overlay */
        double axd = xanc[0], ayd = yanc[0];
        cairo_user_to_device(Dev[id].context, &axd, &ayd);
        _giza_osxcocoa_wait_for_event_band(id, mode, (float)axd, (float)ayd, &fx, &fy, ch);
    } else {
        _giza_osxcocoa_wait_for_event(id, &fx, &fy, ch);
    }

    *x = (double)fx;
    *y = (double)fy;
    cairo_device_to_user(Dev[id].context, x, y);

    _giza_set_trans(oldTrans);
}

/* ---------------------------------------------------------------------- */

int
_giza_init_band_osxcocoa (void)
{
    /* We don't use Cairo Band.box/Band.restore for the OSX driver.
     * Instead GizaBandView draws directly via CoreGraphics.
     * However _giza_init_band() in giza-drivers.c will try to use Band.box
     * after calling us, so we must provide a valid (dummy) cairo context. */
    int wwin, hwin;
    _osxcocoa_surface_size(&wwin, &hwin);
    int w = wwin > 0 ? wwin : 1;
    int h = hwin > 0 ? hwin : 1;
    cairo_surface_t *dummy = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
    if (cairo_surface_status(dummy) != CAIRO_STATUS_SUCCESS) return 0;
    Band.onscreen = dummy;
    Band.box      = cairo_create(dummy);
    Band.restore  = cairo_create(dummy);
    Band.maxWidth  = w;
    Band.maxHeight = h;
    return 1; /* success */
}

#endif /* _GIZA_HAS_OSXCOCOA */
#endif /* __APPLE__ */
