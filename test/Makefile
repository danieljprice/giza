CC=gcc
#PREFIX=/usr/X11R6
PREFIX=/opt/local
CFLAGS=-I${PREFIX}/include -I../include/
CAIROFLAGS= -L${PREFIX}/lib -lX11 -lcairo
LDFLAGS=-L../lib/ -lgiza -lm
FC=gfortran
FFLAGS=-O0 -Wall -I../include/
VPATH=../interface/
PGFLAGS=-L$(PGPLOT_DIR) -lpgplot -L/${PREFIX}/lib -lX11 -lpng

%.o : %.f90
	$(FC) $(FFLAGS) -c $< -o $@

%.o : %.F90
	$(FC) $(FFLAGS) -c $< -o $@

all: test-arrow test-box test-giza-xw test-cairo-xw test-png test-pdf test-fortran

test-arrow: test-arrow.o
	$(CC) -o $@ $< $(LDFLAGS)

text-box: test-box.o
	$(CC) -o $@ $< $(LDFLAGS)

test-giza-xw: test-xw.o
	$(CC) -o $@ $< $(LDFLAGS)

test-cairo-xw: test-cairo-xw.o
	$(CC) -o $@ $< $(CAIROFLAGS)

test-png: test-png.o
	$(CC) -o $@ $< $(LDFLAGS)

test-pdf: test-pdf.o
	$(CC) -o $@ $< $(LDFLAGS)

OBJ=giza-fortran.o test-fortran.o
test-fortran: $(OBJ)
	$(FC) $(FFLAGS) -o $@ $(OBJ) $(LDFLAGS)

OBJ1=giza-fortran.o test-2D.o
test-2D: $(OBJ1)
	$(FC) $(FFLAGS) -o $@ $(OBJ1) $(LDFLAGS)

test-pgncur: test-pgncur.o
	$(FC) $(FFLAGS) -o $@ test-pgncur.o $(PGFLAGS)

test-pgaxis: test-pgaxis.o
	$(FC) $(FFLAGS) -o $@ test-pgaxis.o $(PGFLAGS)

clean:
	rm -f *.o *.mod test-giza-xw test-cairo-xw test-png test-pdf test-fortran
