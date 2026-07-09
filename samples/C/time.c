#define _GNU_SOURCE

#include <error.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

int main(void) {
  struct timespec tv;
  if (clock_gettime(CLOCK_REALTIME, &tv))
    perror("failed to clock_gettime\n");
  uint64_t out = tv.tv_nsec + tv.tv_sec * 1000L;
  printf("timestamp in ms: %lu\n", out);
}