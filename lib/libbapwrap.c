
//For asprintf
#define _GNU_SOURCE

#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/alloc.h>
#include <caml/threads.h>
#include <bap_internal.h>
#include <bap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

NAMED_FUNC(get_symbols)
NAMED_FUNC(byteweight)
NAMED_FUNC(get_segments)
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
NAMED_FUNC(insn_to_bils)
NAMED_FUNC(array_of_list)
NAMED_FUNC(size_to_bits)
NAMED_FUNC(bv_size)
NAMED_FUNC(bv_contents)

static char* argv[] = { NULL };

void bap_init() {
  caml_startup(argv);
  LOAD_FUNC(get_symbols)
  LOAD_FUNC(byteweight)
  LOAD_FUNC(get_segments)
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
  LOAD_FUNC(insn_to_bils)
  LOAD_FUNC(array_of_list)
  LOAD_FUNC(size_to_bits)
  LOAD_FUNC(bv_size)
  LOAD_FUNC(bv_contents)
}

bap_symbol** bap_get_symbols(char* buf, size_t len) {
  value ocaml_buf = caml_alloc_string(len);
  memcpy(String_val(ocaml_buf), buf, len);
  value caml_syms = caml_callback(*caml_get_symbols, ocaml_buf);
  size_t sym_len = caml_array_length(caml_syms);
  bap_symbol** out = malloc(sizeof(bap_symbol*) * (len + 1));
  size_t i;
  for (i = 0; i < sym_len; i++) {
    value cur = Field(caml_syms, i);
    out[i] = malloc(sizeof(bap_symbol));
    out[i]->name  = strdup(String_val(Field(cur, 0)));
    out[i]->func  = Bool_val(Field(cur, 1));
    out[i]->debug = Bool_val(Field(cur, 2));
    out[i]->start = bap_alloc_bitvector(Field(cur, 3));
    out[i]->end   = bap_alloc_bitvector(Field(cur, 4));
  }
  out[i] = NULL;
  return out;
}

bap_addr* bap_byteweight(bap_arch arch, bap_mem mem) {
  value caml_addrs = caml_callback2(*caml_byteweight, arch, *mem);
  size_t addr_count = caml_array_length(caml_addrs);
  bap_addr* out = malloc(sizeof(bap_addr) * (addr_count + 1));
  size_t i;
  for (i = 0; i < addr_count; i++) {
    out[i] = bap_alloc_bitvector(Field(caml_addrs, i));
  }
  out[i] = 0;
  return out;
}

