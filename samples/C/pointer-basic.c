#include <stdlib.h>
#include <stdio.h>

int main() {
  int* p = calloc(sizeof(p), 8);

  assert(p); // it is alwasy a good practice to check if calloc is successful or not.

  p[0] = 5;
  p[7] = 10;

  printf("pointer: %p\n", p);

  free(p);


  // p += 1;
  // free(p);
  return 0;
}