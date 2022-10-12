#include <stdio.h>
#include <stdlib.h>

auto main() -> int {
  void* i = malloc(1);
  printf("Deliberate leak at %p\n", i);
  // never freed/deleted
}
