# $Id$

CC := cc
CFLAGS := -g -std=c99

.PHONY: all clean

all: ./libppm.so ./libppm.a ./demo

./libppm.so: ./ppmlib.c ./ppmlib.h
	$(CC) -shared -fPIC $(CFLAGS) -o ./libppm.so ppmlib.c

./libppm.a: ./ppmlib.c ./ppmlib.h
	$(CC) $(CFLAGS) -c -o ./ppmlib.o ppmlib.c
	ar rcs ./libppm.a ./ppmlib.o

./demo: ./libppm.a ./demo.c
	$(CC) $(CFLAGS) `pkg-config --cflags xaw7` -o ./demo ./demo.c ./libppm.a `pkg-config --libs xaw7`

clean:
	rm -f ./libppm.so ./libppm.a ./ppmlib.o
