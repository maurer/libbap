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
  return 0;
}
