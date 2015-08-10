CFLAGS=-I../lib/
LIBDIR=../_build/clib
LDFLAGS=-L$(LIBDIR) -lbap -Wl,-rpath,$(LIBDIR)

all: test

test: bin.out bin.truth
	diff -a $^

bin.out: bin
	./$< > $@
