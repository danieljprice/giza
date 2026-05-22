/* giza-osxcocoa-main.m
 *
 * giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2026      Shugo Suwazono
 *
 * macOS native window driver — main thread entry point wrapping
 * giza_main() on a worker thread so that NSApp runs on the main thread.*
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License (LGPL).
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 */

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#include <stdlib.h>
#include <pthread.h>

extern int giza_main(int argc, char **argv);
extern void _giza_osxcocoa_mark_app_ready(void);

typedef struct { int argc; char **argv; int retval; } GizaMainArgs;

static void *_giza_worker(void *arg)
{
    GizaMainArgs *a = (GizaMainArgs *)arg;
    a->retval = giza_main(a->argc, a->argv);
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSApp stop:nil];
        NSEvent *ev = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                         location:NSZeroPoint
                                    modifierFlags:0 timestamp:0
                                     windowNumber:0 context:nil
                                          subtype:0 data1:0 data2:0];
        [NSApp postEvent:ev atStart:YES];
    });
    return NULL;
}

int main(int argc, char **argv)
{
    /*
     * IMPORTANT: All NSApplication setup MUST happen on the main thread
     * before the worker thread starts calling giza.
     */
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp finishLaunching];  /* must be on main thread */

    /* Mark NSApp as ready BEFORE starting worker */
    /* Worker will check _nsapp_running via _giza_osxcocoa_mark_app_ready() */
    _giza_osxcocoa_mark_app_ready();  /* signal to driver that app is ready */

    /* Now start worker thread */
    GizaMainArgs args = { argc, argv, 0 };
    pthread_t worker;
    if (pthread_create(&worker, NULL, _giza_worker, &args) != 0) {
        fprintf(stderr, "giza-osxcocoa-main: failed to create worker thread\n");
        return 1;
    }

    /* Run AppKit event loop on main thread */
    [NSApp run];

    pthread_join(worker, NULL);
    return args.retval;
}
#endif
