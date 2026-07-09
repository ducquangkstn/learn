#include "htable.h"

int main(void) {
  // HashMap *map = map_init(test_hash);
  HashMap *map = map_init(rolling_hash);
  map_insert(map, "foo", "bar");
  map_insert(map, "foo2", "bar2");
  map_insert(map, "foo3", "bar3");
  map_insert(map, "foo2", "other values");
  map_print(map);

  printf("%s\n", (char *)map_get(map, "foo2"));

  map_delete(map, "foo2");
  map_delete(map, "foo");
  map_print(map);
  return 0;
}