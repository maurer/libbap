all: _build/clib/libbap.so
# OASIS_START
# DO NOT EDIT (digest: a1e6d63ae0b07166f3cd405919bd43be)

SETUP = ocaml setup.ml

build: setup.data
	$(SETUP) -build $(BUILDFLAGS)

doc: setup.data build
	$(SETUP) -doc $(DOCFLAGS)

test: setup.data build
	$(SETUP) -test $(TESTFLAGS)

all:
	$(SETUP) -all $(ALLFLAGS)

uninstall: setup.data
	$(SETUP) -uninstall $(UNINSTALLFLAGS)

reinstall: setup.data
	$(SETUP) -reinstall $(REINSTALLFLAGS)

clean:
	$(SETUP) -clean $(CLEANFLAGS)

distclean:
	$(SETUP) -distclean $(DISTCLEANFLAGS)

setup.data:
	$(SETUP) -configure $(CONFIGUREFLAGS)

configure:
	$(SETUP) -configure $(CONFIGUREFLAGS)

.PHONY: build doc test all uninstall reinstall clean distclean configure

# OASIS_STOP
DEP_PACKAGES = bap bap-byteweight core_kernel threads findlib.dynload
DEP_PACKAGES_FLAGS = $(foreach p, $(DEP_PACKAGES), -package $p)

_build/clib:
	mkdir -p _build/clib

_build/clib/liblibbap_stubs.a: build
	cp _build/lib/liblibbap_stubs.a $@

_build/clib/libbap.so: build _build/clib _build/clib/liblibbap_stubs.a export.map
	cd _build/clib && ocamlfind ocamlopt -predicates used_bap_byteweight -thread -linkpkg -linkall -runtime-variant _pic -output-obj ../lib/*.cmx -ccopt -Wl,--whole-archive -ccopt liblibbap_stubs.a -ccopt -Wl,--no-whole-archive -o libbap.so $(DEP_PACKAGES_FLAGS)
#	cd _build/clib && ocamlfind ocamlopt -thread -linkpkg -linkall -runtime-variant _pic -output-obj ../lib/*.cmx -ccopt -Wl,--whole-archive -ccopt liblibbap_stubs.a -ccopt -Wl,--no-whole-archive,--version-script=$(CURDIR)/export.map -o libbap.so $(DEP_PACKAGES_FLAGS)

install: _build/clib/libbap.so lib/bap.h
	bash -c 'source setup.data && mkdir -p $$libdir'
	bash -c 'source setup.data && cp _build/clib/libbap.so $$libdir'
	bash -c 'source setup.data && mkdir -p $$prefix/include'
	bash -c 'source setup.data && cp lib/bap.h $$prefix/include'

check: all
	make -C test
