#include <stdio.h>
#include <stdlib.h>

int
main() {
  void* i = malloc(1);
  printf("Deliberate leak at %p\n", i);
  // never freed/deleted
}
