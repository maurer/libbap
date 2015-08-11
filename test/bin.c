#include <bap.h>
#include <stdio.h>
#include <assert.h>

int main() {
  bap_init();
  bap_bitvector bv = bap_create_bitvector64(34L, 8);
  char* bv_str = bap_bitvector_to_string(bv);
  bap_free_bitvector(bv);
  printf("%s\n", bv_str);
  free(bv_str);

  char bs_test_str[] = "foo\0bar";
  bap_bigstring bs = bap_create_bigstring(bs_test_str, sizeof(bs_test_str));
  char bs_out_str[sizeof(bs_test_str)] = {0};
  size_t len = bap_bigstring_to_buf(bs, bs_out_str, sizeof(bs_test_str));
  bap_free_bigstring(bs);
  assert(len == sizeof(bs_test_str));
  printf("%s\n", bs_out_str);
  printf("%s\n", bs_out_str + 4);

  bap_bitvector addr0 = bap_create_bitvector64(0, 32);
  char shellcode[] = "\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80";
  bap_bigstring bs_shellcode = bap_create_bigstring(shellcode, sizeof(shellcode));
  bap_mem mem = bap_create_mem(0, sizeof(shellcode), BAP_LITTLE_ENDIAN, addr0, bs_shellcode);
  bap_free_bitvector(addr0);
  char* mem_str = bap_mem_to_string(mem);
  printf("%s", mem_str);
  free(mem_str);
  bap_disasm d = bap_disasm_mem(NULL, BAP_X86, mem);
  bap_free_mem(mem);
  char* disas_str = bap_disasm_to_string(d);
  printf("%s", disas_str);
  free(disas_str);
  bap_disasm_insn** insns = bap_disasm_get_insns(d);
  bap_disasm_insn** cur;
  for (cur = insns; *cur != NULL; cur++) {
    char* start = bap_bitvector_to_string((*cur)->start);
    char* end   = bap_bitvector_to_string((*cur)->end);
    char* asmx  = bap_insn_to_asm((*cur)->insn);
    printf("%s->%s: %s\n", start, end, asmx);
    free(start);
    free(end);
    free(asmx);
    bap_free_disasm_insn(*cur);
  }
  free(insns);
  return 0;
}
