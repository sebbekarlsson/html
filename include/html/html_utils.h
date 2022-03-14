#ifndef HTML_UTILS_H
#define HTML_UTILS_H
#include <string.h>
void html_str_append(char **s1, char *s2);
void html_str_append_char(char **s1, char c);
char *str_from_char(char c);
void str_to_lower(const char* str, char* out);

unsigned int str_fuzzy_compare(const char* a, const char* b);
#endif
