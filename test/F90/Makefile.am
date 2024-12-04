AM_FCFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS) -I../../src
AM_LDFLAGS = -no-install
LDADD = ../../src/libpgplot.la

CLEANFILES = *.png

ftests = test-2D test-fortran test-pgaxis #test-pgncur

TESTS = $(ftests)
check_PROGRAMS = $(ftests)
AM_DEFAULT_SOURCE_EXT = .f90

# Ensure that the tests are built as part of the 'check' target
check: $(ftests) check_output_files

test_output_files = giza-test.png giza-test-2D.png

# Custom target to check for the existence of expected output files from the Fortran tests
check_output_files: $(ftests)
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