#--------------------------------------------------------------------
#  giza - a scientific plotting library built on cairo
#  Copyright (c) 2010      James Wetter and Daniel Price
#  Copyright (c) 2010-2012 Daniel Price
#
#  This library is free software; and you are welcome to redistribute
#  it under the terms of the GNU General Public License
#  (GPL, see LICENSE file for details) and the provision that
#  this notice remains intact. If you modify this file, please
#  note section 2a) of the GPLv2 states that:
#
#   a) You must cause the modified files to carry prominent notices
#      stating that you changed the files and the date of any change.
#
#  This software is distributed "AS IS", with ABSOLUTELY NO WARRANTY.
#  See the GPL for specific language governing rights and limitations.
#
#  The Original code is the giza plotting library.
#
#  Contributor(s):
#       James Wetter <wetter.j@gmail.com>
#       Daniel Price <daniel.price@monash.edu> (main contact)
#
#--------------------------------------------------------------------

.KEEP_STATE:

#
# Version info
#
GIZAV=0
MINORV=8
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
ifndef CC
CC= gcc
endif
ifndef CFLAGS
CFLAGS= -fPIC -Wall -Wextra -O3 -g
endif

#
# The prefix for the installed files
#
DESTDIR=
PREFIX=/usr/local

#
# Change the following lines if giza can't find x11 or cairo
#
X11DIR=/usr/X11R6/
#X11DIR=/opt/local/
X11LIBS= -L${X11DIR}/lib -lX11
CAIROLIBS= -L${X11DIR}/lib -lcairo
INCFLAGS= -I/${X11DIR}/include -I${X11DIR}/include/cairo -I$(PREFIX)/include -I$(INCDIR) -I$(SRCDIR)

LDFLAGS= $(X11LIBS) $(CAIROLIBS) -lm

#
#--if compiling with Freetype (experimental at this stage)
#
ifeq ($(USE_FREETYPE),yes)
FT_INCLUDE=-I${X11DIR}/include/freetype2
FT_LIBS= -lfreetype
INCFLACS+= $(FT_INCLUDE)
LDFLAGS+= $(FT_LIBS)
CPPFLAGS+= -DGIZA_HAS_FT_FONT
endif

#
# For creating the tarball
#
DISTDIR=giza-$(VERSION)

KNOWN_SYSTEM=no
OSTYPE=${shell uname}
#
# Settings for specific compilers
#
#ifeq ($(SYSTEM),linux)
  # using gcc
  CC= gcc
  CFLAGS= -fPIC -Wall -Wextra -O3 -g
  SHAREDLIBFLAGS= -shared
  DEBUGFLAG= -Wall -Wextra -g -O0
  RANLIB= ranlib
  LIBTOOL= $(CC) 
  FC= gfortran
  KNOWN_SYSTEM=yes
#endif

#
#--libpgplot needs to be linked against the
#  library from the Fortran compiler used
#
FLIBS= -L/usr/local/gfortran/lib/ -lgfortran

LIBEXT=.a

GIZALIB=$(LIBDIR)/libgiza
PGPLOTLIB=$(LIBDIR)/libpgplot
CPGPLOTLIB=$(LIBDIR)/libcpgplot

STATIC_LIB_GIZA=$(GIZALIB)$(LIBEXT)
STATIC_LIB_PGPLOT=$(PGPLOTLIB)$(LIBEXT)
STATIC_LIB_CPGPLOT=$(CPGPLOTLIB)$(LIBEXT)

ifeq ($(OSTYPE), Darwin)
   SHREXT=.dylib
   SHARED_LIB_GIZA_VERSION=$(GIZALIB).$(VERSION)$(SHREXT)
else
   SHREXT=.so
   SHARED_LIB_GIZA_VERSION=$(GIZALIB)$(SHREXT).$(VERSION)
endif

SHARED_LIB_GIZA=$(GIZALIB)$(SHREXT)
SHARED_LIB_PGPLOT=$(PGPLOTLIB)$(SHREXT)
SHARED_LIB_CPGPLOT=$(CPGPLOTLIB)$(SHREXT)

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
	$(FC) $(FFLAGS) $(INCFLAGS) -c $< -o $@
%.o : %.F90
	$(FC) $(FFLAGS) $(INCFLAGS) -c $< -o $@

#
# The source files
#
SOURCE= giza-annotate.c giza-arrow-style.c giza-arrow.c giza-band-style.c \
	giza-band.c giza-box-time.c giza-box.c giza-buffering.c \
	giza-character-size.c giza-circle.c giza-clipping.c giza-colour-bar.c \
	giza-colour-index.c giza-colour-palette.c giza-colour-table.c \
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
	giza-vector.c giza-viewport.c giza-version.c giza-warnings.c giza-window.c giza.c \
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

.PHONY: libgiza giza pgplot cpgplot

all: giza pgplot cpgplot

giza: libgiza $(SHARED_LIB_GIZA)

libgiza: $(STATIC_LIB_GIZA)
	@echo ""
	@echo "Finished building giza"
	@echo ""

pgplot: $(STATIC_LIB_PGPLOT) $(SHARED_LIB_PGPLOT)
	@echo ""
	@echo "Finished building libpgplot"
	@echo ""

cpgplot: $(STATIC_LIB_CPGPLOT) $(SHARED_LIB_CPGPLOT)
	@echo ""
	@echo "Finished building libcpgplot"
	@echo ""

#
#--static libraries
#
$(STATIC_LIB_GIZA): checksystem $(INCDIR)/giza-version.h $(OBJECTS)
	ar ru $@ $(OBJECTS)
	$(RANLIB) $@

