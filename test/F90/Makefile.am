AM_FCFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS) -I../../src
AM_LDFLAGS = -no-install
LDADD = ../../src/libpgplot.la

CLEANFILES = *.png

ftests = test-2D test-fortran test-pgaxis test-pgncur

#TESTS = $(ftests)
check_PROGRAMS = $(ftests)
AM_DEFAULT_SOURCE_EXT = .f90
