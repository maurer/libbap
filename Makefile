all: _build/clib/libbap.a _build/clib/libbap.pc
# OASIS_START
# OASIS_STOP
_build/clib:
	mkdir -p _build/clib

_build/clib/libbap.a: _build/clib build setup.data
	ocamlopt -output-obj _build/lib/libbap.cmxa -o _build/clib/bap_ocaml.o
	cp _build/lib/liblibbap_stubs.a _build/clib/bap_c.a
	cd _build/clib && ar x bap_c.a
	cd _build/clib && ar rcT libbap.a *.o `ocamlc -where`/libasmrun.a
	ranlib _build/clib/libbap.a

_build/clib/libbap.pc: _build/clib setup.data
	./make_pc.bash > $@

_build/clib/libbap.so: _build/clib build setup.data
	ocamlopt -output-obj _build/lib/libbap.cmxa -ccopt -L_build/lib/ -o _build/clib/libbap.so

install: _build/clib/libbap.a _build/clib/libbap.pc _build/clib/libbap.so
	source setup.data && mkdir -p $$libdir
	source setup.data && cp $^ $$libdir
