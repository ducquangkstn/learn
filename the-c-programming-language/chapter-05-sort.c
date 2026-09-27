/*
  TODO:
  - use the built-in getline
  - implement -f option
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINES 5000
char *lineptr[MAXLINES];

typedef int (*CmpFn)(void *, void *);

// return -1 if errors
int readlines(char *re[], int nlines);
void writelines(char *in[], int nlines);
void quick_sort(void *arr[], int l, int r, CmpFn fn, bool rev);
int my_getline(char *re, int max_len);

int numcmp(char *s1, char *s2) {
  double v1, v2;
  v1 = atof(s1);
  v2 = atof(s2);
  if (v1 < v2)
    return -1;
  else if (v1 > v2)
    return 1;
  else
    return 0;
}

int main(int argc, char *argv[]) {
  int nlines;
  bool numberic = false;
  int reverse = false;
  bool case_insensitive = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[1], "-n") == 0) {
      numberic = true;
    } else if (strcmp(argv[i], "-f") == 0) {
      case_insensitive = true;
    } else if (strcmp(argv[i], "-r") == 0) {
      reverse = true;
    } else {
      printf("invalid option: %s", argv[i]);
      return 1;
    }
  }

  if ((nlines = readlines(lineptr, MAXLINES)) < 0) {
    printf("failed to readline\n");
    return 1;
  }
  CmpFn fn = (CmpFn)strcmp;
  if (numberic)
    fn = (CmpFn)numcmp;
  quick_sort((void **)lineptr, 0, nlines - 1, fn, reverse);
  writelines(lineptr, nlines);
  return 0;
}

#define MAX_LINE_LEN 1000

int readlines(char *re[], int maxlines) {
  int nlines = 0;
  char *p, buf[MAX_LINE_LEN];
  int len;

  while ((len = my_getline(buf, MAX_LINE_LEN)) > 0) {
    if (nlines >= maxlines)
      return -1;
    if ((p = malloc(len)) == NULL)
      return -1;
    buf[len - 1] = '\0';
    strcpy(p, buf);
    lineptr[nlines++] = p;
  }

  return nlines;
}

void writelines(char *in[], int nlines) {
  for (int i = 0; i < nlines; i++)
    printf("%s\n", in[i]);
}

static inline void swap_str(void **ch1, void **ch2) {
  void *tmp = *ch1;
  *ch1 = *ch2;
  *ch2 = tmp;
}

void quick_sort(void *arr[], int l, int r, CmpFn fn, bool rev) {
  if (l >= r)
    return;
  swap_str(&arr[l], &arr[(l + r) / 2]);
  int last = l;
  for (int i = l + 1; i <= r; i++) {
    if (!rev ? (fn(arr[i], arr[l]) < 0) : ((fn(arr[i], arr[l]) > 0)))
      swap_str(&arr[i], &arr[++last]);
  }
  swap_str(&arr[l], &arr[last]);
  quick_sort(arr, l, last - 1, fn, rev);
  quick_sort(arr, last + 1, r, fn, rev);
}

int my_getline(char s[], int lim) {
  int c, i;
  for (i = 0; i < lim - 1 && (c = getchar()) != EOF && c != '\n'; ++i)
    s[i] = c;
  if (c == '\n') {
    s[i] = c;
    ++i;
  }
  s[i] = '\0';
  return i;
}