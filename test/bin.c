#include <bap.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>

int main() {
  bap_init();
  bap_bitvector bv = bap_create_bitvector64(34L, 8);
  char* bv_str = bap_bitvector_to_string(bv);
  bap_free_bitvector(bv);
  printf("%s\n", bv_str);
  free(bv_str);

  bv = bap_create_bitvector64(0x0123456789ABCDEFL, 64);
  size_t width = bap_bitvector_size(bv);
  char* contents = bap_bitvector_contents(bv);
  bap_free_bitvector(bv);
  printf("%x:%d\n", *(uint64_t*)contents, width);
  free(contents);

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

    bap_stmt** stmts = bap_insn_get_stmts((*cur)->insn);
    bap_stmt** ic;
    for (ic = stmts; *ic != NULL; ic++) {
      printf("%s\n", bap_render_stmt(*ic));
    }
    bap_free_disasm_insn(*cur);
  }
  free(insns);
  #define ELF_SIZE 7090
  int elf_fd = open("test_elf", O_RDONLY);
  char elf_buf[ELF_SIZE];
  read(elf_fd, elf_buf, ELF_SIZE);
  bap_segment** segments = bap_get_segments(elf_buf, ELF_SIZE);

  bap_segment** cur_seg;
  for (cur_seg = segments; *cur_seg != NULL; cur_seg++) {
    printf("%s", bap_render_segment(*cur_seg));
    bap_addr* starts = bap_byteweight(BAP_X86, (*cur_seg)->mem);
    bap_addr* cur_addr;
    for (cur_addr = starts; *cur_addr != NULL; cur_addr++) {
      size_t width = bap_bitvector_size(*cur_addr);
      char* contents = bap_bitvector_contents(*cur_addr);
      bap_free_bitvector(*cur_addr);
      printf("%x:%d\n", *(uint64_t*)contents, width);
      free(contents);
    }
    bap_free_segment(*cur_seg);
  }
  free(segments);

  bap_symbol** symbols = bap_get_symbols(elf_buf, ELF_SIZE);
  bap_symbol** cur_sym;
  for (cur_sym = symbols; *cur_sym != NULL; cur_sym++) {
    char* contents_start = bap_bitvector_contents((*cur_sym)->start);
    char* contents_end   = bap_bitvector_contents((*cur_sym)->end);
    printf("%s~%x:%d->%x:%d\n", (*cur_sym)->name,
           *(uint64_t*)contents_start, bap_bitvector_size((*cur_sym)->start),
           *(uint64_t*)contents_end,   bap_bitvector_size((*cur_sym)->end));
    free(contents_start);
    free(contents_end);
    bap_free_symbol(*cur_sym);
  }

  bap_release();
  assert(bap_thread_unregister());
  return 0;
}
