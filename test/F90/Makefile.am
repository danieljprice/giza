AM_FCFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS) -I../../src
AM_LDFLAGS = -no-install
LDADD = ../../src/libpgplot.la

CLEANFILES = *.png

# Tests that run non-interactively (file-based devices only)
auto_ftests = test-2D test-fortran \
	test-pgpnts test-pgconb test-pgconf test-pgconl test-pgconx test-pghi2d test-pgscrl

# Tests that need an X display
interactive_ftests = test-pgaxis

TESTS = $(auto_ftests)
check_PROGRAMS = $(auto_ftests) $(interactive_ftests)
AM_DEFAULT_SOURCE_EXT = .f90

test_output_files = giza-test.png giza-test-2D.png

# Custom target to check for the existence of expected output files from the Fortran tests
check-local: check-TESTS
	@echo "Checking for required png files..."
	@for i in $(test_output_files); do \
	    if [ ! -s $$i ]; then \
	        echo "ERROR: Required file $$i does not exist."; \
	        exit 1; \
	    else \
	        echo "$$i exists"; \
	    fi; \
	done
	@echo "All required png files exist."


