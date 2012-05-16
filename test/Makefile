CC=gcc
#PREFIX=/usr/X11R6
PREFIX=/opt/local
CFLAGS=-I${PREFIX}/include -I../include/
CAIROFLAGS= -L${PREFIX}/lib -lX11 -lcairo
LDFLAGS=-L../lib/ -lgiza

all: test-giza-xw test-cairo-xw test-png test-pdf

test-giza-xw: test-xw.o
	$(CC) -o $@ $< $(LDFLAGS)

test-cairo-xw: test-cairo-xw.o
	$(CC) -o $@ $< $(CAIROFLAGS)

test-png: test-png.o
	$(CC) -o $@ $< $(LDFLAGS)

test-pdf: test-pdf.o
	$(CC) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.o test-giza-xw test-cairo-xw test-png test-pdf
