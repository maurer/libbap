#include <bap.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

void arm_sub() {
  #define ELF_ARM_SIZE 6818
  int arm_fd = open("nop.arm", O_RDONLY);
  char arm_buf[ELF_ARM_SIZE];
  size_t elf_arm_read = read(arm_fd, arm_buf, ELF_ARM_SIZE);
  assert(elf_arm_read == ELF_ARM_SIZE);
  bap_arch arch_arm = bap_get_arch(arm_buf, ELF_ARM_SIZE);
  char* arch_arm_string = bap_arch_to_string(arch_arm);
  printf("ARCH_ARM: %lx:%s\n", arch_arm, arch_arm_string);
  free(arch_arm_string);
  close(arm_fd);

  bap_bitvector addr0 = bap_create_bitvector64(0, 32);
  char shellcode[] = "\xf8\x43\x2d\xe9\x4c\x50\x9f\xe5";
  bap_bigstring bs_shellcode = bap_create_bigstring(shellcode, sizeof(shellcode));
  bap_mem mem = bap_create_mem(0, sizeof(shellcode), BAP_LITTLE_ENDIAN, addr0, bs_shellcode);
  bap_free_bitvector(addr0);
  char* mem_str = bap_mem_to_string(mem);
  printf("%s", mem_str);
  free(mem_str);
  bap_disasm d = bap_disasm_mem(NULL, BAP_ARM, mem);
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
    bool call = bap_insn_is_call((*cur)->insn);
    printf("%s->%s: %s Call:%d\n", start, end, asmx, call);
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
  #
}

int main() {
  setbuf(stdout, NULL);
  bap_init();
  bap_bitvector bv = bap_create_bitvector64(34L, 8);
  char* bv_str = bap_bitvector_to_string(bv);
  bap_free_bitvector(bv);
  printf("%s\n", bv_str);
  free(bv_str);

  uint64_t buf[] = {0x0123456789ABCDEFL, 0xFEDCBA9876543210L};
  bap_bitvector bvb = bap_create_bitvector(128, 16, buf);
  char* bvb_str = bap_bitvector_to_string(bvb);
  bap_free_bitvector(bvb);
  printf("%s\n", bvb_str);
  free(bvb_str);


  bv = bap_create_bitvector64(0x0123456789ABCDEFL, 64);
  size_t width = bap_bitvector_size(bv);
  char* contents = bap_bitvector_contents(bv);
  bap_free_bitvector(bv);
  printf("%lx:%zu\n", *(uint64_t*)contents, width);
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
  char shellcode[] = "\xff\xd0\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80";
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
    bool call = bap_insn_is_call((*cur)->insn);
    printf("%s->%s: %s Call:%d\n", start, end, asmx, call);
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
  #define ELF_SIZE 4924
  int elf_fd = open("test_elf.x86", O_RDONLY);
  char elf_buf[ELF_SIZE];
  size_t elf_read = read(elf_fd, elf_buf, ELF_SIZE);
  assert(elf_read == ELF_SIZE);
  bap_segment** segments = bap_get_segments(elf_buf, ELF_SIZE);
  bap_arch arch = bap_get_arch(elf_buf, ELF_SIZE);
  char* arch_string = bap_arch_to_string(arch);
  printf("ARCH: %lx:%s\n", arch, arch_string);
  free(arch_string);
  close(elf_fd);

  bap_segment** cur_seg;
  for (cur_seg = segments; *cur_seg != NULL; cur_seg++) {
    printf("%s", bap_render_segment(*cur_seg));
    bap_mem_c* cur_mem = bap_project_mem((*cur_seg)->mem);
    printf("%lu\n", cur_mem->len);
    bap_free_bitvector(cur_mem->start);
    bap_free_bitvector(cur_mem->end);
    free(cur_mem->data);
    free(cur_mem);
    bap_addr* starts = bap_byteweight(BAP_X86, (*cur_seg)->mem);
    bap_addr* cur_addr;
    for (cur_addr = starts; *cur_addr != NULL; cur_addr++) {
      size_t width = bap_bitvector_size(*cur_addr);
      char* contents = bap_bitvector_contents(*cur_addr);
      bap_free_bitvector(*cur_addr);
      printf("%x:%zu\n", *(uint32_t*)contents, width);
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
    printf("%s~%x:%zu->%x:%zu\n", (*cur_sym)->name,
           *(uint32_t*)contents_start, bap_bitvector_size((*cur_sym)->start),
           *(uint32_t*)contents_end,   bap_bitvector_size((*cur_sym)->end));
    free(contents_start);
    free(contents_end);
    bap_free_symbol(*cur_sym);
  }

  #define ELF_64_SIZE 7313
  int elf_64_fd = open("test_elf.x86_64", O_RDONLY);
  char elf_64_buf[ELF_64_SIZE];
  size_t elf_64_read = read(elf_64_fd, elf_64_buf, ELF_64_SIZE);
  assert(elf_64_read == ELF_64_SIZE);
  bap_arch arch_64 = bap_get_arch(elf_64_buf, ELF_64_SIZE);
  char* arch_64_string = bap_arch_to_string(arch_64);
  printf("ARCH64: %lx:%s\n", arch_64, arch_64_string);
  free(arch_64_string);
  close(elf_64_fd);

  arm_sub();

  bap_release();
  assert(bap_thread_unregister());
  return 0;
}
