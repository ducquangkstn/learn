/* Exercise 7-6. Write a program that print out the 1st time that are
 * different
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define pretty_exit(code, ...)                                                 \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
    exit(code);                                                                \
  } while (0)

#define LINE_SEPARATOR "-------------------------------"
#define EMPTYLINE "No more new line"

int main(int argc, char *argv[]) {
  if (argc != 3)
    pretty_exit(1, "Usage: diff <src> <dest>");

  FILE *f1 = fopen(argv[1], "r");
  if (f1 == NULL)
    pretty_exit(2, "open 1st file");

  FILE *f2 = fopen(argv[2], "r");
  if (f2 == NULL)
    pretty_exit(2, "open 2nd file");

  char *buf1, *buf2;
  size_t len1, len2;

  while (1) {
    int tmp1 = getline(&buf1, &len1, f1);
    int tmp2 = getline(&buf2, &len2, f2);
    if (tmp1 == EOF && tmp2 == EOF)
      pretty_exit(3, "both files are identical\n");
    if (tmp1 == EOF)
      pretty_exit(0, "%s\n%s\n%s\n%s\n%s\n", LINE_SEPARATOR, EMPTYLINE,
                  LINE_SEPARATOR, buf2, LINE_SEPARATOR);

    if (tmp2 == EOF)
      pretty_exit(0, "%s\n%s\n%s\n%s\n%s\n", LINE_SEPARATOR, buf1,
                  LINE_SEPARATOR, EMPTYLINE, LINE_SEPARATOR);

    if (strcmp(buf1, buf2) == 0)
      continue;

    pretty_exit(0, "%s\n%s\n%s\n%s\n%s\n", LINE_SEPARATOR, buf1, LINE_SEPARATOR,
                buf2, LINE_SEPARATOR);
  }

  fclose(f1);
  fclose(f2);
}