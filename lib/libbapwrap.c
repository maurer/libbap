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
NAMED_FUNC(disasm_to_string)
NAMED_FUNC(disasm_to_insns)
NAMED_FUNC(insn_asm)

static char* argv[] = { NULL };

void bap_init() {
  caml_startup(argv);
  LOAD_FUNC(bigstring_of_string)
  LOAD_FUNC(mem_create)
  LOAD_FUNC(disassemble_mem)
  LOAD_FUNC(bitvector_of_int64)
  LOAD_FUNC(bitvector_to_string)
  LOAD_FUNC(bigstring_to_string)
  LOAD_FUNC(mem_to_string)
  LOAD_FUNC(disasm_to_string)
  LOAD_FUNC(disasm_to_insns)
  LOAD_FUNC(insn_asm)
}

bap_bigstring bap_create_bigstring(char* buf, size_t len) {
  value ocaml_string = caml_alloc_string(len);
  memcpy(String_val(ocaml_string), buf, len);

  return bap_alloc_bigstring(caml_callback3(*caml_bigstring_of_string,
                                            Val_int(0), Val_int(len),
                                            ocaml_string));
}

bap_bitvector bap_create_bitvector64(int64_t val, int16_t width) {
  return bap_alloc_bitvector(caml_callback2(*caml_bitvector_of_int64,
                                            Val_int(width),
                                            caml_copy_int64(val)));
}

char* bap_bitvector_to_string(bap_bitvector bv) {
  return strdup(String_val(caml_callback(*caml_bitvector_to_string, *bv)));
}

char* bap_mem_to_string(bap_mem mem) {
  return strdup(String_val(caml_callback(*caml_mem_to_string, *mem)));
}

char* bap_disasm_to_string(bap_disasm d) {
  return strdup(String_val(caml_callback(*caml_disasm_to_string, *d)));
}

size_t bap_bigstring_to_buf(bap_bigstring bv, char* buf, size_t buf_size) {
  value caml_buf = caml_callback(*caml_bigstring_to_string, *bv);
  size_t caml_buf_len = caml_string_length(caml_buf);
  size_t out_len = (buf_size < caml_buf_len) ? buf_size : caml_buf_len;
  memcpy(buf, String_val(caml_buf), out_len);
  return out_len;
}

bap_mem bap_create_mem(off_t pos, size_t len, bap_endian endian, bap_addr addr,
                       bap_bigstring buf) {
  value args[] = {Val_int(pos), Val_int(len), endian, *addr, *buf};
  return bap_alloc_mem(caml_callbackN(*caml_mem_create, 5, args));
}

static const char* dummy_roots[] = {NULL};

static value id(const char* v) {
  return (value)v;
}

bap_disasm bap_disasm_mem(bap_addr* roots, bap_arch arch, bap_mem mem) {
  value roots_arr;
  if (roots) {
    //XXX: Something's fishy about caml_alloc_array and const
    roots_arr = caml_alloc_array(id, (const char**)roots);
  } else {
    roots_arr = caml_alloc_array(id, dummy_roots);
  }
  return bap_alloc_disasm(caml_callback3(*caml_disassemble_mem,
                                         roots_arr, arch, *mem));
}

void bap_free_disasm_insn(bap_disasm_insn *di) {
  bap_free_bitvector(di->start);
  bap_free_bitvector(di->end);
  bap_free_insn(di->insn);
  free(di);
}

bap_disasm_insn** bap_disasm_get_insns(bap_disasm d) {
  value caml_arr = caml_callback(*caml_disasm_to_insns, *d);
  size_t arr_len = caml_array_length(caml_arr);
  bap_disasm_insn** out = malloc(sizeof(bap_disasm_insn*) * (arr_len + 1));
  size_t i;
  for (i = 0; i < arr_len; i++) {
    out[i] = malloc(sizeof(bap_disasm_insn));
    value entry = Field(caml_arr, i);
    out[i]->start = bap_alloc_bitvector(Field(entry, 0));
    out[i]->end   = bap_alloc_bitvector(Field(entry, 1));
    out[i]->insn  = bap_alloc_insn(Field(entry, 2));
  }
  out[arr_len] = NULL;
  return out;
}

char* bap_insn_to_asm(bap_insn i) {
  return strdup(String_val(caml_callback(*caml_insn_asm, *i)));
}
