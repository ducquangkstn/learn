#include "htable.h"

uint16_t rolling_hash(char *key) {
  uint16_t re = 0, factor = 101;

  for (int i = 0; i < strlen(key); i++) {
    re = (re * factor + (uint16_t)key[i]) % (UINT16_MAX - 1);
  }
  return re;
}

uint16_t test_hash(char *key) { return 1; }

HashMap *map_init(HashFn hash_fn) {
  HashMap *re = malloc(sizeof(*re));
  assert(re != NULL);
  re->arr = calloc(UINT16_MAX, sizeof(*re->arr));
  assert(re->arr != NULL);
  re->size = 0;
  re->hash_fn = hash_fn;
  re->v_destroy_fn = NULL;
  return re;
}

void *map_insert(HashMap *self, char *key, void *value) {
  assert(key != NULL);
  assert(value != NULL);

  uint16_t hash = self->hash_fn(key);
  Node *root = self->arr[hash];
  if (root == NULL) {
    Node *n = init_node(key, value);
    assert(n != NULL);
    self->arr[hash] = n;
    return NULL;
  }

  Node *n = get_node(root, key);
  if (n) {
    void *old_value = n->value;
    n->value = value;
    if (self->v_destroy_fn) {
      self->v_destroy_fn(old_value);
      return NULL;
    }
    return old_value;
  }

  Node *last = get_last_node(root);

  n = init_node(key, value);
  assert(n != NULL);
  last->next = n;
  return NULL;
}

void *map_get(HashMap *self, char *key) {
  assert(key != NULL);

  uint16_t hash = self->hash_fn(key);
  Node *root = self->arr[hash];
  if (root == NULL) {
    return NULL;
  }
  Node *n = get_node(root, key);
  if (n)
    return n->value;
  else
    return NULL;
}

void *map_delete(HashMap *self, char *key) {
  assert(key != NULL);

  uint16_t hash = self->hash_fn(key);
  Node *root = self->arr[hash];
  if (root == NULL) {
    return NULL;
  }
  if (strcmp(root->key, key) == 0) {
    self->arr[hash] = root->next;
    return node_delete(root, self->v_destroy_fn);
  }

  for (Node *n = root; n->next; n++) {
    if (strcmp(n->next->key, key) != 0)
      continue;
    Node *tmp = n->next;
    n->next = n->next->next;
    return node_delete(tmp, self->v_destroy_fn);
  }
  return NULL;
}

void map_print(HashMap *self) {
  printf("-------------------------\n");
  for (uint16_t i = 0; i < UINT16_MAX; i++) {
    if (!self->arr[i])
      continue;
    printf("- %x:\n", i);
    print_ll(self->arr[i]);
  }
  printf("-------------------------\n");
}

Node *get_node(Node *root, char *key) {
  for (Node *n = root; n; n = n->next) {
    if (strcmp(n->key, key) == 0) {
      return n;
    }
  }
  return NULL;
}

Node *init_node(char *key, void *value) {
  Node *n = malloc(sizeof(*n));
  assert(n != NULL);

  n->key = malloc(strlen(key) + 1);
  assert(n->key != NULL);
  strcpy(n->key, key);

  n->value = value;
  n->next = NULL;
  return n;
}

void *node_delete(Node *n, DestroyFn delete_fn) {
  free(n->key);
  void *tmp = n->value;
  free(n);

  if (delete_fn) {
    delete_fn(tmp);
    return NULL;
  }
  return tmp;
}

Node *get_last_node(Node *root) {
  Node *n;
  for (n = root; n->next; n = n->next)
    ;

  return n;
}

void print_ll(Node *root) {
  for (Node *n = root; n; n = n->next)
    printf("\t- %s=%s\n", n->key, (char *)n->value);
}
