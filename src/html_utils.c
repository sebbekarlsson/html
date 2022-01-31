#include <html/html_utils.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void html_str_append(char **s1, char *s2) {
  char *s = *s1;
  size_t len = s ? strlen(s) : 0;
  size_t newlen = len + (s2 ? strlen(s2) : 0);

  s = (char *)realloc(s, (newlen + 1) * sizeof(char));
  s[len] = 0;
  strcat(s, s2);

  *s1 = s;
}

char *str_from_char(char c) {
  char *buff = (char *)calloc(2, sizeof(char));
  buff[0] = c;
  buff[1] = '\0';
  return buff;
}

void html_str_append_char(char **s1, char c) {
  char *strchar = str_from_char(c);
  html_str_append(s1, strchar);
  free(strchar);
}
