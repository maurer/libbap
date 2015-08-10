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

typedef bitvector bap_addr;

//C-mode types
typedef enum {
  BAP_AARCH64,
  BAP_AARCH64_BE,
  BAP_ARM,
  BAP_ARMEB,
  BAP_ARMV4,
  BAP_ARMV4T,
  BAP_ARMV5,
  BAP_ARMV6,
  BAP_ARMV7,
  BAP_HEXAGON,
  BAP_MIPS,
  BAP_MIPS64,
  BAP_MIPS64EL,
  BAP_MIPSEL,
  BAP_NVPTX,
  BAP_NVPTX64,
  BAP_PPC,
  BAP_PPC64,
  BAP_PPC64LE,
  BAP_R600,
  BAP_SPARC,
  BAP_SPARCV9,
  BAP_SYSTEMZ,
  BAP_THUMB,
  BAP_THUMBEB,
  BAP_X86,
  BAP_X86_64,
  BAP_XCORE
} bap_arch;

typedef enum {
  BAP_LITTLE_ENDIAN,
  BAP_BIG_ENDIAN
} bap_endian;

#define Val_endian(x) Val_int(x)
#define Endian_val(x) Int_val(x)

// Load ocaml funcs
#define NAMED_FUNC(name) \
  static value *caml_##name = NULL;
#define LOAD_FUNC(name) \
  caml_##name = caml_named_value(#name);\
  assert(caml_##name != NULL);\
  caml_register_global_root(caml_##name);

bitvector bitvector_of_int64(int64_t val, int8_t width);
char*     bitvector_to_string(bitvector);
bigstring create_bigstring(off_t start, size_t len, char* buf);
size_t    bigstring_to_buf(bigstring, char*, size_t);
