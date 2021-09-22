#ifndef XXHTML_H
#define XXHTML_H
#include <html_lexer.h>
#include <html_parser.h>

typedef struct HTMLAST_STRUCT HTMLNode;

HTMLNode *html(char *src);

float html_get_value_number(HTMLNode *node);
char *html_get_value_str(HTMLNode *node);
char **html_get_propnames(HTMLNode *node, int *len);
float html_get_propvalue_number(HTMLNode *node, char *propname);
char *html_get_propvalue_str(HTMLNode *node, char *propname);

void html_free(HTMLNode *node);
#endif
