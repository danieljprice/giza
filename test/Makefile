CC=gcc
CFLAGS=-I/usr/X11R6/include
CAIROFLAGS=-L/usr/X11R6/lib -lcairo -lX11

test-cairo-xw: test-cairo-xw.o
	$(CC) -o $@ $< $(CAIROFLAGS)

clean:
	rm -f *.o
