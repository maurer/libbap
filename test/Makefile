CFLAGS+=-I../lib/
LIBDIR=$(shell realpath ../_build/clib)
LDFLAGS+=-L$(LIBDIR) -Wl,-rpath,$(LIBDIR)
LDLIBS=-lbap

all: test

test: bin.out bin.truth
	diff -a $^

bin.out: bin
	./$< | tee $@
