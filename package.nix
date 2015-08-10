{stdenv, ocaml, findlib, camlp4, ocaml_oasis, core_kernel, bap, ncurses,
# XXX These should not be required, and are here to deal with deps bugs
faillib, pa_ounit, pa_test, camlzip, ocamlgraph, bitstring, zarith, uuidm,
fileutils}:

stdenv.mkDerivation rec {
  name = "libbap";
  version = "0";
  src = ./.;

  buildInputs = [ ocaml findlib camlp4 ocaml_oasis core_kernel bap ncurses
  # XXX These should not be required, and are here to deal with deps bugs
                  faillib pa_ounit pa_test camlzip ocamlgraph bitstring
                  zarith uuidm fileutils ];
  preConfigure = "oasis setup";
  doCheck = true;
  meta = with stdenv.lib; {
    description = "C Bindings for BAP";
    license = licenses.mit;
  };
}
