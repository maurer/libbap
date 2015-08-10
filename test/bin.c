#include <bap.h>
#include <stdio.h>
#include <assert.h>

int main() {
  libbap_init();
  bitvector bv = bitvector_of_int64(34L, 8);
  char* bv_str = bitvector_to_string(bv);
  free_bitvector(bv);
  printf("%s\n", bv_str);
  free(bv_str);

  char bs_test_str[] = "foo\0bar";
  bigstring bs = create_bigstring(0, sizeof(bs_test_str), bs_test_str);
  char bs_out_str[sizeof(bs_test_str)] = {0};
  size_t len = bigstring_to_buf(bs, bs_out_str, sizeof(bs_test_str));
  free_bigstring(bs);
  assert(len == sizeof(bs_test_str));
  printf("%s\n", bs_out_str);
  printf("%s\n", bs_out_str + 4);

  bitvector addr0 = bitvector_of_int64(0, 32);
  char shellcode[] = "\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80";
  bigstring bs_shellcode = create_bigstring(0, sizeof(shellcode), shellcode);
  mem mem = create_mem(0, sizeof(shellcode), BAP_LITTLE_ENDIAN, addr0, bs_shellcode);
  free_bitvector(addr0);
  char* mem_str = mem_to_string(mem);
  free_mem(mem);
  printf("%s", mem_str);
  free(mem_str);
  return 0;
}
