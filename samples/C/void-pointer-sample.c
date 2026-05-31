#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int floatcmp(const void* p1, const void* p2);

int main() {
  printf("hello world\n");

  float* numbers = malloc(10 * sizeof(float));
  assert(numbers);

  for (int i = 0;i < 10;i++)
    numbers[i] = (float)rand() / (float)RAND_MAX;
  qsort(numbers, 10, sizeof(float), floatcmp);

  for (int i = 0;i < 10;i++)
    printf("%f\t", numbers[i]);
  return 0;
}

int floatcmp(const void* p1, const void* p2) {
  float x1 = *(float*)p1;
  float x2 = *(float*)p2;
  return (x1 < x2) ? -1 : ((x1 == x2) ? 0 : 1);
}
