#include "storage.h"

Storage *storage_init() {
  Storage *re = malloc(sizeof(*re));
  assert(re);
  re->map = map_init(rolling_hash);
  re->map->v_destroy_fn = value_free;
  return re;
}

Value *value_init_string(char *v) {
  Value *re = malloc(sizeof(*re));
  assert(re);
  re->expiredInMs = -1;
  re->vtype = VTYPE_STR;
  re->v = v;
  return re;
}

void value_free(void *v) {
  Value *self = v;
  free(self->v);
  free(self);
}

inline int64_t current_ts() {
  struct timespec tv;
  if (clock_gettime(CLOCK_REALTIME, &tv))
    perror("failed to clock_gettime\n");
  int64_t out = tv.tv_nsec / 1000000L + tv.tv_sec * 1000L;
  return out;
}