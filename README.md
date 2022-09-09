giza - a scientific plotting library for C/Fortran
==================================================

About
-----
A 2D scientific plotting library built on cairo. Provides uniform output to pdf, ps, png and X-Windows. Written in C with no dependencies (other than cairo) as a direct replacement for PGPLOT.

Features
--------
- 2D plotting library
- Output to png, ps, eps, pdf, svg and X-window
- Interactivity via cursor calls (similar to PGCURS, PGBAND)
- Includes PGPLOT interface that compiles replacement versions of libpgplot and libcpgplot with many backwards compatible features
- most of PGPLOT API implemented
- Minimal dependencies: depends only on very standard system libraries (cairo, png, X11)
- Written in standard C with a Fortran 90/95/2003 interface
- Compiles as a standalone library or can be incorporated into other codes
- Interface routines handle both float and double precision input
- Easily called from Fortran/C/C++ code
- Originally written as a backend for SPLASH, in stable use now for more than 10 years

Status
------
![build](https://github.com/danieljprice/giza/workflows/build/badge.svg)

Install
-------
Type:
```
./configure
make install
```
You will need a C compiler, cairo and X11 development packages. If they are
ready to go the above should build giza using gcc,
and "make install" should install under a Linux or Mac operating system.

To install in a special location (e.g. your userspace), use:
```
./configure --prefix=${HOME}
make install
```

If you have X11 or cairo installed in a custom location, or you don't have pkg-config installed, specify the location with the following flags:
```
./configure X11_CFLAGS='-I/usr/X11/include' X11_LIBS='-L/usr/X11/lib -lX11'
            CAIRO_CFLAGS='-I/usr/X11/include/cairo' CAIRO_LIBS='-L/usr/X11/lib/cairo -lcairo'
make install
```
You can find out more configuration options using ./configure --help

Basic usage (using giza directly, not via pgplot interface)
-----------
For simple examples, you can try compiling some of the examples in the tests/ directory, e.g.
```
cd giza/tests/C/
make test-png
./test-png
%giza: test_0000.png created
%giza: test_0001.png created
%giza: test_0002.png created
%giza: test_0003.png created
%giza: test_0004.png created
%giza: test_0005.png created
%giza: test_0006.png created
%giza: test_0007.png created
%giza: test_0008.png created
%giza: test_0009.png created
```
Or for a Fortran equivalent:
```
cd giza/tests/F90/
make test-2D
./test-2D
```

PGPLOT replacement usage
------------------------
giza compiles a version of libpgplot that can be used as a drop-in replacement
for the original PGPLOT libraries

If you want to test a program that previously used PGPLOT, then first compile
and install giza as above (which by default will install libraries
to /usr/local/lib). Then on a Mac you would type:
 (assuming a Mac with giza libraries installed to /usr/local/lib), just type:
```
export DYLD_LIBRARY_PATH=/usr/local/lib:${DYLD_LIBRARY_PATH}
```
or in Linux
```
export LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}
```
then run your PGPLOT-based program (you don't even need to recompile it).
That is, as long as giza's version of pgplot libraries are first in the shared
library path, they are the ones that will be used.

There are some basic examples of PGPLOT usage (mainly written for my own testing)
in the F90 dir:
```
cd giza/test/F90
make test-pgaxis test-pgncur
./test-pgaxis
./test-pgncur
```
