#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef __BAP_INTERNAL_H
//Abstract types
#define ABSTRACT_TYPE(name) \
  typedef void* bap_##name; \
  void inline bap_free_##name(bap_##name);

ABSTRACT_TYPE(disasm)
ABSTRACT_TYPE(mem)
ABSTRACT_TYPE(bigstring)
ABSTRACT_TYPE(bitvector)
ABSTRACT_TYPE(insn)
#endif

typedef bap_bitvector bap_addr;

//C-mode types
typedef enum bap_arch {
  BAP_ARM    = 0x93fcb9,
  BAP_X86    = 0xb67eed,
  BAP_X86_64 = 0xba7b274f,
} bap_arch;

typedef enum bap_endian {
  BAP_LITTLE_ENDIAN = 1,
  BAP_BIG_ENDIAN = 3
} bap_endian;

typedef struct {
  bap_addr start;
  bap_addr end;
  bap_insn insn;
} bap_disasm_insn;

typedef enum bap_type_kind {
  BAP_TYPE_IMM,
  BAP_TYPE_MEM
} bap_type_kind;

typedef struct {
  char* name;
  bool r, w, x;
  bap_mem mem;
} bap_segment;

typedef struct {
  bap_addr start;
  bap_addr end;
  char* data;
  size_t len;
} bap_mem_c;

typedef struct {
  char* name;
  bool func;
  bool debug;
  bap_addr start;
  bap_addr end;
} bap_symbol;

// All sizes are in bits. All of them. Always.

typedef struct {
  bap_type_kind kind;
  union {
    int imm;
    struct {
      int addr_size;
      int cell_size;
    } mem;
  };
} bap_type;

typedef struct {
  char* name;
  bap_type* type;
  bool tmp;
  int version;
} bap_var;

typedef enum bap_binop {
  BAP_BINOP_PLUS,
  BAP_BINOP_MINUS,
  BAP_BINOP_TIMES,
  BAP_BINOP_DIVIDE,
  BAP_BINOP_SDIVIDE,
  BAP_BINOP_MOD,
  BAP_BINOP_SMOD,
  BAP_BINOP_LSHIFT,
  BAP_BINOP_RSHIFT,
  BAP_BINOP_ARSHIFT,
  BAP_BINOP_AND,
  BAP_BINOP_OR,
  BAP_BINOP_XOR,
  BAP_BINOP_EQ,
  BAP_BINOP_NEQ,
  BAP_BINOP_LT,
  BAP_BINOP_LE,
  BAP_BINOP_SLT,
  BAP_BINOP_SLE
} bap_binop;

typedef enum bap_unop {
  BAP_UNOP_NEG,
  BAP_UNOP_NOT
} bap_unop;

typedef enum bap_expr_kind {
  BAP_EXPR_LOAD,
  BAP_EXPR_STORE,
  BAP_EXPR_BINOP,
  BAP_EXPR_UNOP,
  BAP_EXPR_VAR,
  BAP_EXPR_IMM,
  BAP_EXPR_CAST,
  BAP_EXPR_LET,
  BAP_EXPR_UNK,
  BAP_EXPR_ITE,
  BAP_EXPR_EXTRACT,
  BAP_EXPR_CONCAT
} bap_expr_kind;

typedef enum bap_cast_kind {
  BAP_CAST_UNSIGNED,
  BAP_CAST_SIGNED,
  BAP_CAST_HIGH,
  BAP_CAST_LOW,
} bap_cast_type;

typedef struct bap_expr {
  bap_expr_kind kind;
  union {
    struct {
      struct bap_expr *memory;
      struct bap_expr *index;
      bap_endian endian;
      int size;
    } load;
    struct {
      struct bap_expr *memory;
      struct bap_expr *index;
      struct bap_expr *value;
      bap_endian endian;
      int size;
    } store;
    struct {
      bap_binop op;
      struct bap_expr *lhs, *rhs;
    } binop;
    struct {
      bap_unop op;
      struct bap_expr *arg;
    } unop;
    bap_var *var;
    bap_bitvector imm;
    struct {
      bap_cast_type type;
      int width;
      struct bap_expr *val;
    } cast;
    struct {
      bap_var *bound_var;
      struct bap_expr *bound_expr;
      struct bap_expr *body_expr;
    } let;
    struct {
      char* descr;
      bap_type *type;
    } unknown;
    struct {
      struct bap_expr *cond;
      struct bap_expr *t;
      struct bap_expr *f;
    } ite;
    struct {
      int low_bit;
      int high_bit;
      struct bap_expr *val;
    } extract;
    struct {
      struct bap_expr *low, *high;
    } concat;
  };
} bap_expr;

typedef enum bap_stmt_kind {
  BAP_STMT_MOVE,
  BAP_STMT_JMP,
  BAP_STMT_SPECIAL,
  BAP_STMT_WHILE,
  BAP_STMT_IF,
  BAP_STMT_CPU_EXN
} bap_stmt_kind;

typedef struct bap_stmt {
  bap_stmt_kind kind;
  union {
    struct {
      bap_var  *lhs;
      bap_expr *rhs;
    } move;
    bap_expr *jmp;
    char *special;
    struct {
      bap_expr *cond;
      struct bap_stmt **body;
    } s_while;
    struct {
      bap_expr *cond;
      struct bap_stmt **t;
      struct bap_stmt **f;
    } ite;
    int cpu_exn;
  };
} bap_stmt;

void bap_free_symbol(bap_symbol*);
void bap_free_disasm_insn(bap_disasm_insn*);
void bap_free_segment(bap_segment*);

// String rendering
char*     bap_render_stmt(bap_stmt*);
char*     bap_disasm_to_string(bap_disasm);
char*     bap_bitvector_to_string(bap_bitvector);
char*     bap_mem_to_string(bap_mem);
char*     bap_insn_to_asm(bap_insn);
char*     bap_render_segment(bap_segment* seg);

// Projection (convert abstract types to C-native ones)
// Determine length of a bigstring, e.g. for use with bap_bigstring_to_buf.
size_t    bap_bigstring_length(bap_bigstring);

// Load bigstring back to a native C buffer. Returns bytes copied.
size_t    bap_bigstring_to_buf(bap_bigstring, // Bigstring to project
                               char*,         // Buffer to land in
                               size_t);       // Size of provided buffer
size_t bap_bitvector_size(bap_bitvector bv);
char* bap_bitvector_contents(bap_bitvector bv);

// Project a mem object into its data and location components
bap_mem_c* bap_project_mem(bap_mem);

// Construction
// Creates an bitvector holding the provided value and of the given width.
// The width is given in bits.
// If the width is shorter than 64, the value will be truncated.
// If the width is wider than 64, upper bits are assumed zero.
bap_bitvector bap_create_bitvector64(int64_t val, int16_t width);

// Create a bigstring from a native C buffer.
bap_bigstring bap_create_bigstring(char* buf, size_t len);

// Create a memory region from a bigstring buffer
bap_mem       bap_create_mem(size_t pos,          // Starting index in the buf
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

// Acquire the BIL description from an instruction as a null terminated array
// Returning null indicates that the lift failed
bap_stmt** bap_insn_get_stmts(bap_insn i);

// Container functions
bap_segment** bap_get_segments(char* buf, size_t len);
bap_symbol** bap_get_symbols(char* buf, size_t len);
bap_addr* bap_byteweight(bap_arch arch, bap_mem mem);

// Library Lifecycle

// Free native pointers
void bap_free(void*);

// Initialize library
void bap_init();

// Thread-safety
bool bap_thread_register();
bool bap_thread_unregister();

void bap_acquire();
void bap_release();