$(STATIC_LIB_PGPLOT): $(INCDIR)/giza-version.h $(OBJECTS) giza-fortran.o giza-pgplot.o
	ar ru $@ $(OBJECTS) giza-fortran.o giza-pgplot.o
	$(RANLIB) $@

$(STATIC_LIB_CPGPLOT): $(INCDIR)/giza-version.h $(OBJECTS) giza-cpgplot.o
	ar ru $@ $(OBJECTS) giza-cpgplot.o
	$(RANLIB) $@

#
#--dynamic libraries
#
$(SHARED_LIB_GIZA): $(INCDIR)/giza-version.h $(OBJECTS)
	$(LIBTOOL) $(SHAREDLIBFLAGS) -o $(SHARED_LIB_GIZA_VERSION) $(OBJECTS) $(LDFLAGS)
	ln -sf $(SHARED_LIB_GIZA_VERSION) $(SHARED_LIB_GIZA)

$(SHARED_LIB_PGPLOT): $(INCDIR)/giza-version.h $(OBJECTS) giza-fortran.o giza-pgplot.o
	$(LIBTOOL) $(SHAREDLIBFLAGS) -o $(SHARED_LIB_PGPLOT) $(OBJECTS) giza-fortran.o giza-pgplot.o $(LDFLAGS) $(FLIBS)

$(SHARED_LIB_CPGPLOT):  $(INCDIR)/giza-version.h $(OBJECTS) giza-cpgplot.o
	$(LIBTOOL) $(SHAREDLIBFLAGS) -o $@ $(OBJECTS) giza-cpgplot.o $(LDFLAGS)

#
#--header file specifying current giza version
#
$(INCDIR)/giza-version.h:
	@echo "creating $@"
	@echo "#define GIZA_VERSION_MAJOR $(GIZAV)" > $@
	@echo "#define GIZA_VERSION_MINOR $(MINORV)" >> $@
	@echo "#define GIZA_VERSION_MICRO $(MICROV)" >> $@
	@echo "#define GIZA_VERSION_STRING \"$(VERSION)\"" >> $@
#
#--other dependencies
#
.PHONY: install installgiza installpgplot dist clean

install: installpgplot destdircheck checksystem giza

installgiza: destdircheck checksystem giza
	cp $(STATIC_LIB_GIZA) $(DESTDIR)$(PREFIX)/lib/
	cp $(SHARED_LIB_GIZA) $(DESTDIR)$(PREFIX)/lib/
	cp $(INCDIR)/giza-shared.h  $(DESTDIR)$(PREFIX)/include/
	cp $(INCDIR)/giza-version.h $(DESTDIR)$(PREFIX)/include/
	cp $(INCDIR)/giza.h         $(DESTDIR)$(PREFIX)/include/

installpgplot: installgiza pgplot cpgplot
	cp $(STATIC_LIB_PGPLOT)  $(DESTDIR)$(PREFIX)/lib/
	cp $(SHARED_LIB_PGPLOT)  $(DESTDIR)$(PREFIX)/lib/
	cp $(STATIC_LIB_CPGPLOT) $(DESTDIR)$(PREFIX)/lib/
	cp $(SHARED_LIB_CPGPLOT) $(DESTDIR)$(PREFIX)/lib/
	cp $(INCDIR)/cpgplot.h $(DESTDIR)$(PREFIX)/include/

distclean: destdircheck clean
	@rm -f $(DESTDIR)$(PREFIX)/lib/$(STATIC_LIB_GIZA)
	@rm -f $(DESTDIR)$(PREFIX)/lib/$(SHARED_LIB_GIZA)
	@rm -f $(DESTDIR)$(PREFIX)/lib/$(STATIC_LIB_PGPLOT)
	@rm -f $(DESTDIR)$(PREFIX)/lib/$(SHARED_LIB_PGPLOT)
	@rm -f $(DESTDIR)$(PREFIX)/lib/$(STATIC_LIB_CPGPLOT)
	@rm -f $(DESTDIR)$(PREFIX)/lib/$(SHARED_LIB_CPGPLOT)
	@rm -f $(DESTDIR)$(PREFIX)/include/giza-shared.h
	@rm -f $(DESTDIR)$(PREFIX)/include/giza-version.h
	@rm -f $(DESTDIR)$(PREFIX)/include/giza.h

destdircheck:
	@if test -d $(DESTDIR)$(PREFIX)/lib; then echo $(DESTDIR)$(PREFIX)/lib exists and is a directory; else \
        echo; echo "*** ERROR in make install ***"; echo "$(DESTDIR)$(PREFIX)/lib is not a valid directory"; echo;\
        $(MAKE) err; fi;

dist: distclean
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
	cp ../README ../$(DISTDIR)
	cp ../CHANGES ../$(DISTDIR)
	cp ../COPYING ../$(DISTDIR)
	cp ../AUTHORS ../$(DISTDIR)
	tar czvf ../$(DISTDIR).tar.gz ../$(DISTDIR)
	rm -rf ../$(DISTDIR)

clean: cleantest
	rm -f $(LIBDIR)/libgiza* $(LIBDIR)/libpgplot* $(LIBDIR)/libcpgplot* *.o *.mod $(INCDIR)/giza-version.h ../*.tar.gz

cleantest:
	cd ../test; $(MAKE) clean

cleandist: clean
	rm -fr ../$(DISTDIR)
	rm -f ../$(DISTDIR).tar.gz

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
