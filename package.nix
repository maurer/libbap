{stdenv, buildOcaml, camlp4, ocaml_oasis, core_kernel, bap,
# XXX These should not be required, and are here to deal with deps bugs
faillib, pa_ounit, pa_test, camlzip, ocamlgraph, bitstring, zarith, uuidm,
fileutils}:

buildOcaml rec {
  name = "libbap";
  version = "0";
  src = ./.;

  createFindlibDestdir = true;
  buildInputs = [ camlp4 ocaml_oasis core_kernel bap
  # XXX These should not be required, and are here to deal with deps bugs
                  faillib pa_ounit pa_test camlzip ocamlgraph bitstring
                  zarith uuidm fileutils ];
  preConfigure = "oasis setup";
  meta = with stdenv.lib; {
    description = "C Bindings for BAP";
    license = licenses.mit;
  };
}
