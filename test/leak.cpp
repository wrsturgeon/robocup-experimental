#include <stdio.h>

int main() {
  void* i = new int;
  printf("Deliberate leak at %p\n", i);
  // never freed/deleted
}
