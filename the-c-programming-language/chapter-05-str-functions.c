#include <stdio.h>
#include <string.h>

/* append t into s, s must be big enough */
void my_strcat(char *s, char *t) {
  for (; *s; s++)
    ;
  for (; (*s = *t) != '\0'; s++, t++)
    ;
}

/* return 1 if s end at t, otherwise return 0*/
int my_strend(char *s, char *t) {
  if (strlen(s) < strlen(t))
    return 0;

  char *t_end = t;
  for (; *t_end; t_end++)
    ;
  for (; *s; s++)
    ;
  for (; t_end >= t; t_end--, s--)
    if (*t_end != *s)
      return 0;
  return 1;
}

int main() {
  char s[30] = "hello world";
  char *t = " Jane";
  my_strcat(s, t);
  printf("strcat: '%s' - Expected: 'hello world Jane'\n", s);

  char *ch1 = "foobar123";
  char *ch2 = "123";
  char *ch3 = "423";

  printf("my_strend: '%d' - Expected: 1\n", my_strend(ch1, ch2));
  printf("my_strend: '%d' - Expected: 0\n", my_strend(ch1, ch3));

  return 0;
}