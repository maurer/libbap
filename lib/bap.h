#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

//Abstract types
#define ABSTRACT_TYPE(name) \
  typedef value* bap_##name; \
  void static inline bap_free_##name(bap_##name ptr) { \
    caml_remove_global_root(ptr); \
    free(ptr); \
  } \
  bap_##name static inline bap_alloc_##name(value caml) { \
    bap_##name out = malloc(sizeof(value)); \
    *out = caml; \
    caml_register_global_root(out); \
    return out; \
  }

ABSTRACT_TYPE(disasm)
ABSTRACT_TYPE(mem)
ABSTRACT_TYPE(bigstring)
ABSTRACT_TYPE(bitvector)
ABSTRACT_TYPE(insn)

typedef bap_bitvector bap_addr;

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
  bap_insn insn;
} bap_disasm_insn;

void free_disasm_insn(bap_disasm_insn*);

// Load ocaml funcs
#define NAMED_FUNC(name) \
  static value *caml_##name = NULL;
#define LOAD_FUNC(name) \
  caml_##name = caml_named_value(#name);\
  assert(caml_##name != NULL);\
  caml_register_global_root(caml_##name);

// String rendering
char*     bap_disasm_to_string(bap_disasm);
char*     bap_bitvector_to_string(bap_bitvector);
char*     bap_mem_to_string(bap_mem);
char*     bap_insn_to_asm(bap_insn);

// Projection (convert abstract types to C-native ones)
// Determine length of a bigstring, e.g. for use with bap_bigstring_to_buf.
size_t    bap_bigstring_length(bap_bigstring);

// Load bigstring back to a native C buffer. Returns bytes copied.
size_t    bap_bigstring_to_buf(bap_bigstring, // Bigstring to project
                               char*,         // Buffer to land in
                               size_t);       // Size of provided buffer

// Construction
// Creates an bitvector holding the provided value and of the given width.
// The width is given in bits.
// If the width is shorter than 64, the value will be truncated.
// If the width is wider than 64, upper bits are assumed zero.
bap_bitvector bap_create_bitvector64(int64_t val, int16_t width);

// Create a bigstring from a native C buffer.
bap_bigstring bap_create_bigstring(char* buf, size_t len);

// Create a memory region from a bigstring buffer
bap_mem       bap_create_mem(off_t pos,          // Starting index in the buf
                             size_t len,         // Number of bytes to put in
                             bap_endian endian,  // Endiannes of the memory
                             bap_addr addr,      // Region base address
                             bap_bigstring buf); // Backing buffer.

// Disassembly
// Performs recursive disassembly from each provided root
bap_disasm bap_disasm_mem(bap_addr* roots, // NULL terminated array of roots
                                           // If NULL, base of mem is used.
                          bap_arch arch,   // Arch to disassemble as
                          bap_mem mem);    // Memory region to disassemble

// Acquire the instructions from a disassembly as a null terminated array
bap_disasm_insn** bap_disasm_get_insns(bap_disasm d);
