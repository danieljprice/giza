AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
AM_CPPFLAGS = -I../../src $(CAIRO_CFLAGS) $(X11_CFLAGS) $(FT_CFLAGS)
AM_LDFLAGS = -no-install -lm -lX11 \
    -Wl,-rpath,$(abs_top_builddir)/src/.libs \
    -Wl,-rpath,$(libdir)
LDADD = ../../src/libgiza.la
CLEANFILES = *.png *.pdf *.svg

if HAVE_OSXCOCOA
AM_CPPFLAGS += -Dmain=giza_main
LDADD += ../../src/libgiza-osxcocoa-main.la
endif

ctests = test-arrow test-box test-cairo-xw test-change-page \
    test-circle test-colour-index \
    test-environment \
    test-error-bars test-format-number test-giza-xw test-line-cap \
    test-line-style test-openclose test-pdf test-png test-points \
    test-qtext test-rectangle test-set-line-width \
    test-svg test-vector test-window test-XOpenDisplay test-contour test-render \
    test-band

TESTS = $(ctests)
check_PROGRAMS = $(ctests)
