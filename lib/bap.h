#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>

#include <stdint.h>
#include <assert.h>

//Abstract types
#define ABSTRACT_TYPE(name) \
  typedef value* name; \
  void static inline free_##name(name ptr) { \
    caml_remove_global_root(ptr); \
    free(ptr); \
  } \
  name static inline alloc_##name(value caml) { \
    name out = malloc(sizeof(value)); \
    *out = caml; \
    caml_register_global_root(out); \
    return out; \
  }

ABSTRACT_TYPE(disasm)
ABSTRACT_TYPE(mem)
ABSTRACT_TYPE(bigstring)
ABSTRACT_TYPE(bitvector)
ABSTRACT_TYPE(insn)

typedef bitvector bap_addr;

//C-mode types
typedef enum {
  BAP_ARM = 0x93fcb9,
  BAP_X86 = 0xb67eed,
  BAP_X86_64 = 0xba7b274f,
} bap_arch;

typedef enum {
  BAP_LITTLE_ENDIAN = Val_int(0),
  BAP_BIG_ENDIAN = Val_int(1)
} bap_endian;

typedef struct {
  bap_addr start;
  bap_addr end;
  insn insn;
} bap_disasm_insn;

void free_disasm_insn(bap_disasm_insn*);

// Load ocaml funcs
#define NAMED_FUNC(name) \
  static value *caml_##name = NULL;
#define LOAD_FUNC(name) \
  caml_##name = caml_named_value(#name);\
  assert(caml_##name != NULL);\
  caml_register_global_root(caml_##name);
char*     disasm_to_string(disasm);
bitvector bitvector_of_int64(int64_t val, int8_t width);
char*     bitvector_to_string(bitvector);
bigstring create_bigstring(off_t start, size_t len, char* buf);
size_t    bigstring_to_buf(bigstring, char*, size_t);
mem       create_mem(off_t pos, size_t len, bap_endian endian, bap_addr addr,
		     bigstring buf);
char*     mem_to_string(mem);
disasm disasm_mem(bap_addr* roots, bap_arch arch, mem mem);
bap_disasm_insn** disasm_insns(disasm d);
char*     insn_to_asm(insn);
