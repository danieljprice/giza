/* giza-driver-osxcocoa.m
 *
 * giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2010      James Wetter and Daniel Price
 * Copyright (c) 2010-2012 Daniel Price
 * Copyright (c) 2026      Shugo Suwazono
 *
 * macOS/Quartz native Cocoa window driver
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

 * ---- Design -------------------------------------------------------------
 *
 * This file (.m) contains ONLY Objective-C / Cocoa code.
 * No giza-private.h (avoids "extern int id" vs ObjC "id" clash).
 * All giza internals are in giza-driver-osxcocoa-bridge.c (plain C).
 *
 * ---- Threading ----------------------------------------------------------
 *
 * When giza_main() runs on a worker thread (via giza-osxcocoa-main.m):
 *   - [NSApp run] runs on the main thread continuously
 *   - _run_on_main() dispatches UI work to main thread via GCD
 *   - No beachball, resize works
 *
 * When called from the main thread (simple programs without giza-osxcocoa-main.m):
 *   - Events are drained manually in each flush
 *   - Beachball when cursor enters window (known limitation)
 */

#ifdef __APPLE__
#include <cairo/cairo-quartz.h>
#ifdef CAIRO_HAS_QUARTZ_SURFACE

#import <Cocoa/Cocoa.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "giza-driver-osxcocoa-private.h"

#ifndef GIZA_LEFT_CLICK
#define GIZA_LEFT_CLICK    'A'
#define GIZA_RIGHT_CLICK   'X'
#define GIZA_MIDDLE_CLICK  'D'
#define GIZA_SHIFT_CLICK   15
#define GIZA_OTHER_CLICK   'X'
#define GIZA_SCROLL_UP     21
#define GIZA_SCROLL_DOWN   4
#define GIZA_SCROLL_LEFT   12
#define GIZA_SCROLL_RIGHT  18
#endif

#define GIZA_OSXCOCOA_TITLE   "giza"
#define GIZA_MAX_DEV     8  /* must match GIZA_MAX_DEVICES in giza-private.h */

/* ======================================================================== */
/* GizaView                                                                  */
/* ======================================================================== */

/* ======================================================================== */
/* GizaBandView — transparent overlay that draws rubber-band shapes         */
/* using CoreGraphics. Lives on top of GizaView; only exists during         */
/* a giza_band() / PGBAND call.                                              */
/* ======================================================================== */

@interface GizaBandView : NSView
{
    int     _mode;       /* GIZA_BAND_* mode */
    NSPoint _anchor;     /* anchor point in surface coords */
    NSPoint _cursor;     /* current cursor in surface coords */
    int     _surfW;      /* source surface size (for full-width lines) */
    int     _surfH;
    CGRect  _displayRect;/* same letterbox rect as parent GizaView */
}
- (instancetype)initWithFrame:(NSRect)frame
                         mode:(int)mode
                       anchor:(NSPoint)anc
                      surfaceSize:(NSSize)ss
                      displayRect:(CGRect)dr;
- (void)updateCursor:(NSPoint)surfPt displayRect:(CGRect)dr;
@end

@implementation GizaBandView

- (instancetype)initWithFrame:(NSRect)frame
                         mode:(int)mode
                       anchor:(NSPoint)anc
                      surfaceSize:(NSSize)ss
                      displayRect:(CGRect)dr
{
    self = [super initWithFrame:frame];
    if (self) {
        _mode = mode;
        _anchor = anc;
        _cursor = anc;
        _surfW  = (int)ss.width;
        _surfH  = (int)ss.height;
        _displayRect = dr;
    }
    return self;
}

- (BOOL)isOpaque { return NO; }
- (BOOL)acceptsFirstResponder { return NO; }  /* events handled by GizaView */

/* Convert cairo surface coords to view coords for drawing.
 * GizaView is not flipped (AppKit origin bottom-left, y up); cairo uses y down. */
- (CGPoint)_viewPtFromSurface:(NSPoint)sp
{
    if (_surfW <= 0 || _surfH <= 0 || _displayRect.size.width < 1)
        return CGPointMake(sp.x, sp.y);
    CGFloat sx = _displayRect.origin.x + sp.x / _surfW * _displayRect.size.width;
    CGFloat sy = _displayRect.origin.y + _displayRect.size.height
               - (sp.y / _surfH * _displayRect.size.height);
    return CGPointMake(sx, sy);
}

