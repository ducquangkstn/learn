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

Node* find_node_linear(char* path) {
  for (Node* p = (Node*)&root; p; p = p->west)
    if (strcmp(p->path, path) == 0)
      return p;
  return NULL;
}

Leaf* lookup_linear(char* path, char* key) {
  Node* n= find_node(path);
  if (!n)
    return NULL;
  return NULL;
}

Leaf* find_last_linear(Node* parent) {
  assert(parent);

  if (!parent->east) {
    return_err(NoErr);
  }

  Leaf* l;
  for (l = parent->east; l -> east; l = l->east);
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

// Warning: not thread-safe.
char* indent(uint16 n) {
  static char buf[256];
  memset(buf, 0, 256);
  assert(n < 127);
  if (n < 0) n = 0;
  char* p; int i;
  for (i = 0, p = buf;i < n;p += 2, i++)
    *p = *(p+1) = ' ';
  return buf;
}

#define Print(x) \
  memset(buf, 0, 256); \
  strncpy(buf, (x), 255); \
  size = strlen(buf); \
  if (size) \
    assert(write(fd, buf, size) != -1) \

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
void print_tree(Tree* root, int fd) {
  int indentation = 0;
  char buf[256];
  uint16 size;

  for (Node *n = (Node*)root; n; n = n->west) {
    Print(indent(indentation++));
    Print(n->path);
    Print("\n");
    for (Leaf* l = n->east; l; l = l->east) {
      Print(indent(indentation));
      Print(n->path);
      Print("/");
      Print(l->key);
      Print(" -> '");
      assert(write(fd, l->value, l->size) != -1);
      Print("'\n");
    }
  }
}
#pragma GCC diagnostic pop



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

  // printf("create_leaf 1: %p - %p\n", l1, n2->east);
  // print_tree(&root, STDOUT_FILENO);

  key = "john";
  value = "dsrf342";
  size = strlen(value);
  Leaf* l2 = create_leaf(n2, key, value, size);
  assert(l2);

  print_tree(&root, STDOUT_FILENO);
  printf("find_node 1: %p (expected: %p) - %p\n", find_node("/Users/login"), n2, find_node("/Users/test"));

  free(n);
  free(n2);
  free(l1);

  return 0;
} 