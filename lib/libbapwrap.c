#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/alloc.h>
#include <bap.h>
#include <string.h>
#include <assert.h>

NAMED_FUNC(bigstring_of_string)
NAMED_FUNC(mem_create)
NAMED_FUNC(disassemble_mem)
NAMED_FUNC(bitvector_of_int64)
NAMED_FUNC(bitvector_to_string)

static char* argv[] = { NULL };

void libbap_init() {
  caml_startup(argv);
  LOAD_FUNC(bigstring_of_string)
  LOAD_FUNC(mem_create)
  LOAD_FUNC(disassemble_mem)
  LOAD_FUNC(bitvector_of_int64)
  LOAD_FUNC(bitvector_to_string)
}

bigstring create_bigstring(char* buf, size_t len, off_t pos) {
  value ocaml_string = caml_alloc_string(len);
  memcpy(String_val(ocaml_string), buf, len);

  return alloc_bigstring(caml_callback3(*caml_bigstring_of_string,
                                        Val_int(pos), Val_int(len),
                                        ocaml_string));
}

bitvector bitvector_of_int64(int64_t val, int8_t width) {
  return alloc_bitvector(caml_callback2(*caml_bitvector_of_int64,
                                        Val_int(width),
					caml_copy_int64(val)));
}

char* bitvector_to_string(bitvector bv) {
  return strdup(String_val(caml_callback(*caml_bitvector_to_string, *bv)));
}

/*
mem create_mem(off_t pos, size_t len, bap_endian endian, bap_addr addr,
		bigstring buf) {
  return alloc_mem(caml_callback5(*caml_mem_create,
			          Val_int(pos), Val_int(len),
				  Val_endian(endian), *addr,
				  *buf));
}
*/

disasm disasm_mem(bap_addr* roots, size_t num_addrs, bap_arch arch, mem mem) {
  //TODO actually load roots
  value roots_list = Val_emptylist;
  return alloc_disasm(caml_callback3(*caml_disassemble_mem,
                                     roots_list, arch, *mem));
}