- (void)drawRect:(NSRect)dirtyRect
{
    CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
    CGContextClearRect(ctx, NSRectToCGRect(self.bounds));

    if (_mode <= 0) return;

    /* Style: white line with 40% opacity fill for area modes */
    CGContextSetLineWidth(ctx, 1.5);
    CGContextSetRGBStrokeColor(ctx, 0.9, 0.9, 0.9, 1.0);
    CGContextSetRGBFillColor  (ctx, 1.0, 1.0, 1.0, 0.15);

    CGPoint a = [self _viewPtFromSurface:_anchor];
    CGPoint c = [self _viewPtFromSurface:_cursor];

    /* full-width / full-height extent in view coords */
    CGPoint tl = [self _viewPtFromSurface:NSMakePoint(0,0)];
    CGPoint br = [self _viewPtFromSurface:NSMakePoint(_surfW,_surfH)];

    switch (_mode) {
        case 1: /* line anchor → cursor */
            CGContextMoveToPoint(ctx, a.x, a.y);
            CGContextAddLineToPoint(ctx, c.x, c.y);
            CGContextStrokePath(ctx);
            break;
        case 2: /* hollow rectangle */
            {
                CGRect r = CGRectMake(fmin(a.x,c.x), fmin(a.y,c.y),
                                      fabs(c.x-a.x), fabs(c.y-a.y));
                CGContextFillRect(ctx, r);
                CGContextStrokeRect(ctx, r);
            }
            break;
        case 3: /* two horizontal lines */
            CGContextMoveToPoint(ctx, tl.x, a.y);
            CGContextAddLineToPoint(ctx, br.x, a.y);
            CGContextMoveToPoint(ctx, tl.x, c.y);
            CGContextAddLineToPoint(ctx, br.x, c.y);
            CGContextStrokePath(ctx);
            break;
        case 4: /* two vertical lines */
            CGContextMoveToPoint(ctx, a.x, tl.y);
            CGContextAddLineToPoint(ctx, a.x, br.y);
            CGContextMoveToPoint(ctx, c.x, tl.y);
            CGContextAddLineToPoint(ctx, c.x, br.y);
            CGContextStrokePath(ctx);
            break;
        case 5: /* single horizontal line */
            CGContextMoveToPoint(ctx, tl.x, c.y);
            CGContextAddLineToPoint(ctx, br.x, c.y);
            CGContextStrokePath(ctx);
            break;
        case 6: /* single vertical line */
            CGContextMoveToPoint(ctx, c.x, tl.y);
            CGContextAddLineToPoint(ctx, c.x, br.y);
            CGContextStrokePath(ctx);
            break;
        case 7: /* crosshair */
            CGContextMoveToPoint(ctx, tl.x, c.y);
            CGContextAddLineToPoint(ctx, br.x, c.y);
            CGContextMoveToPoint(ctx, c.x, tl.y);
            CGContextAddLineToPoint(ctx, c.x, br.y);
            CGContextStrokePath(ctx);
            break;
        case 8: /* circle centred on anchor */
            {
                CGFloat dx = c.x - a.x, dy = c.y - a.y;
                CGFloat r  = sqrt(dx*dx + dy*dy);
                CGContextBeginPath(ctx);
                CGContextAddArc(ctx, a.x, a.y, r, 0, 2*M_PI, 0);
                CGContextFillPath(ctx);
                CGContextBeginPath(ctx);
                CGContextAddArc(ctx, a.x, a.y, r, 0, 2*M_PI, 0);
                CGContextStrokePath(ctx);
            }
            break;
        default:
            break;
    }
}

- (void)updateCursor:(NSPoint)surfPt displayRect:(CGRect)dr
{
    _cursor = surfPt;
    _displayRect = dr;
    [self setNeedsDisplay:YES];
}

@end

/* ======================================================================== */
/* GizaView                                                                  */
/* ======================================================================== */

