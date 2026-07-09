#define _GNU_SOURCE

#include <error.h>
#include <stdint.h>
#include <time.h>

#include "htable.h"

#ifndef MANAGER
#define MANAGER

#define VTYPE_STR 0

typedef struct {
  int64_t expiredInMs; // -1 mean infinity
  uint8_t vtype;       // will be used later.
  char *v;
} Value;

Value *value_init_string(char *);
void value_free(void *v);

typedef struct {
  HashMap *map; // key = char* value = Value*
} Storage;

Storage *storage_init();

// utils func
int64_t current_ts();

#endif