#include <stdio.h>

int filecopy(FILE *, FILE *);

int main(int argc, char *argv[]) {
  FILE *fp;

  if (argc == 1) {
    if (filecopy(stdin, stdout) == -1)
      return 2;
    return 0;
  }

  for (int i = 1; i < argc; i++) {
    fp = fopen(argv[i], "r");
    if (fp == NULL) {
      fprintf(stderr, "can't open: %s\n", argv[i]);
      return 1;
    }

    filecopy(fp, stdout);
    fclose(fp);
  }
  return 0;
}

int filecopy(FILE *inf, FILE *outf) {
  int c;
  while ((c = getc(inf)) != EOF)
    putc(c, outf);

  if (ferror(outf)) {
    fprintf(stderr, "failed to write to out file");
    return -1;
  }

  if (ferror(inf)) {
    fprintf(stderr, "failed to write to out file");
    return -1;
  }

  return 0;
}