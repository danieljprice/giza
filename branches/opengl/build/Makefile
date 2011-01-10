#--------------------------------------------------------------------
#
# giza - a scientific plotting layer built on cairo
#
# This file is (or was) part of GIZA, a scientific plotting
# layer built on cairo.
# GIZA comes with ABSOLUTELY NO WARRANTY.
# This is free software; and you are welcome to redistribute
# it under the terms of the GNU General Public License
# (see LICENSE file for details) and the provision that
# this notice remains intact. If you modify this file, please
# note section 2a) of the GPLv2 states that:
#
#  a) You must cause the modified files to carry prominent notices
#     stating that you changed the files and the date of any change.
#
# Copyright (C) 2010 James Wetter. All rights reserved.
# Contact: wetter.j@gmail.com
#
#--------------------------------------------------------------------

.KEEP_STATE:

#
# Version info
#
GIZAV=0
MAJORV=1
MINORV=1
VERSION=$(GIZAV).$(MAJORV).$(MINORV)


KNOWN_SYSTEM=no
SRCDIR=../src
BINDIR=../bin
LIBDIR=../lib
INCDIR=../include
VPATH=$(SRCDIR)
SHELL=/bin/bash

#
# Some basic defaults
#
CC=
CFLAGS=

#
# Change the following lines if giza can't find x11 or giza
#
X11LIBS= -L/usr/X11R6/lib -lX11
CAIROLIBS= -L/usr/local/lib -lcairo
GLLIBS= -L/usr/X11R6/lib -lGL -lXext
INCFLAGS= -I/usr/X11R6/include -I$(INCDIR) -I$(SRCDIR)

LDFLAGS= $(X11LIBS) $(CAIROLIBS) -lm

#
# The prefix for the installed files
#
PREFIX=/usr/local

#
# For creating the tarball
#
DISTDIR=giza-$(GIZAV).$(MAJORV).$(MINORV)

KNOWN_SYSTEM=no
#
# Settings for specific compilers
#
ifeq ($(SYSTEM),linux)
  # using gcc
  CC= gcc
  CFLAGS += -fPIC -Wall -Wextra
  SHAREDLIBFLAGS= -shared
  SHAREDEXT=.so
  DEBUGFLAG= -Wall -Wextra -g -O0
  RANLIB= ranlib
  KNOWN_SYSTEM=yes
endif

#
# Dubug flags
#
ifeq ($(DEBUG),yes)
  CFLAGS += $(DEBUGFLAGS)
endif

#
# Some implicit rules
#
%.o: %.c
	$(CC) -c $(CFLAGS) $(INCFLAGS) $< -o $@

#
# The source files
#
SOURCE= giza-annotate.c giza-arrow.c giza-arrow-style.c giza-band.c \
	giza-band-style.c giza-box.c giza-buffering.c giza.c \
	giza-character-size.c giza-circle.c giza-colour-index.c \
	giza-colour-table.c giza-contour.c giza-device-has-cursor.c \
	giza-draw.c giza-driver-eps.c giza-driver-null.c giza-driver-pdf.c giza-driver-png.c \
	giza-driver-ps.c giza-drivers.c giza-driver-xw.c giza-environment.c \
	giza-error-bars.c giza-fill.c giza-format-number.c giza-function-t.c \
	giza-function-x.c giza-function-y.c giza-get-key-press.c \
	giza-get-surface-size.c giza-io.c giza-label.c giza-line.c \
	giza-line-cap.c giza-line-style.c giza-line-width.c giza-move.c \
	giza-points.c giza-polygon.c giza-prompting.c giza-ptext.c \
	giza-qtext.c giza-rectangle.c giza-render.c giza-save.c \
	giza-set-font.c giza-stroke.c giza-text.c giza-text-background.c \
        giza-transforms.c giza-vector.c giza-viewport.c giza-warnings.c giza-window.c \
	lex.yy.c giza-gl-interactive.c

#
# Private header files
#
PHEADERS= giza-arrow-style-private.h giza-band-private.h \
	  giza-character-size-private.h giza-colour-index-private.h \
	  giza-colour-table-private.h giza-driver-eps-private.h giza-driver-null-private.h \
	  giza-driver-pdf-private.h giza-driver-png-private.h \
	  giza-driver-ps-private.h giza-drivers-private.h \
	  giza-driver-xw-private.h giza-fill-private.h giza-io-private.h \
	  giza-line-style-private.h giza-private.h giza-prompting-private.h \
	  giza-set-font-private.h giza-stroke-private.h \
	  giza-text-private.h giza-text-background-private.h \
          giza-transforms-private.h giza-viewport-private.h \
          giza-warnings-private.h giza-window-private.h 

OBJECTS= $(SOURCE:.c=.o)

all: giza

giza: checksystem $(OBJECTS)
	gcc $(SHAREDLIBFLAGS) -o $(LIBDIR)/libgiza$(SHAREDEXT).$(VERSION) $(LDFLAGS) $(OBJECTS)
	@#rm -f $(LIBDIR)/libgiza$(SHAREDEXT)
	ln -sf $(LIBDIR)/libgiza$(SHAREDEXT).$(VERSION) $(LIBDIR)/libgiza$(SHAREDEXT)
	cp $(SRCDIR)/giza.h $(INCDIR)
	cp $(SRCDIR)/giza-gl.h $(INCDIR)
	cp $(SRCDIR)/giza-features.h $(INCDIR)
	cp $(SRCDIR)/giza-shared-cpp.h $(INCDIR)

install: checksystem giza
	cp -r $(LIBDIR) $(PREFIX)/
	cp -r $(INCDIR) $(PREFIX)/

clean:
	rm -f $(LIBDIR)/* *.o $(INCDIR)/* ../*.tar.gz
	rm -fr ../$(DISTDIR)
	rm -f ../$(DISTDIR).tar.gz

dist: clean
	mkdir ../$(DISTDIR)
	cp -r ../build ../$(DISTDIR)
	cp -r ../docs ../$(DISTDIR)
	cp -r ../include ../$(DISTDIR)
	cp -r ../interface ../$(DISTDIR)
	cp -r ../lib ../$(DISTDIR)
	cp -r ../src ../$(DISTDIR)
	cp -r ../test ../$(DISTDIR)
	cp ../Makefile ../$(DISTDIR)
	cp ../INSTALL ../$(DISTDIR)
	tar czvf ../$(DISTDIR).tar.gz ../$(DISTDIR)
	rm -rf ../$(DISTDIR)

checksystem:
    ifeq ($(KNOWN_SYSTEM), yes)
	  @echo ""
	  @echo "Compiling giza for $(SYSTEM) system..........."
	  @echo ""
    else
	  @echo ""
	  @echo " Makefile for giza by James Wetter and Daniel Price "
	  @echo ""
	  @echo " make: ERROR: value of SYSTEM=$(SYSTEM) not recognised..."
	  @echo " => set the environment variable SYSTEM to one listed "
	  @echo "    in build/Makefile and try again"
	  @echo ""
	  @${MAKE} compilers
	  @$(MAKE) err;
    endif
