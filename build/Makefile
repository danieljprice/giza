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
# Copyright (C) 2010-2011 James Wetter and Daniel Price. All rights reserved.
# Contact: wetter.j@gmail.com
#          daniel.price@monash.edu
#
#--------------------------------------------------------------------

.KEEP_STATE:

#
# Version info
#
GIZAV=0
MINORV=7
MICROV=0
VERSION=$(GIZAV).$(MINORV).$(MICROV)


KNOWN_SYSTEM=no
SRCDIR=../src
BINDIR=../bin
LIBDIR=../lib
INCDIR=../include
INTERFACEDIR=../interface
VPATH=$(SRCDIR) $(INTERFACEDIR)
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
CAIROLIBS= -L/usr/X11R6/lib -lcairo
INCFLAGS= -I/usr/X11R6/include -I$(INCDIR) -I$(SRCDIR)

LDFLAGS= $(X11LIBS) $(CAIROLIBS) -lm

#
# The prefix for the installed files
#
PREFIX=/usr/local

#
# For creating the tarball
#
DISTDIR=giza-$(VERSION)

KNOWN_SYSTEM=no
#
# Settings for specific compilers
#
#ifeq ($(SYSTEM),linux)
  # using gcc
  CC= gcc
  CFLAGS += -fPIC -Wall -Wextra -O3
  SHAREDLIBFLAGS= -shared
  SHAREDEXT=.so
  DEBUGFLAG= -Wall -Wextra -g -O0
  RANLIB= ranlib
  F90C= gfortran
  KNOWN_SYSTEM=yes
#endif

#
#--libpgplot needs to be linked against the
#  library from the Fortran compiler used
#
FORTRAN_COMPILER_LIBS= -L/usr/local/lib/x86_64/ -lgfortran

GIZALIB=$(LIBDIR)/libgiza.a
PGPLOTLIB=$(LIBDIR)/libpgplot.a
CPGPLOTLIB=$(LIBDIR)/libcpgplot.a

SHARED_LIB_GIZA=$(LIBDIR)/libgiza$(SHAREDEXT)
SHARED_LIB_PGPLOT=$(LIBDIR)/libpgplot$(SHAREDEXT)
SHARED_LIB_CPGPLOT=$(LIBDIR)/libcpgplot$(SHAREDEXT)

#
# Debug flags
#
ifeq ($(DEBUG),yes)
  CFLAGS += $(DEBUGFLAGS)
endif

#
# Some implicit rules
#
%.o: %.c
	$(CC) -c $(CFLAGS) $(INCFLAGS) $< -o $@
%.o : %.f90
	$(F90C) $(F90FLAGS) $(INCFLAGS) -c $< -o $@
%.o : %.F90
	$(F90C) $(F90FLAGS) $(INCFLAGS) -c $< -o $@

#
# The source files
#
SOURCE= giza-annotate.c giza-arrow-style.c giza-arrow.c giza-band-style.c \
	giza-band.c giza-box.c giza-buffering.c giza-character-size.c \
	giza-circle.c giza-clipping.c giza-colour-index.c giza-colour-table.c \
	giza-contour.c giza-cursor-routines.c giza-device-has-cursor.c \
	giza-draw-background.c giza-draw.c giza-driver-eps.c giza-driver-null.c \
	giza-driver-pdf.c giza-driver-png.c giza-driver-ps.c giza-driver-svg.c \
	giza-driver-xw.c giza-drivers.c giza-environment.c giza-error-bars.c \
	giza-fill.c giza-format-number.c giza-function-t.c giza-function-x.c \
	giza-function-y.c giza-get-key-press.c giza-get-surface-size.c \
	giza-io.c giza-label.c giza-line-cap.c giza-line-style.c \
	giza-line-width.c giza-line.c giza-move.c giza-paper.c giza-points.c \
	giza-polygon.c giza-print-id.c giza-prompting.c giza-ptext.c \
	giza-qtext.c giza-rectangle.c giza-render.c giza-save.c giza-set-font.c \
	giza-stroke.c giza-text-background.c giza-text.c giza-transforms.c \
	giza-vector.c giza-viewport.c giza-warnings.c giza-window.c giza.c \
	lex.yy.c

