#ifndef __BAP_INTERNAL_H
#define __BAP_INTERNAL_H
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

//Abstract types
#define ABSTRACT_TYPE(name) \
  typedef value* bap_##name; \
  void inline bap_free_##name(bap_##name ptr) { \
    caml_remove_global_root(ptr); \
    free(ptr); \
  } \
  bap_##name inline bap_alloc_##name(value caml) { \
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
ABSTRACT_TYPE(bil)

// Load ocaml funcs
#define NAMED_FUNC(name) \
  static value *caml_##name = NULL;
#define LOAD_FUNC(name) \
  caml_##name = caml_named_value(#name);\
  assert(caml_##name != NULL);\
  caml_register_global_root(caml_##name);

#endif
