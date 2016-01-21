#include <stdio.h>
char* f() {
  return "foo";
}

int main (int argc, char* argv[]) {
  printf("Hello world: %s\n", f());
  return 0;
}
