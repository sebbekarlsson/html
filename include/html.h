#ifndef HTML_H
#define HTML_H
#include <AST.h>
#include <lexer.h>
#include <parser.h>
#include <token.h>

typedef HTMLAST HTMLNode;

HTMLNode *html(char *src);

float html_get_value_number(HTMLNode *node);
char *html_get_value_str(HTMLNode *node);
char **html_get_propnames(HTMLNode *node, int *len);
float html_get_propvalue_number(HTMLNode *node, char *propname);
char *html_get_propvalue_str(HTMLNode *node, char *propname);

void html_free(HTMLNode *node);
#endif
