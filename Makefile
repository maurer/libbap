all: _build/clib/libbap.so
# OASIS_START
# OASIS_STOP
DEP_PACKAGES = bap core_kernel
DEP_PACKAGES_FLAGS = $(foreach p, $(DEP_PACKAGES), -package $p)

_build/clib:
	mkdir -p _build/clib

_build/clib/liblibbap_stubs.a: build
	cp _build/lib/liblibbap_stubs.a $@

_build/clib/libbap.so: build _build/clib _build/clib/liblibbap_stubs.a
	cd _build/clib && ocamlfind ocamlopt -linkpkg -output-obj ../lib/*.cmx -ccopt -Wl,--whole-archive -ccopt liblibbap_stubs.a -ccopt -Wl,--no-whole-archive -o libbap.so $(DEP_PACKAGES_FLAGS)

install: _build/clib/libbap.so lib/bap.h
	source setup.data && mkdir -p $$libdir
	source setup.data && cp _build/clib/libbap.so $$libdir
	source setup.data && mkdir -p $$prefix/include
	source setup.data && cp lib/bap.h $$prefix/include

check: all
	make -C test
