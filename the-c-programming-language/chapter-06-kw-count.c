#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_WORD 100
#define NKEYS 12
typedef struct key {
  char *word;
  int count;
} Key;

struct key keytab[NKEYS] = {"auto",    0, "break",    0, "case",     0,
                            "char",    0, "const",    0, "continue", 0,
                            "default", 0, "return",   0, "unsigned", 0,
                            "void",    0, "volatile", 0, "while",    0};

// return EOF if failed.
int get_word(char *re, int max);
int binary_search(char *, Key[], int);

int main(void) {
  char word[MAX_WORD + 1];

  while (get_word(word, MAX_WORD) != EOF) {
    if (!isalpha(word[0]))
      continue;

    int id = binary_search(word, keytab, NKEYS);
    if (id >= 0)
      keytab[id].count++;
  }

  for (int i = 0; i < NKEYS; i++)
    if (keytab[i].count > 0)
      printf("%4d - %s\n", keytab[i].count, keytab[i].word);

  return 0;
}

int binary_search(char *word, Key keytab[], int n) {
  int l = 0, h = n - 1;
  while (l <= h) {
    int mid = (l + h) / 2;
    int tmp = strcmp(word, keytab[mid].word);
    if (tmp == 0)
      return mid;
    if (tmp < 0)
      h = mid - 1;
    else
      l = mid + 1;
  }
  return -1;
}

int get_word(char *word, int lim) {
  int c;

  char *w = word; // point to the current position that we are writing to
  // skip every space or next new line.
  while (isspace(c = getchar()))
    ;

  if (c != EOF)
    *w++ = c;

  // if next char is not alpha, return it immediately.
  if (!isalpha(c)) {
    *w = '\0';
    return c;
  }
  // read until the next char is not alpha anymore.
  for (; --lim > 0; w++)
    if (!isalpha(*w = getchar())) {
      ungetc(*w, stdin);
      break;
    }
  *w = '\0';
  return word[0];
}