#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/alloc.h>

static value *caml_bigstring_of_string = NULL;

static char* argv[] = { NULL };

void libbap_init() {
  caml_startup(argv);
  caml_bigstring_of_string = caml_named_value("bigstring_of_string");
}

value* bigstring_of_buf(char* buf, size_t len) {
  return caml_callback3(*caml_bigstring_of_string,
		        Val_int(0), Val_int(len),
			// This is silly, since it's copying C -> OCaml -> C
			// but I don't see a bigstring_of_cstring, so...
			caml_copy_string(buf));
}
