/* tree.h */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <errno.h>

#define TagRoot 1
#define TagNode 2
#define TagLeaf 4

#define NoErr 0


#define find_last(x) find_last_linear(x)
#define return_err(x) \
  errno = (x); \
  return NULL

typedef unsigned int  uint32;
typedef unsigned short int uint16;
typedef unsigned char Tag;

struct s_node {
  Tag tag;
  struct s_node* north;
  struct s_node* west;
  struct s_leaf* east;

  char path[256];

};

typedef struct s_node Node;

struct s_leaf {
  Tag tag;
  union u_tree* west;
  struct s_leaf* east;

  char key[128];
  char* value;
  size_t size; // size of value
};

typedef struct s_leaf Leaf;

union u_tree {
  Node n;
  Leaf l;
};
typedef union u_tree Tree;

Node* create_node(Node* parent, char* path);
Leaf* find_last_linear(Node* parent);
Leaf* create_leaf(Node* parent, char* key, char* value, size_t count);

char* indent(uint16);
void print_tree(Tree* self, int fd);