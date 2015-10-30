AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
AM_CPPFLAGS = -I../../src
AM_LDFLAGS = -no-install -lm -lX11
LDADD = ../../src/libgiza.la

CLEANFILES = *.png *.pdf *.svg

ctests = test-arrow test-band test-box test-cairo-xw test-change-page \
	test-circle test-colour-index \
	test-environment \
	test-error-bars test-format-number test-giza-xw test-line-cap \
	test-line-style test-openclose test-pdf test-png test-points \
	test-qtext test-rectangle test-set-line-width \
	test-svg test-vector test-window test-XOpenDisplay # test-contour test-render

#TESTS = $(ctests)
check_PROGRAMS = $(ctests)
