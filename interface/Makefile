#
# Preliminary build of the libpgplot interface to giza
# Added by Daniel Price, July 2010
#
F90C=gfortran
CC=gcc
CFLAGS=-Wall -Wextra -O3 -I../src -I/usr/X11/include
VPATH=../src
X11LIBS = -L/usr/X11R6/lib -lX11 -lcairo
LIBS=$(X11LIBS) -L/usr/local/lib/x86_64/ -lgfortran

# define the implicit rule to make a .o file from a .f90/.f95 file
# (some Make versions don't know this)

%.o : %.f90
	$(F90C) $(F90FLAGS) -c $< -o $@
%.o : %.F90
	$(F90C) $(F90FLAGS) -c $< -o $@
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@


GIZASRC=${shell cd ../src/; ls *.c}
GIZAOBJ=${GIZASRC:.c=.o}

SRC= giza-fortran.F90 giza-pgplot.f90
OBJ1=$(SRC:.f90=.o)
OBJ= $(GIZAOBJ) $(OBJ1:.F90=.o)
RANLIB=ranlib
GIZA_LIB=
SHARED_LIB=libpgplot.dylib
SHARED_LD=gcc -shared -o libpgplot.dylib
SHARED_LIB_LIBS=${GIZA_LIB}

SHARED_LIBC=libcpgplot.dylib
SHARED_LDC=gcc -shared -o libcpgplot.dylib

all: lib libc

lib: libpgplot.a $(SHARED_LIB)

libc: libcpgplot.a $(SHARED_LIBC)

libpgplot.a: $(OBJ)
	ar ru libpgplot.a $(OBJ)
	$(RANLIB) libpgplot.a

libcpgplot.a: $(GIZAOBJ) giza-cpgplot.o
	ar ru libcpgplot.a $(GIZAOBJ) giza-cpgplot.o
	$(RANLIB) libcpgplot.a

$(SHARED_LIB): $(OBJ)
	$(SHARED_LD) $(LIBS) $(OBJ) $(SHARED_LIB_LIBS)

$(SHARED_LIBC): $(GIZAOBJ) giza-cpgplot.o
	$(SHARED_LDC) $(LIBS) $(GIZAOBJ) giza-cpgplot.o $(SHARED_LIB_LIBS)

clean:
	rm -f libpgplot.a libcpgplot.a $(SHARED_LIB) $(SHARED_LIBC) *.o *.mod