@interface GizaView : NSView
{
    CGLayerRef     _layer;
    int            _devId;
    BOOL           _gotEvent;
    NSPoint        _eventPt;
    char           _eventCh;
    unsigned char *_pixelData;
    int            _pixelWidth;   /* cairo surface の元サイズ */
    int            _pixelHeight;
    int            _pixelStride;
    /* リサイズ表示用: drawRect が描画する実際の矩形 (letterbox/pillarbox) */
    CGRect         _displayRect;
    /* rubber-band overlay */
    GizaBandView  *_bandView;
    BOOL           _bandActive;
    NSPoint        _bandMovePt;   /* most recent mouse-moved surface point */
    BOOL           _gotMouseMove;
}
- (instancetype)initWithFrame:(NSRect)frame devId:(int)devId;
- (CGLayerRef)cgLayer;
- (void)rebuildLayerSize:(NSSize)sz;
- (void)waitForInputEvent;
- (void)waitForInputEventWithBand;
- (NSPoint)lastEventPoint;
- (char)lastEventChar;
- (void)setPixelData:(unsigned char*)data width:(int)w height:(int)h stride:(int)s;
/* view 座標 → cairo surface 座標への逆変換 */
- (NSPoint)surfacePointFromViewPoint:(NSPoint)vp;
- (NSPoint)currentMouseViewPoint;
/* band control */
- (void)attachBandViewMode:(int)mode anchorSurfacePt:(NSPoint)anc;
- (void)detachBandView;
@end

@implementation GizaView

- (instancetype)initWithFrame:(NSRect)frame devId:(int)devId
{
    self = [super initWithFrame:frame];
    if (self) {
        _devId = devId; _layer = NULL; _gotEvent = NO; _eventCh = ' ';
        _pixelData = NULL; _pixelWidth = _pixelHeight = _pixelStride = 0;
        _displayRect = CGRectZero;
        _bandView = nil; _bandActive = NO;
        _bandMovePt = NSZeroPoint; _gotMouseMove = NO;
    }
    return self;
}

- (void)dealloc { if (_layer) { CGLayerRelease(_layer); _layer = NULL; } }
- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)isFlipped             { return NO; }
- (CGLayerRef)cgLayer         { return _layer; }

