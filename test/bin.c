#include <bap.h>
#include <stdio.h>

int main() {
  libbap_init();
  bitvector bv = bitvector_of_int64(34L, 8);
  char* bv_str = bitvector_to_string(bv);
  free_bitvector(bv);
  printf("%s\n", bv_str);
  free(bv_str);
  return 0;
}