bap_segment** bap_get_segments(char* buf, size_t len) {
  value ocaml_buf = caml_alloc_string(len);
  memcpy(String_val(ocaml_buf), buf, len);
  value caml_segs = caml_callback(*caml_get_segments, ocaml_buf);
  size_t seg_len = caml_array_length(caml_segs);
  bap_segment** out = malloc(sizeof(bap_segment*) * (len + 1));
  size_t i;
  for (i = 0; i < seg_len; i++) {
    out[i] = malloc(sizeof(bap_segment));
    value entry = Field(caml_segs, i);
    value lhs = Field(entry, 0);
    out[i]->name = strdup(String_val(Field(lhs, 0)));
    out[i]->r    = Bool_val(Field(lhs, 1));
    out[i]->w    = Bool_val(Field(lhs, 2));
    out[i]->x    = Bool_val(Field(lhs, 3));
    out[i]->mem  = bap_alloc_mem(Field(entry, 1));
  }
  out[i] = NULL;
  return out;
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

size_t bap_bitvector_size(bap_bitvector bv) {
  return Int_val(caml_callback(*caml_bv_size, *bv));
}

char* bap_bitvector_contents(bap_bitvector bv) {
  CAMLparam0 ();
  CAMLlocal1(caml_arr);
  caml_arr = caml_callback(*caml_bv_contents, *bv);
  size_t arr_len = caml_array_length(caml_arr);
  char* out = malloc(sizeof(char) * arr_len);
  size_t i;
  for (i = 0; i < arr_len; i++) {
    out[i] = Field(caml_arr, i);
  }
  CAMLreturnT(char*, out);
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

bap_mem bap_create_mem(size_t pos, size_t len, bap_endian endian, bap_addr addr,
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

void bap_free_segment(bap_segment *seg) {
  bap_free_mem(seg->mem);
  free(seg->name);
  free(seg);
}

void bap_free_symbol(bap_symbol *sym) {
  bap_free_bitvector(sym->start);
  bap_free_bitvector(sym->end);
  free(sym->name);
  free(sym);
}

bap_disasm_insn** bap_disasm_get_insns(bap_disasm d) {
  CAMLparam0 ();
  CAMLlocal1(caml_arr);
  caml_arr = caml_callback(*caml_disasm_to_insns, *d);
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
  CAMLreturnT(bap_disasm_insn**, out);
}

char* bap_insn_to_asm(bap_insn i) {
  return strdup(String_val(caml_callback(*caml_insn_asm, *i)));
}

static bap_stmt* project_bil_stmt(value);
static bap_var* project_bap_var(value);
static bap_expr* project_bap_expr(value);
static bap_stmt** project_bil_list_stmts(value);
static bap_type* project_bap_type(value);
static int project_bap_size(value);

static int project_bap_size(value abs_sz) {
  return Int_val(caml_callback(*caml_size_to_bits, abs_sz));
}

static bap_type* project_bap_type(value abs_type) {
  bap_type* type = malloc(sizeof(bap_type));
  type->kind = Tag_val(abs_type);
  switch (type->kind) {
    case BAP_TYPE_IMM:
      type->imm = Int_val(Field(abs_type, 0));
      break;
    case BAP_TYPE_MEM:
      type->mem.addr_size = project_bap_size(Field(abs_type, 0));
      type->mem.cell_size = project_bap_size(Field(abs_type, 1));
      break;
    default:
      assert(!"Unknown type kind");
  }
  return type;
}

static bap_var* project_bap_var(value abs_var) {
  bap_var* var = malloc(sizeof(bap_var));
  var->name = strdup(String_val(Field(abs_var, 0)));
  var->version = Int_val(Field(abs_var, 1));
  var->type = project_bap_type(Field(abs_var, 2));
  var->tmp = Bool_val(Field(abs_var, 3));
  return var;
}

static bap_expr* project_bap_expr(value abs_expr) {
  bap_expr* expr = malloc(sizeof(bap_expr));
  expr->kind = Tag_val(abs_expr);
  switch (expr->kind) {
    case BAP_EXPR_LOAD:
      expr->load.memory = project_bap_expr(Field(abs_expr, 0));
      expr->load.index = project_bap_expr(Field(abs_expr, 1));
      expr->load.endian = Field(abs_expr, 2);
      expr->load.size = project_bap_size(Field(abs_expr, 3));
      break;
    case BAP_EXPR_STORE:
      expr->store.memory = project_bap_expr(Field(abs_expr, 0));
      expr->store.index = project_bap_expr(Field(abs_expr, 1));
      expr->store.value = project_bap_expr(Field(abs_expr, 2));
      expr->store.endian = Field(abs_expr, 3);
      expr->store.size = project_bap_size(Field(abs_expr, 4));
      break;
    case BAP_EXPR_BINOP:
      expr->binop.op = Int_val(Field(abs_expr, 0));
      expr->binop.lhs = project_bap_expr(Field(abs_expr, 1));
      expr->binop.rhs = project_bap_expr(Field(abs_expr, 2));
      break;
    case BAP_EXPR_UNOP:
      expr->unop.op = Int_val(Field(abs_expr, 0));
      expr->unop.arg = project_bap_expr(Field(abs_expr, 1));
      break;
    case BAP_EXPR_VAR:
      expr->var = project_bap_var(Field(abs_expr, 0));
      break;
    case BAP_EXPR_IMM:
      expr->imm = bap_alloc_bitvector(Field(abs_expr, 0));
      break;
    case BAP_EXPR_CAST:
      expr->cast.type = Int_val(Field(abs_expr, 0));
      expr->cast.width = Int_val(Field(abs_expr, 1));
      expr->cast.val = project_bap_expr(Field(abs_expr, 2));
      break;
    case BAP_EXPR_LET:
      expr->let.bound_var = project_bap_var(Field(abs_expr, 0));
      expr->let.bound_expr = project_bap_expr(Field(abs_expr, 1));
      expr->let.body_expr = project_bap_expr(Field(abs_expr, 2));
      break;
    case BAP_EXPR_UNK:
      expr->unknown.descr = strdup(String_val(Field(abs_expr, 0)));
      expr->unknown.type = project_bap_type(Field(abs_expr, 1));
      break;
    case BAP_EXPR_ITE:
      expr->ite.cond = project_bap_expr(Field(abs_expr, 0));
      expr->ite.t = project_bap_expr(Field(abs_expr, 1));
      expr->ite.f = project_bap_expr(Field(abs_expr, 2));
      break;
    case BAP_EXPR_EXTRACT:
      expr->extract.low_bit = Int_val(Field(abs_expr, 0));
      expr->extract.high_bit = Int_val(Field(abs_expr, 1));
      expr->extract.val = project_bap_expr(Field(abs_expr, 2));
      break;
    case BAP_EXPR_CONCAT:
      expr->concat.low = project_bap_expr(Field(abs_expr, 0));
      expr->concat.high = project_bap_expr(Field(abs_expr, 1));
      break;
    default:
      assert(!"Unknown expression kind");
  }
  return expr;
}

static bap_stmt** project_bil_list_stmts(value bil_list) {
  CAMLparam0();
  CAMLlocal1(stmt_arr);
  stmt_arr = caml_callback(*caml_array_of_list, bil_list);
  size_t arr_len = caml_array_length(stmt_arr);
  bap_stmt** out = malloc(sizeof(bap_stmt*) * (arr_len + 1));
  size_t i;
  for (i = 0; i < arr_len; i++) {
    out[i] = project_bil_stmt(Field(stmt_arr, i));
  }
  out[arr_len] = NULL;
  CAMLreturnT(bap_stmt**, out);
}
static bap_stmt* project_bil_stmt(value bil) {
  bap_stmt* stmt = malloc(sizeof(bap_stmt));
  stmt->kind = Tag_val(bil);
  switch (stmt->kind) {
    case BAP_STMT_MOVE:
      stmt->move.lhs = project_bap_var(Field(bil, 0));
      stmt->move.rhs = project_bap_expr(Field(bil, 1));
      break;
    case BAP_STMT_JMP:
      stmt->jmp = project_bap_expr(Field(bil, 0));
      break;
    case BAP_STMT_SPECIAL:
      stmt->special = strdup(String_val(Field(bil, 0)));
      break;
    case BAP_STMT_WHILE:
      stmt->s_while.cond = project_bap_expr(Field(bil, 0));
      stmt->s_while.body = project_bil_list_stmts(Field(bil, 1));
      break;
    case BAP_STMT_IF:
      stmt->ite.cond = project_bap_expr(Field(bil, 0));
      stmt->ite.t = project_bil_list_stmts(Field(bil, 1));
      stmt->ite.f = project_bil_list_stmts(Field(bil, 2));
      break;
    case BAP_STMT_CPU_EXN:
      stmt->cpu_exn = Int_val(Field(bil, 0));
      break;
    default:
      assert(!"Unknown statement kind");
  }
  return stmt;
}

bap_stmt** bap_insn_get_stmts(bap_insn insn) {
  CAMLparam0 ();
  CAMLlocal1(bils);
  bils = caml_callback_exn(*caml_insn_to_bils, *insn);
  if (Is_exception_result(bils)) {
    CAMLreturnT(bap_stmt**, NULL);
  }
  CAMLreturnT(bap_stmt**, project_bil_list_stmts(bils));
}

const char* bap_str_endian(bap_endian e) {
  switch (e) {
    case BAP_LITTLE_ENDIAN:
      return "el";
    case BAP_BIG_ENDIAN:
      return "be";
    default:
      assert(!"Endian must be big or little");
      return "??";
  }
}

const char* bap_str_binop(bap_binop op) {
  switch (op) {
    case BAP_BINOP_PLUS:
      return "+";
    case BAP_BINOP_MINUS:
      return "-";
    case BAP_BINOP_TIMES:
      return "*";
    case BAP_BINOP_DIVIDE:
      return "/";
    case BAP_BINOP_SDIVIDE:
      return "~/";
    case BAP_BINOP_MOD:
      return "%";
    case BAP_BINOP_SMOD:
      return "~%";
    case BAP_BINOP_LSHIFT:
      return "<<";
    case BAP_BINOP_RSHIFT:
      return ">>";
    case BAP_BINOP_ARSHIFT:
      return "~>>";
    case BAP_BINOP_AND:
      return "&";
    case BAP_BINOP_OR:
      return "|";
    case BAP_BINOP_XOR:
      return "^";
    case BAP_BINOP_EQ:
      return "==";
    case BAP_BINOP_NEQ:
      return "!=";
    case BAP_BINOP_LT:
      return "<";
    case BAP_BINOP_LE:
      return "<=";
    case BAP_BINOP_SLT:
      return "~<";
    case BAP_BINOP_SLE:
      return "~<=";
    default:
      assert(!"Unknown binop");
      return "?";
  }
}

const char* bap_str_unop(bap_unop op) {
  switch (op) {
    case BAP_UNOP_NEG:
      return "-";
    case BAP_UNOP_NOT:
      return "~";
    default:
      assert(!"Unknown unop");
      return "?";
  }
}

const char* bap_str_cast_type(bap_cast_type cast) {
  switch (cast) {
    case BAP_CAST_UNSIGNED:
      return "u";
    case BAP_CAST_SIGNED:
      return "~";
    case BAP_CAST_HIGH:
      return "h";
    case BAP_CAST_LOW:
      return "l";
    default:
      assert(!"Unknown cast");
      return "?";
  }
}

char* bap_render_type(bap_type* type) {
  char* out;
  switch (type->kind) {
    case BAP_TYPE_IMM:
      if (asprintf(&out, "r%d", type->imm) == -1) {
        return (char*)-1;
      }
      break;
    case BAP_TYPE_MEM:
      if (asprintf(&out, "mem[%d:%d]", type->mem.addr_size, type->mem.cell_size) == -1) {
        return (char*)-1;
      }
      break;
    default:
      assert(!"Tried to render invalid type kind");
  }
  return out;
}

char* bap_render_var(bap_var* var) {
  char* tmp = var->tmp ? ".tmp" : "";
  char* type_str = bap_render_type(var->type);
  char* out;
  if (asprintf(&out, "%s%s.%d : %s", var->name, tmp, var->version, type_str) == -1) {
    return (char*)-1;
  }
  free(type_str);
  return out;
}

char* bap_render_expr(bap_expr* expr) {
  char* out;
  switch (expr->kind) {
    case BAP_EXPR_LOAD:
    {
      char *mem_str = bap_render_expr(expr->load.memory);
      char *index_str = bap_render_expr(expr->load.index);
      const char *endian_str = bap_str_endian(expr->load.endian);
      if (asprintf(&out, "%s[%s~%d:%s]", mem_str, index_str, expr->load.size, endian_str) == -1) {
        return (char*)-1;
      }
      free(mem_str);
      free(index_str);
      break;
    }
    case BAP_EXPR_STORE:
    {
      char *mem_str = bap_render_expr(expr->store.memory);
      char *index_str = bap_render_expr(expr->store.index);
      const char *endian_str = bap_str_endian(expr->store.endian);
      char *val_str = bap_render_expr(expr->store.value);
      if (asprintf(&out, "(%s[%s~%d:%s] <- %s)", mem_str, index_str, expr->store.size, endian_str, val_str) == -1) {
        return (char*)-1;
      }
      free(mem_str);
      free(index_str);
      free(val_str);
      break;
    }
    case BAP_EXPR_BINOP:
    {
      const char *binop_str = bap_str_binop(expr->binop.op);
      char *lhs_str = bap_render_expr(expr->binop.lhs);
      char *rhs_str = bap_render_expr(expr->binop.rhs);
      if (asprintf(&out, "%s %s %s", lhs_str, binop_str, rhs_str) == -1) {
        return (char*)-1;
      }
      free(lhs_str);
      free(rhs_str);
      break;
    }
    case BAP_EXPR_UNOP:
    {
      const char *unop_str = bap_str_unop(expr->unop.op);
      char *arg_str = bap_render_expr(expr->unop.arg);
      if (asprintf(&out, "(%s %s)", unop_str, arg_str) == -1) {
        return (char*)-1;
      }
      free(arg_str);
      break;
    }
    case BAP_EXPR_VAR:
    {
      out = bap_render_var(expr->var);
      break;
    }
    case BAP_EXPR_IMM:
    {
      out = bap_bitvector_to_string(expr->imm);
      break;
    }
    case BAP_EXPR_CAST:
    {
      const char* cast_type_str = bap_str_cast_type(expr->cast.type);
      char* arg_str = bap_render_expr(expr->cast.val);
      if (asprintf(&out, "(%s:%d)(%s)", cast_type_str, expr->cast.width, arg_str) == -1) {
        return (char*)-1;
      }
      free(arg_str);
      break;
    }
    case BAP_EXPR_LET:
    {
      char* bound_var_str = bap_render_var(expr->let.bound_var);
      char* bound_expr_str = bap_render_expr(expr->let.bound_expr);
      char* body_str = bap_render_expr(expr->let.body_expr);
      if (asprintf(&out, "let %s = %s in %s", bound_var_str, bound_expr_str, body_str) == -1) {
        return (char*)-1;
      }
      free(bound_var_str);
      free(bound_expr_str);
      free(body_str);
      break;
    }
    case BAP_EXPR_UNK:
    {
      char* type_str = bap_render_type(expr->unknown.type);
      if (asprintf(&out, "(\"%s\" : %s)", expr->unknown.descr, type_str) == -1) {
        return (char*)-1;
      }
      free(type_str);
      break;
    }
    case BAP_EXPR_ITE:
    {
      char* cond_str = bap_render_expr(expr->ite.cond);
      char* t_str = bap_render_expr(expr->ite.t);
      char* f_str = bap_render_expr(expr->ite.f);
      if (asprintf(&out, "(%s : %s ? %s)", cond_str, t_str, f_str) == -1) {
        return (char*)-1;
      }
      free(cond_str);
      free(t_str);
      free(f_str);
      break;
    }
    case BAP_EXPR_EXTRACT:
    {
      char* val_str = bap_render_expr(expr->extract.val);
      if (asprintf(&out, "%s[%d:%d]", val_str, expr->extract.low_bit, expr->extract.high_bit) == -1) {
        return (char*)-1;
      }
      free(val_str);
      break;
    }
    case BAP_EXPR_CONCAT:
    {
      char* low_str = bap_render_expr(expr->concat.low);
      char* high_str = bap_render_expr(expr->concat.high);
      if (asprintf(&out, "%s ++ %s", low_str, high_str) == -1) {
        return (char*)-1;
      }
      free(low_str);
      free(high_str);
      break;
    }
    default:
      assert(!"Rendering unknown expr kind");
  }
  return out;
}

char* bap_render_stmt(bap_stmt*);

char* bap_render_stmts(char* indent, bap_stmt** stmts) {
  bap_stmt** cur;
  size_t stmt_count = 0;
  for (cur = stmts; *cur != NULL; cur++, stmt_count++);
  char** strs = alloca(sizeof(char*) * stmt_count);
  size_t str_len = 1;
  size_t i;
  for (i = 0; i < stmt_count; i++) {
    char* stmt_str = bap_render_stmt(stmts[i]);
    size_t delta = asprintf(&strs[i], "%s%s\n", indent, stmt_str);
    if (delta == -1) {
      return (char*)-1;
    }
    str_len += delta;
    free(stmt_str);
  }
  char* out = malloc(sizeof(char) * str_len);
  size_t remaining = str_len;
  char* cur_out = out;
  for (i = 0; i < stmt_count; i++) {
    size_t written = snprintf(cur_out, remaining, "%s", strs[i]);
    cur_out += written;
    remaining -= written;
    free(strs[i]);
  }
  return out;
}

char* bap_render_stmt(bap_stmt* stmt) {
  char* out;
  switch (stmt->kind) {
    case BAP_STMT_MOVE:
    {
      char* var_str = bap_render_var(stmt->move.lhs);
      char* expr_str = bap_render_expr(stmt->move.rhs);
      if (asprintf(&out, "%s <- %s", var_str, expr_str) == -1) {
        return (char*)-1;
      }
      free(var_str);
      free(expr_str);
      break;
    }
    case BAP_STMT_JMP:
    {
      char* tgt = bap_render_expr(stmt->jmp);
      if (asprintf(&out, "jmp %s", tgt) == -1) {
        return (char*)-1;
      }
      free(tgt);
      break;
    }
    case BAP_STMT_SPECIAL:
      if (asprintf(&out, "special: %s", stmt->special) == -1) {
        return (char*)-1;
      }
      break;
    case BAP_STMT_WHILE:
    {
      char* cond = bap_render_expr(stmt->s_while.cond);
      char* body = bap_render_stmts("  ", stmt->s_while.body);
      if (asprintf(&out, "while (%s) {\n%s}", cond, body) == -1) {
        return (char*)-1;
      }
      free(cond);
      free(body);
      break;
    }
    case BAP_STMT_IF:
    {
      char* cond = bap_render_expr(stmt->ite.cond);
      char* t = bap_render_stmts("  ", stmt->ite.t);
      char* f = bap_render_stmts("  ", stmt->ite.f);
      if (asprintf(&out, "if (%s) {\n%s} else {\n%s}", cond, t, f) == -1) {
        return (char*)-1;
      }
      free(cond);
      free(t);
      free(f);
      break;
    }
    case BAP_STMT_CPU_EXN:
      if (asprintf(&out, "cpu_exn: %d", stmt->cpu_exn) == -1) {
        return (char*)-1;
      }
      break;
    default:
      if (asprintf(&out, "DECODE ERROR: STMT KIND %d", stmt->kind) == -1) {
        return (char*)-1;
      }
  }
  return out;
}


char* bap_render_segment(bap_segment* seg) {
  char* out;
  const char* rwx;
  switch ((seg->r << 2) | (seg->w << 1) | seg->x) {
    case 7: rwx = "rwx"; break;
    case 6: rwx = "rw"; break;
    case 5: rwx = "rx"; break;
    case 4: rwx = "r"; break;
    case 3: rwx = "wx"; break;
    case 2: rwx = "w"; break;
    case 1: rwx = "x"; break;
    case 0: rwx = ""; break;
    default: rwx = "Internal error"; break;
  }
  char* mem_str = bap_mem_to_string(seg->mem);
  if (asprintf(&out, "Name: %s\tPerms: %s\nMemory:\n %s\n", seg->name, rwx, mem_str) == -1) {
    return (char*)-1;
  }
  return out;
}

void bap_free(void* p) {
  free(p);
}

bool bap_thread_register() {
  return caml_c_thread_register();
}

bool bap_thread_unregister() {
  return caml_c_thread_unregister();
}

void bap_acquire() {
  caml_acquire_runtime_system();
}

void bap_release() {
  caml_release_runtime_system();
}
