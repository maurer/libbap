#include <caml/mlvalues.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
caml_startup(argv);
printf("typedef enum {\n");
printf("  BAP_ARM = 0x%x,\n", caml_hash_variant("arm"));
printf("  BAP_X86 = 0x%x,\n", caml_hash_variant("x86"));
printf("  BAP_X86_64 = 0x%x,\n", caml_hash_variant("x86_64"));
printf("} bap_arch;\n");
}