- (void)rebuildLayerSize:(NSSize)sz
{
    if (_layer) { CGLayerRelease(_layer); _layer = NULL; }
    CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
    CGContextRef tmp = CGBitmapContextCreate(NULL,1,1,8,0,cs,
                           (CGBitmapInfo)kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(cs);
    _layer = CGLayerCreateWithContext(tmp, NSSizeToCGSize(sz), NULL);
    CGContextRelease(tmp);
    CGContextRef lctx = CGLayerGetContext(_layer);
    CGContextSetRGBFillColor(lctx,1,1,1,1);
    CGContextFillRect(lctx, CGRectMake(0,0,sz.width,sz.height));
}

/* ---- アスペクト比を維持して view 全体に収まる描画矩形を計算 ----------- */
- (CGRect)_computeDisplayRect
{
    if (_pixelWidth <= 0 || _pixelHeight <= 0) return CGRectZero;
    NSSize vs = self.bounds.size;
    CGFloat sx = vs.width  / (CGFloat)_pixelWidth;
    CGFloat sy = vs.height / (CGFloat)_pixelHeight;
    CGFloat s  = (sx < sy) ? sx : sy;   /* アスペクト比を壊さない最大倍率 */
    CGFloat dw = _pixelWidth  * s;
    CGFloat dh = _pixelHeight * s;
    CGFloat ox = (vs.width  - dw) * 0.5; /* 中央寄せ */
    CGFloat oy = (vs.height - dh) * 0.5;
    return CGRectMake(ox, oy, dw, dh);
}

- (void)drawRect:(NSRect)dirtyRect

{
    CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
    /* 背景を黒で塗りつぶし */
    CGContextSetRGBFillColor(ctx, 0, 0, 0, 1);
    CGContextFillRect(ctx, NSRectToCGRect(self.bounds));

    if (_pixelData && _pixelWidth > 0 && _pixelHeight > 0) {
        CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
        CGDataProviderRef prov = CGDataProviderCreateWithData(NULL, _pixelData,
            (size_t)(_pixelStride * _pixelHeight), NULL);
        CGImageRef img = CGImageCreate((size_t)_pixelWidth,(size_t)_pixelHeight,
            8,32,(size_t)_pixelStride,cs,
            kCGBitmapByteOrder32Host|(CGBitmapInfo)kCGImageAlphaPremultipliedFirst,
            prov,NULL,false,kCGRenderingIntentDefault);
        CGColorSpaceRelease(cs); CGDataProviderRelease(prov);
        if (img) {
            _displayRect = [self _computeDisplayRect];
            /* flip なし: CGContextDrawImage が自動的に正しい向きで描画 */
            CGContextDrawImage(ctx, _displayRect, img);
            CGImageRelease(img);
        }
    }
}


- (void)setFrameSize:(NSSize)sz
{
    [super setFrameSize:sz];
    /* Cairo surface sync happens on replot via _giza_prepare_interactive_draw */
    [self setNeedsDisplay:YES];
}

- (void)keyDown:(NSEvent *)ev
{
    NSString *s = [ev charactersIgnoringModifiers];
    if ([s length] == 0) return;
    unichar c = [s characterAtIndex:0];
    switch(c) {
        case NSLeftArrowFunctionKey:  _eventCh='D'; break;
        case NSRightArrowFunctionKey: _eventCh='C'; break;
        case NSUpArrowFunctionKey:    _eventCh='A'; break;
        case NSDownArrowFunctionKey:  _eventCh='B'; break;
        case 27:                      _eventCh='q'; break;
        default: _eventCh=(char)(c&0x7F);
    }
    /* Return cursor position at key press, not the view centre (matches /xw). */
    _eventPt = [self surfacePointFromViewPoint:[self currentMouseViewPoint]];
    _gotEvent = YES;
}

/* Mouse position in this view's coordinate system */
- (NSPoint)currentMouseViewPoint
{
    NSWindow *win = [self window];
    if (win)
        return [self convertPoint:[win mouseLocationOutsideOfEventStream] fromView:nil];
    return NSMakePoint(self.bounds.size.width / 2, self.bounds.size.height / 2);
}

- (void)_recordButton:(NSEvent *)ev ch:(char)ch {
    NSPoint vp = [self convertPoint:[ev locationInWindow] fromView:nil];
    _eventPt = [self surfacePointFromViewPoint:vp];
    _eventCh = ch; _gotEvent = YES;
}
- (void)mouseDown:(NSEvent *)ev {
    char ch = ([ev modifierFlags]&NSEventModifierFlagShift)
              ?(char)GIZA_SHIFT_CLICK:(char)GIZA_LEFT_CLICK;
    [self _recordButton:ev ch:ch];
}
- (void)rightMouseDown:(NSEvent *)ev { [self _recordButton:ev ch:(char)GIZA_RIGHT_CLICK]; }
- (void)otherMouseDown:(NSEvent *)ev { [self _recordButton:ev ch:(char)GIZA_MIDDLE_CLICK]; }
- (void)scrollWheel:(NSEvent *)ev {
    CGFloat dy=[ev scrollingDeltaY],dx=[ev scrollingDeltaX];
    char ch;
    if(dy>0) ch=(char)GIZA_SCROLL_UP;
    else if(dy<0) ch=(char)GIZA_SCROLL_DOWN;
    else if(dx>0) ch=(char)GIZA_SCROLL_LEFT;
    else ch=(char)GIZA_SCROLL_RIGHT;
    [self _recordButton:ev ch:ch];
}

- (void)waitForInputEvent {
    _gotEvent = NO;
    [[self window] setAcceptsMouseMovedEvents:YES];
    while (!_gotEvent) {
        NSEvent *ev = [NSApp nextEventMatchingMask:NSEventMaskAny
                                         untilDate:[NSDate distantFuture]
                                            inMode:NSDefaultRunLoopMode
                                           dequeue:YES];
        if (ev) [NSApp sendEvent:ev];
    }
    [[self window] setAcceptsMouseMovedEvents:NO];
}

/* Band-aware event loop: also handles mouse-moved events to update band */
- (void)waitForInputEventWithBand {
    _gotEvent = NO;
    [[self window] setAcceptsMouseMovedEvents:YES];
    while (!_gotEvent) {
        /* distantFutureでブロックするとdispatch_asyncブロック内でRunLoopが詰まる。
         * 短いタイムアウトでRunLoopを回し続けることで回避する。 */
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode
                                 beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
        NSEvent *ev;
        while ((ev = [NSApp nextEventMatchingMask:NSEventMaskAny
                                       untilDate:[NSDate distantPast]
                                          inMode:NSDefaultRunLoopMode
                                         dequeue:YES])) {
            if (ev.type == NSEventTypeMouseMoved ||
                ev.type == NSEventTypeLeftMouseDragged) {
                NSPoint vp = [self convertPoint:[ev locationInWindow] fromView:nil];
                NSPoint sp = [self surfacePointFromViewPoint:vp];
                [_bandView updateCursor:sp displayRect:_displayRect];
                [_bandView displayIfNeeded];
            } else {
                [NSApp sendEvent:ev];
            }
        }
    }
    [[self window] setAcceptsMouseMovedEvents:NO];
}

/* Attach a GizaBandView overlay */
- (void)attachBandViewMode:(int)mode anchorSurfacePt:(NSPoint)anc
{
    [self detachBandView];
    NSSize ss = NSMakeSize(_pixelWidth > 0 ? _pixelWidth : (int)self.bounds.size.width,
                           _pixelHeight > 0 ? _pixelHeight : (int)self.bounds.size.height);
    _bandView = [[GizaBandView alloc]
                    initWithFrame:self.bounds
                             mode:mode
                           anchor:anc
                      surfaceSize:ss
                      displayRect:_displayRect];
    [self addSubview:_bandView];
    _bandActive = YES;
}

- (void)detachBandView
{
    if (_bandView) {
        [_bandView removeFromSuperview];
        _bandView = nil;
    }
    _bandActive = NO;
}

- (NSPoint)surfacePointFromViewPoint:(NSPoint)vp
{
    NSSize bs = self.bounds.size;
    /* _displayRect が未計算なら view サイズから等倍で変換 */
    if (_pixelWidth <= 0 || _pixelHeight <= 0
        || _displayRect.size.width < 1 || _displayRect.size.height < 1) {
        if (bs.width > 0 && bs.height > 0) {
            CGFloat sx = vp.x / bs.width  * (_pixelWidth  > 0 ? _pixelWidth  : bs.width);
            CGFloat sy = (bs.height - vp.y) / bs.height
                     * (_pixelHeight > 0 ? _pixelHeight : bs.height);
            return NSMakePoint(sx, sy);
        }
        return NSMakePoint(vp.x, bs.height - vp.y);
    }
    /* view 座標 (y up) → cairo surface 座標 (y down) */
    CGFloat sx = (vp.x - _displayRect.origin.x) / _displayRect.size.width  * _pixelWidth;
    CGFloat sy = (_displayRect.origin.y + _displayRect.size.height - vp.y)
               / _displayRect.size.height * _pixelHeight;
    return NSMakePoint(sx, sy);
}

- (NSPoint)lastEventPoint { return _eventPt; }
- (char)lastEventChar     { return _eventCh; }
- (void)setPixelData:(unsigned char*)data width:(int)w height:(int)h stride:(int)s {
    _pixelData=data; _pixelWidth=w; _pixelHeight=h; _pixelStride=s;
}
@end

/* ======================================================================== */
/* Module state                                                              */
/* ======================================================================== */

static NSWindow  *_osxcocoa_win [GIZA_MAX_DEV];
static GizaView  *_osxcocoa_view[GIZA_MAX_DEV];
static int        _osxcocoa_inuse[GIZA_MAX_DEV];
static int        _app_started = 0;
static volatile int _app_on_main = 0;
static volatile int _nsapp_running = 0;

/* Called from giza-osxcocoa-main.m BEFORE worker thread starts */
void _giza_osxcocoa_mark_app_ready(void)
{
    _app_started  = 1;
    _app_on_main  = 1;
    _nsapp_running = 1;
}

static void _ensure_app(void)
{
    if (_app_started) return;
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp finishLaunching];
    _app_started = 1;
}

