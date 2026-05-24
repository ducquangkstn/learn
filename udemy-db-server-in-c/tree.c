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

int main() {
  printf("hello\n");
  printf("%p\n", &root);
  return 0;
} 