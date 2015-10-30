CLEANFILES = *.mod .*pc

lib_LTLIBRARIES = libcpgplot.la libgiza.la libpgplot.la

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
       giza-warnings.c giza-window.c giza.c lex.yy.c

# uncomment these if pkg-config not working
#X11_LIBS = "-L/usr/X11/lib -L/$(prefix)/lib -lX11"
#X11_CFLAGS = "-I/usr/X11/include"
#CAIRO_LIBS = "-lcairo"

libgiza_la_CPPFLAGS = $(X11_CFLAGS) $(CAIRO_CFLAGS)

libcpgplot_la_CPPFLAGS = $(libgiza_la_CPPFLAGS)

libpgplot_la_CPPFLAGS = $(libgiza_la_CPPFLAGS)

libgiza_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
       -no-undefined $(X11_LIBS) $(CAIRO_LIBS)

libcpgplot_la_SOURCES = giza-cpgplot.c $(libgiza_la_SOURCES)

libcpgplot_la_LDFLAGS = $(libgiza_la_LDFLAGS)

libpgplot_la_SOURCES = giza-fortran.F90 giza-pgplot.f90 $(libgiza_la_SOURCES)

libpgplot_la_LDFLAGS = $(libgiza_la_LDFLAGS)

include_HEADERS = cpgplot.h giza.h giza-shared.h giza-version.h giza.mod

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cpgplot.pc giza.pc pgplot.pc
