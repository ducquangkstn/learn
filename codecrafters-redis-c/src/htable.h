/* Implementation of hash table. Ref:
 * https://www.geeksforgeeks.org/dsa/implementation-of-hash-table-in-c-using-separate-chaining/
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HTABLE
#define HTABLE

uint16_t rolling_hash(char *key);
uint16_t test_hash(char *key);

typedef struct s_node {
  char *key;
  void *value;

  struct s_node *next;
} Node;

typedef uint16_t (*HashFn)(char *);
typedef void (*DestroyFn)(void *);

typedef struct s_hashmap {
  int size;

  HashFn hash_fn;
  DestroyFn v_destroy_fn;

  Node **arr;
} HashMap;

Node *get_node(Node *root, char *key);
Node *get_last_node(Node *root);
Node *init_node(char *key, void *value);
void print_ll(Node *root); // only works with value type as char*
void *node_delete(Node *, DestroyFn);

HashMap *map_init(HashFn);
void *map_insert(HashMap *self, char *key, void *value);
void map_print(HashMap *self); // only works with value type as char*
void *map_delete(HashMap *self, char *key);
void *map_get(HashMap *self, char *key);

#endif