static void _drain_events(void)
{
    NSEvent *ev;
    while ((ev=[NSApp nextEventMatchingMask:NSEventMaskAny
                                 untilDate:[NSDate distantPast]
                                    inMode:NSDefaultRunLoopMode
                                   dequeue:YES]))
        [NSApp sendEvent:ev];
}

static void _run_on_main(dispatch_block_t block)
{
    if ([NSThread isMainThread]) {
        block();
        return;
    }
    dispatch_semaphore_t sem = dispatch_semaphore_create(0);
    dispatch_async(dispatch_get_main_queue(), ^{
        block();
        dispatch_semaphore_signal(sem);
    });
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
}

/* ======================================================================== */
/* _giza_osxcocoa_start_app_if_needed                                            */
/* ======================================================================== */

void
_giza_osxcocoa_start_app_if_needed(void)
{
    if ([NSThread isMainThread]) {
        /* Main thread: init NSApp here (simple programs without osxcocoa-main.m) */
        if (!_app_started) {
            _ensure_app();
        }
        _app_on_main = 0;
        return;
    }

    /* Worker thread: NSApp must already be initialized by giza-osxcocoa-main.m.
     * Wait until _giza_osxcocoa_mark_app_ready() has been called. */
    int waited_ms = 0;
    while (!_nsapp_running && waited_ms < 5000) {
        usleep(10000);
        waited_ms += 10;
    }
    if (!_nsapp_running) {
        fprintf(stderr, "giza-driver-osxcocoa: NSApp not ready: link with -lgiza-osxcocoa-main\n");
    }
}

