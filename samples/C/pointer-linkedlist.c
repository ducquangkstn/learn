#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int v;
  struct Node* next;
} Node;

void addNode(Node* p, int v) {
  Node* tmp = calloc(sizeof(tmp), 1);
  tmp->v = v;
  tmp->next = NULL;
  p->next = tmp;
}

void printLL(Node* root) {
  Node* tmp = root;
  while (tmp != NULL) {
    printf("%d - %p -", tmp->v, tmp->next);
    tmp = tmp->next;
  }
  printf("\n");
}

int main() {
  Node* root = calloc(sizeof(root), 1);
  root->v = 5;
  addNode(root, 10);
  printf("root.next: %p\n", root->next);
  printf("root.next.next: %p\n", root->next->next);

  printLL(root);

  return 0;
}