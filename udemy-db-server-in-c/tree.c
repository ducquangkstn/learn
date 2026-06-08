/* tree.c */
#include "tree.h"

Tree root = {
  .n = {
  .tag = (TagRoot | TagNode),
  .north = (Node*)&root,
  .west = 0,
  .east = 0,
  .path = "/"
  }
};

Node* create_node(Node* parent, char* path) {
  assert(parent);
  
  size_t size = sizeof(Node);
  Node* n = malloc(size);
  memset(n, 0, size);

  parent->west = n;
  n->tag = TagNode;
  n->north = parent;
  strcpy(n->path, path);

  return n;
}

Leaf* find_last_linear(Node* parent) {
  assert(parent);

  if (!parent->east)
    return_err(NoErr);
  Leaf* l;
  for (l = parent->east; l != NULL;l = l->east);
  assert(l);

  return l;
}

Leaf* create_leaf(Node* parent, char* key, char *value, size_t count) {
  assert(parent);

  Leaf* l = find_last(parent);

  size_t size = sizeof(Leaf);
  Leaf* new = malloc(size);
  assert(new);
  memset(new, 0, size);

  if (!l)  // directly connect
    parent->east = new;
  else  // l is a leaf
    l->east = new;

  new->tag = TagLeaf;
  new->west = (!l) ? (Tree*)parent : (Tree*)l;
  strcpy(new->key, key);
  new->value = malloc(count);
  memset(new->value, 0, count);
  strcpy(new->value, value);
  new->size = count;

  return new;
}

int main() {
  printf("root: %p\n", &root);

  Node* n = create_node((Node *)&root, "/Users");
  assert(n);

  Node* n2 = create_node(n, "/Users/login");
  assert(n2);

  char* key = "jonas";
  char* value = "abcd1324r98";
  size_t size = strlen(value);
  Leaf* l1 = create_leaf(n2, key, value, size);
  assert(l1);

  key = "john";
  value = "dsrf342";
  size = strlen(value);
  Leaf* l2 = create_leaf(n2, key, value, size);
  assert(l2);

  printf("%p %p\n", n, n2);
  printf("%s\n", l1->value);
  printf("%s\n", l2 -> key);


  free(n);
  free(n2);
  free(l1);

  return 0;
} 