/* ======================================================================== */
/* C bridge functions                                                        */
/* ======================================================================== */

CGContextRef
_giza_osxcocoa_open_window(int devId, int width, int height)
{
    if (devId < 0 || devId >= GIZA_MAX_DEV) return NULL;
    __block CGContextRef result = NULL;
    _run_on_main(^{
        NSRect frame = NSMakeRect(100,100,width,height);
        NSWindowStyleMask style = NSWindowStyleMaskTitled
                                | NSWindowStyleMaskClosable
                                | NSWindowStyleMaskResizable
                                | NSWindowStyleMaskMiniaturizable;
        NSWindow *win = [[NSWindow alloc]
                           initWithContentRect:frame styleMask:style
                                       backing:NSBackingStoreBuffered defer:NO];
        [win setTitle:@GIZA_OSXCOCOA_TITLE];
        [win setReleasedWhenClosed:NO];
        GizaView *view = [[GizaView alloc]
                            initWithFrame:NSMakeRect(0,0,width,height) devId:devId];
        [view rebuildLayerSize:NSMakeSize(width,height)];
        [win setContentView:view];
        [win makeFirstResponder:view];
        [win makeKeyAndOrderFront:nil];
        [NSApp activateIgnoringOtherApps:YES];
        _osxcocoa_win[devId]=win; _osxcocoa_view[devId]=view; _osxcocoa_inuse[devId]=1;
        if (!_app_on_main) _drain_events();
        CGLayerRef layer=[view cgLayer];
        result = layer ? CGLayerGetContext(layer) : NULL;
    });
    return result;
}

CGContextRef
_giza_osxcocoa_clear_and_get_context(int devId, int width, int height)
{
    if (devId < 0 || devId >= GIZA_MAX_DEV || !_osxcocoa_inuse[devId]) return NULL;
    __block CGContextRef result = NULL;
    _run_on_main(^{
        GizaView *view = _osxcocoa_view[devId];
        [view setFrameSize:NSMakeSize(width, height)];
        [view rebuildLayerSize:NSMakeSize(width,height)];
        [view setNeedsDisplay:YES];
        CGLayerRef layer = [view cgLayer];
        result = layer ? CGLayerGetContext(layer) : NULL;
    });
    return result;
}

/* ======================================================================== */
/* _giza_osxcocoa_resize_window                                              */
/* ======================================================================== */

/**
 * Resize the NSWindow and GizaView to the given pixel dimensions.
 */
