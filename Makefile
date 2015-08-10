all: _build/clib/libbap.so
# OASIS_START
# OASIS_STOP
DEP_PACKAGES = bap core_kernel
DEP_PACKAGES_FLAGS = $(foreach p, $(DEP_PACKAGES), -package $p)

_build/clib:
	mkdir -p _build/clib

_build/clib/libbap.so: build _build/clib
	ocamlfind ocamlopt -linkpkg -output-obj _build/lib/*.cmx -ccopt -L_build/lib/ -o _build/clib/libbap.so $(DEP_PACKAGES_FLAGS)

install: _build/clib/libbap.so
	source setup.data && mkdir -p $$libdir
	source setup.data && cp $^ $$libdir
