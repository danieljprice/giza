/* giza-osxcocoa-main.h
 *
 * giza - a scientific plotting library built on cairo
 *
 * Copyright (c) 2026      Shugo Suwazono
 *
 * macOS native window driver — public header for giza-osxcocoa-main.m;
 * include this and rename main() to giza_main() for full interactive support.
 *
 * This library is free software; and you are welcome to redistribute
 * it under the terms of the GNU Lesser General Public License
 * (LGPL, see COPYING file for details).
 *
 * This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
 * See the LGPL for specific language governing rights and limitations.
 *
 * Include this header and link with -lgiza-osxcocoa-main to use the /osxcocoa device
 * without beachball cursor issues.
 *
 * Rename your main() to giza_main() and link:
 *   cc myprogram.c -lgiza -lgiza-osxcocoa-main -framework Cocoa -fobjc-arc
 */

#ifndef GIZA_OSXCOCOA_MAIN_H
#define GIZA_OSXCOCOA_MAIN_H
#ifdef __APPLE__
/* Your program should define this instead of main() */
extern int giza_main(int argc, char **argv);
#endif
#endif
