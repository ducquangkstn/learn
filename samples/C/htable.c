/* Implementation of hash table. Ref: https://www.geeksforgeeks.org/dsa/implementation-of-hash-table-in-c-using-separate-chaining/ */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#

typedef struct s_node {
  char* key;
  char* value;

  struct s_node* next;
} Node;

typedef uint16_t(*HashFn)(char*);

typedef struct s_hashmap {
  int size;

  HashFn hash_fn;

  Node** arr;
} HashMap;


Node* get_node(Node* root, char* key);
Node* get_last_node(Node* root);
Node* init_node(char* key, char* value);
void print_ll(Node* root);
void node_delete(Node*);

HashMap* map_init(HashFn);
Node* map_insert(HashMap* self, char* key, char* value);
void map_print(HashMap* self);
void map_delete(HashMap* self, char* key);
char* map_get(HashMap* self, char* key);


uint16_t rolling_hash(char* key) {
  uint16_t re = 0, factor = 101;

  for (int i = 0;i < strlen(key); i++) {
    re = (re * factor + (uint16_t)key[i]) % (UINT16_MAX - 1);
  }
  return re;

}

uint16_t test_hash(char* key) {
  return 1;
}

int main() {
  // HashMap* map = map_init(rolling_hash);
  HashMap* map = map_init(test_hash);
  map_insert(map, "foo", "bar");
  map_insert(map, "foo2", "bar2");
  map_insert(map, "foo3", "bar3");
  map_insert(map, "foo2", "other values");
  map_print(map);

  printf("%s\n", map_get(map, "foo2"));

  map_delete(map, "foo2");
  map_delete(map, "foo");
  map_print(map);
  return 0;
}




HashMap* map_init(HashFn hash_fn) {
  HashMap* re = malloc(sizeof(*re));
  assert(re != NULL);
  re->arr = malloc(sizeof(*re->arr) * UINT16_MAX);
  assert(re->arr != NULL);
  re->size = 0;
  re->hash_fn = hash_fn;
  return re;
}


Node* map_insert(HashMap* self, char* key, char* value) {
  assert(key != NULL);
  assert(value != NULL);

  uint16_t hash = self->hash_fn(key);
  Node* root = self->arr[hash];
  if (root == NULL) {
    Node* n = init_node(key, value);
    assert(n != NULL);
    self->arr[hash] = n;
    return n;
  }

  Node* n = get_node(root, key);
  if (n) {
    // in-case the memory size is not enough, realloc it.
    if (strlen(n->value) < strlen(value)) {
      n->value = realloc(n->value, strlen(value) + 1);
      assert(n->value != NULL);
    }
    strcpy(n->value, value);
    return n;
  }

  Node* last = get_last_node(root);

  n = init_node(key, value);
  assert(n != NULL);
  last->next = n;
  return n;
}

char* map_get(HashMap* self, char* key) {
  assert(key != NULL);

  uint16_t hash = self->hash_fn(key);
  Node* root = self->arr[hash];
  if (root == NULL) {
    return NULL;
  }
  Node* n = get_node(root, key);
  if (n)
    return n->value;
  else
    return NULL;
}

void map_delete(HashMap* self, char* key) {
  assert(key != NULL);

  uint16_t hash = self->hash_fn(key);
  Node* root = self->arr[hash];
  if (root == NULL) {
    return;
  }
  if (strcmp(root->key, key) == 0) {
    self->arr[hash] = root->next;
    node_delete(root);
    return;
  }

  for (Node* n = root; n->next; n++) {
    if (strcmp(n->next->key, key) != 0) continue;
    Node* tmp = n->next;
    n->next = n->next->next;
    node_delete(tmp);
    return;
  }
}


void map_print(HashMap* self) {
  printf("-------------------------\n");
  for (uint16_t i = 0; i < UINT16_MAX; i++) {
    if (!self->arr[i]) continue;
    printf("- %x:\n", i);
    print_ll(self->arr[i]);
  }
  printf("-------------------------\n");
}


Node* get_node(Node* root, char* key) {
  for (Node* n = root; n; n = n->next) {
    if (strcmp(n->key, key) == 0) {
      return n;
    }
  }
  return NULL;
}

Node* init_node(char* key, char* value) {
  Node* n = malloc(sizeof(*n));
  assert(n != NULL);

  n->key = malloc(strlen(key) + 1);
  assert(n->key != NULL);
  strcpy(n->key, key);

  n->value = malloc(strlen(value) + 1);
  assert(n->value != NULL);
  strcpy(n->value, value);
  n->next = NULL;
  return n;
}

void node_delete(Node* n) {
  free(n->key);
  free(n->value);
  free(n);
}

Node* get_last_node(Node* root) {
  Node* n;
  for (n = root; n->next; n = n->next);

  return n;
}

void print_ll(Node* root) {
  for (Node* n = root; n; n = n->next)
    printf("\t- %s=%s\n", n->key, n->value);
}


