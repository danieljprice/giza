CC=gcc
CFLAGS=-I/usr/X11R6/include -I../include/
CAIROFLAGS=-L/usr/X11R6/lib -lcairo -lX11
LDFLAGS=$(CAIROFLAGS) -L../lib/ -lgiza

test-giza-xw: test-xw.o
	$(CC) -o $@ $< $(LDFLAGS)

test-cairo-xw: test-cairo-xw.o
	$(CC) -o $@ $< $(CAIROFLAGS)

clean:
	rm -f *.o