void
_giza_osxcocoa_resize_window(int devId, int width, int height)
{
    if (devId < 0 || devId >= GIZA_MAX_DEV || !_osxcocoa_inuse[devId]) return;
    _run_on_main(^{
        NSWindow *win = _osxcocoa_win[devId];
        GizaView *view = _osxcocoa_view[devId];
        [win setContentSize:NSMakeSize(width, height)];
        [view setFrameSize:NSMakeSize(width, height)];
        [view rebuildLayerSize:NSMakeSize(width, height)];
        [view setNeedsDisplay:YES];
    });
}

/* ======================================================================== */
/* _giza_osxcocoa_get_view_size                                              */
/* ======================================================================== */

/**
 * Return the current GizaView size in pixels (main-thread safe).
 */
void
_giza_osxcocoa_get_view_size(int devId, int *width, int *height)
{
    if (width)  *width  = 0;
    if (height) *height = 0;
    if (!width || !height) return;
    if (devId < 0 || devId >= GIZA_MAX_DEV || !_osxcocoa_inuse[devId]) return;

    __block int w = 0, h = 0;
    _run_on_main(^{
        GizaView *view = _osxcocoa_view[devId];
        if (!view) return;
        NSSize sz = [view bounds].size;
        w = (int)(sz.width  + 0.5);
        h = (int)(sz.height + 0.5);
    });
    *width  = w;
    *height = h;
}

void
_giza_osxcocoa_flush_pixels(int devId, unsigned char *data,
                        int width, int height, int stride)
{
    if (!_osxcocoa_inuse[devId]) return;
    _run_on_main(^{
        GizaView *view = _osxcocoa_view[devId];
        [view setPixelData:data width:width height:height stride:stride];
        [view setNeedsDisplay:YES];
        [view displayIfNeeded];
        if (!_app_on_main) _drain_events();
    });
}

void
_giza_osxcocoa_close_window(int devId)
{
    if (!_osxcocoa_inuse[devId]) return;
    _run_on_main(^{
        [_osxcocoa_win[devId] close];
        _osxcocoa_win[devId]=nil; _osxcocoa_view[devId]=nil; _osxcocoa_inuse[devId]=0;
    });
}

void
_giza_osxcocoa_wait_for_event(int devId, float *x, float *y, char *ch)
{
    if (!_osxcocoa_inuse[devId]) { *ch='q'; return; }
    GizaView *view = _osxcocoa_view[devId];
    if ([NSThread isMainThread]) {
        [view waitForInputEvent];
        NSPoint pt = [view lastEventPoint];
        *x=(float)pt.x; *y=(float)pt.y; *ch=[view lastEventChar];
    } else {
        __block NSPoint pt=NSZeroPoint;
        __block char c=' ';
        dispatch_semaphore_t sem = dispatch_semaphore_create(0);
        dispatch_async(dispatch_get_main_queue(), ^{
            [view waitForInputEvent];
            pt=[view lastEventPoint]; c=[view lastEventChar];
            dispatch_semaphore_signal(sem);
        });
        dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
        *x=(float)pt.x; *y=(float)pt.y; *ch=c;
    }
}

/* attach + wait + detach を全てmainスレッドで一括実行することでデッドロックを回避 */
void
_giza_osxcocoa_wait_for_event_band(int devId, int mode,
                                   float xancSurface, float yancSurface,
                                   float *x, float *y, char *ch)
{
    if (!_osxcocoa_inuse[devId]) { *ch='q'; return; }
    GizaView *view = _osxcocoa_view[devId];
    __block NSPoint pt = NSZeroPoint;
    __block char c = ' ';
    _run_on_main(^{
        NSPoint anc = NSMakePoint(xancSurface, yancSurface);
        [view attachBandViewMode:mode anchorSurfacePt:anc];
        [view waitForInputEventWithBand];
        pt = [view lastEventPoint];
        c  = [view lastEventChar];
        [view detachBandView];
    });
    *x = (float)pt.x; *y = (float)pt.y; *ch = c;
}

#endif /* CAIRO_HAS_QUARTZ_SURFACE */
#endif /* __APPLE__ */
