AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
AM_CPPFLAGS = -I../../src -I../../include $(CAIRO_CFLAGS) $(X11_CFLAGS)
AM_LDFLAGS = -no-install -lm -lX11
LDADD = ../../src/libgiza.la

CPGPLOT_LDADD = ../../src/libcpgplot.la ../../src/libgiza.la

CLEANFILES = *.png *.pdf *.svg

if HAVE_OSXCOCOA
LDADD += ../../src/libgiza-osxcocoa-main.la
endif

# Tests that run non-interactively (file-based devices only)
auto_ctests = test-format-number test-pdf test-png test-svg \
	test-cpgpnts test-cpgconb test-cpgconf test-cpgconl test-cpgconx test-cpghi2d test-cpgscrl

# Tests that need an X display (use /xw or "?" which defaults to /xw)
interactive_ctests = test-arrow test-box test-cairo-xw test-change-page \
	test-circle test-colour-index \
	test-environment \
	test-error-bars test-giza-xw test-line-cap \
	test-line-style test-openclose test-points \
	test-qtext test-rectangle test-set-line-width \
	test-vector test-window test-XOpenDisplay test-contour test-render \
	test-band

TESTS = $(auto_ctests)
check_PROGRAMS = $(auto_ctests) $(interactive_ctests)

test_cpgpnts_LDADD = $(CPGPLOT_LDADD)
test_cpgconb_LDADD = $(CPGPLOT_LDADD)
test_cpgconf_LDADD = $(CPGPLOT_LDADD)
test_cpgconl_LDADD = $(CPGPLOT_LDADD)
test_cpgconx_LDADD = $(CPGPLOT_LDADD)
test_cpghi2d_LDADD = $(CPGPLOT_LDADD)
test_cpgscrl_LDADD = $(CPGPLOT_LDADD)

test_cairo_xw_LDADD = $(LDADD) $(CAIRO_LIBS) $(X11_LIBS)
test_XOpenDisplay_LDADD = $(LDADD) $(X11_LIBS)
