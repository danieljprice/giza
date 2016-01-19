.NOTPARALLEL:

CLEANFILES = *.mod *.pc

lib_LTLIBRARIES = libgiza.la libcpgplot.la libpgplot.la

libgiza_la_SOURCES = giza-annotate.c giza-arrow-style.c giza-arrow.c \
       giza-band-style.c giza-band.c giza-box-time.c giza-box.c \
       giza-buffering.c giza-character-size.c giza-circle.c \
       giza-clipping.c giza-colour-bar.c giza-colour-index.c \
       giza-colour-palette.c giza-colour-table.c giza-contour.c \
       giza-cursor-routines.c giza-device-has-cursor.c \
       giza-draw-background.c giza-draw.c giza-driver-eps.c \
       giza-driver-null.c giza-driver-pdf.c giza-driver-png.c \
       giza-driver-ps.c giza-driver-svg.c giza-driver-xw.c \
       giza-drivers.c giza-environment.c giza-error-bars.c giza-fill.c \
       giza-format-number.c giza-function-t.c giza-function-x.c \
       giza-function-y.c giza-get-key-press.c giza-get-surface-size.c \
       giza-io.c giza-histogram.c giza-label.c giza-line-cap.c \
       giza-line-style.c giza-line-width.c giza-line.c giza-move.c \
       giza-paper.c giza-points.c giza-polygon.c giza-print-id.c \
       giza-prompting.c giza-ptext.c giza-qtext.c giza-rectangle.c \
       giza-render.c giza-save.c giza-set-font.c giza-stroke.c \
       giza-subpanel.c giza-text-background.c giza-text.c \
       giza-transforms.c giza-vector.c giza-viewport.c giza-version.c \
       giza-warnings.c giza-window.c giza.c lex.yy.c \
       giza-arrow-style-private.h giza-driver-svg-private.h giza-stroke-private.h \
       giza-band-private.h giza-driver-xw-private.h giza-subpanel-private.h \
       giza-character-size-private.h giza-drivers-private.h giza-text-background-private.h \
       giza-colour-private.h giza-fill-private.h giza-text-private.h \
       giza-cursor-private.h giza-io-private.h giza-transforms-private.h \
       giza-driver-eps-private.h giza-line-style-private.h giza-version.h \
       giza-driver-null-private.h giza-private.h giza-viewport-private.h \
       giza-driver-pdf-private.h giza-render-private.h giza-warnings-private.h \
       giza-driver-png-private.h giza-set-font-private.h giza-window-private.h \
       giza-driver-ps-private.h giza-shared.h giza.h

libgiza_la_CPPFLAGS = $(X11_CFLAGS) $(CAIRO_CFLAGS)

libcpgplot_la_CPPFLAGS = $(libgiza_la_CPPFLAGS)

libpgplot_la_CPPFLAGS = $(libgiza_la_CPPFLAGS)

libgiza_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
       -no-undefined $(X11_LIBS) $(CAIRO_LIBS)

libcpgplot_la_SOURCES = giza-cpgplot.c

libcpgplot_la_LDFLAGS = $(libgiza_la_LDFLAGS) -lgiza

libpgplot_la_SOURCES = giza-fortran.F90 giza-pgplot.f90

libpgplot_la_LDFLAGS = $(libcpgplot_la_LDFLAGS)

include_HEADERS = cpgplot.h giza.h giza-shared.h giza-version.h giza-fortran.F90

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cpgplot.pc giza.pc pgplot.pc

fmoddir = $(libdir)/$(FC)/modules
fmod_DATA = giza.mod