#
# Private header files
#
PHEADERS= giza-arrow-style-private.h giza-band-private.h \
	  giza-character-size-private.h giza-colour-private.h \
	  giza-cursor-private.h giza-driver-eps-private.h \
	  giza-driver-null-private.h giza-driver-pdf-private.h \
	  giza-driver-png-private.h giza-driver-ps-private.h \
	  giza-driver-svg-private.h giza-driver-xw-private.h \
	  giza-drivers-private.h giza-fill-private.h giza-io-private.h \
	  giza-line-style-private.h giza-private.h giza-prompting-private.h \
	  giza-render-private.h giza-set-font-private.h giza-stroke-private.h \
	  giza-text-background-private.h giza-text-private.h \
	  giza-transforms-private.h giza-viewport-private.h \
	  giza-warnings-private.h giza-window-private.h

OBJECTS= $(SOURCE:.c=.o)

all: giza pgplot

giza: $(GIZALIB) $(SHARED_LIB_GIZA)
	cp $(SRCDIR)/giza.h $(INCDIR)/
	cp $(SRCDIR)/giza-shared.h $(INCDIR)/
	@echo ""
	@echo "Finished building giza"
	@echo ""

pgplot: $(PGPLOTLIB) $(SHARED_LIB_PGPLOT)
	@echo ""
	@echo "Finished building libpgplot"
	@echo ""

cpgplot: $(CPGPLOTLIB) $(SHARED_LIB_CPGPLOT)
	cp $(SRCDIR)/cpgplot.h $(INCDIR)/
	@echo ""
	@echo "Finished building libcpgplot"
	@echo ""

#
#--static libraries
#
$(GIZALIB): checksystem $(INCDIR)/giza-version.h $(OBJECTS)
	ar ru $@ $(OBJECTS)
	$(RANLIB) $@

$(PGPLOTLIB): $(INCDIR)/giza-version.h $(OBJECTS) giza-fortran.o giza-pgplot.o
	ar ru $@ $(OBJECTS) giza-fortran.o giza-pgplot.o
	$(RANLIB) $@

$(CPGPLOTLIB): $(INCDIR)/giza-version.h $(OBJECTS) giza-cpgplot.o
	ar ru $@ $(OBJECTS) giza-cpgplot.o
	$(RANLIB) $@

#
#--dynamic libraries
#
$(SHARED_LIB_GIZA): $(INCDIR)/giza-version.h $(OBJECTS)
	$(CC) $(SHAREDLIBFLAGS) -o $@.$(VERSION) $(OBJECTS) $(LDFLAGS)
	ln -sf $(SHARED_LIB_GIZA).$(VERSION) $(SHARED_LIB_GIZA)

$(SHARED_LIB_PGPLOT): $(INCDIR)/giza-version.h $(OBJECTS) giza-fortran.o giza-pgplot.o
	$(CC) $(SHAREDLIBFLAGS) -o $@ $(OBJECTS) giza-fortran.o giza-pgplot.o $(LDFLAGS) $(FORTRAN_COMPILER_LIBS)

$(SHARED_LIB_CPGPLOT):  $(INCDIR)/giza-version.h $(OBJECTS) giza-cpgplot.o
	$(CC) $(SHAREDLIBFLAGS) -o $@ $(OBJECTS) giza-cpgplot.o $(LDFLAGS)

#
#--header file specifying current giza version
#
$(INCDIR)/giza-version.h:
	@echo "creating $@"
	@echo "#define GIZA_VERSION_MAJOR $(GIZAV)" > $@
	@echo "#define GIZA_VERSION_MINOR $(MINORV)" >> $@
	@echo "#define GIZA_VERSION_MICRO $(MICROV)" >> $@
	@echo "#define GIZA_VERSION_STRING \"$(VERSION)\"" >> $@

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
	  @echo "Compiling giza-$(VERSION) for $(SYSTEM) system..........."
	  @echo ""
    else
	  @echo ""
	  @echo " Makefile for giza by James Wetter and Daniel Price "
	  @echo ""
	  @echo " make: ERROR: value of SYSTEM=$(SYSTEM) not recognised..."
	  @echo " => set the environment variable SYSTEM to one listed "
	  @echo "    in build/Makefile and try again"
	  @echo ""
    endif
