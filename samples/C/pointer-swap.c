#include <stdio.h>

void swap(int* a, int* b) {
  int tmp = *b;
  *b = *a;
  *a = tmp;
}


int main() {
  int x = 10, y = 20;
  swap(&x, &y);

  printf("%d - %d\n", x, y); // expected 20 -10
  return 0;
}