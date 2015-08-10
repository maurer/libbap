#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/alloc.h>
#include <bap.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

NAMED_FUNC(bigstring_of_string)
NAMED_FUNC(mem_create)
NAMED_FUNC(disassemble_mem)
NAMED_FUNC(bitvector_of_int64)
NAMED_FUNC(bitvector_to_string)
NAMED_FUNC(bigstring_to_string)
NAMED_FUNC(mem_to_string)

static char* argv[] = { NULL };

void libbap_init() {
  caml_startup(argv);
  LOAD_FUNC(bigstring_of_string)
  LOAD_FUNC(mem_create)
  LOAD_FUNC(disassemble_mem)
  LOAD_FUNC(bitvector_of_int64)
  LOAD_FUNC(bitvector_to_string)
  LOAD_FUNC(bigstring_to_string)
  LOAD_FUNC(mem_to_string)
}

bigstring create_bigstring(off_t pos, size_t len, char* buf) {
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

char* mem_to_string(mem mem) {
  return strdup(String_val(caml_callback(*caml_mem_to_string, *mem)));
}

size_t bigstring_to_buf(bigstring bv, char* buf, size_t buf_size) {
  value caml_buf = caml_callback(*caml_bigstring_to_string, *bv);
  size_t caml_buf_len = caml_string_length(caml_buf);
  size_t out_len = (buf_size < caml_buf_len) ? buf_size : caml_buf_len;
  memcpy(buf, String_val(caml_buf), out_len);
  return out_len;
}

mem create_mem(off_t pos, size_t len, bap_endian endian, bap_addr addr,
		bigstring buf) {
  value args[] = {Val_int(pos), Val_int(len), endian, *addr, *buf};
  return alloc_mem(caml_callbackN(*caml_mem_create, 5, args));
}

disasm disasm_mem(bap_addr* roots, size_t num_addrs, bap_arch arch, mem mem) {
  //TODO actually load roots
  value roots_list = Val_emptylist;
  return alloc_disasm(caml_callback3(*caml_disassemble_mem,
                                     roots_list, arch, *mem));
}
