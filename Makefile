all: _build/clib/libbap.so _build/clib/dlllibbap_stubs.so
# OASIS_START
# OASIS_STOP
DEP_PACKAGES = bap core_kernel
DEP_PACKAGES_FLAGS = $(foreach p, $(DEP_PACKAGES), -package $p)

_build/clib:
	mkdir -p _build/clib

_build/clib/dlllibbap_stubs.so: build
	cp _build/lib/dlllibbap_stubs.so $@

_build/clib/libbap.so: build _build/clib _build/clib/dlllibbap_stubs.so
	cd _build/clib && ocamlfind ocamlopt -linkpkg -output-obj ../lib/*.cmx -ccopt dlllibbap_stubs.so -o libbap.so $(DEP_PACKAGES_FLAGS)

SOS = libbap dlllibbap_stubs
SOS_FILES = $(foreach so, $(SOS), _build/clib/$(so).so)

install: $(SOS_FILES) lib/bap.h
	source setup.data && mkdir -p $$libdir
	source setup.data && cp $(SOS_FILES) $$libdir
	source setup.data && mkdir -p $$prefix/include
	source setup.data && cp lib/bap.h $$prefix/include
