#ifndef XXHTML_H
#define XXHTML_H
#ifdef __cplusplus
extern "C" {
#endif

#include <html_lexer.h>
#include <html_parser.h>

typedef struct HTMLAST_STRUCT HTMLNode;

HTMLNode *html(char *src);

float html_get_value_number(HTMLNode *node);
char *html_get_value_str(HTMLNode *node);
char **html_get_propnames(HTMLNode *node, int *len);
float html_get_propvalue_number(HTMLNode *node, char *propname);
char *html_get_propvalue_str(HTMLNode *node, char *propname);
HTMLNode *html_get_value(HTMLNode *node, char *key);

void html_set_propvalue_str(HTMLNode *node, char *propname, char *value);
void html_set_propvalue_number(HTMLNode *node, char *propname, float value);

char *html_to_string(HTMLNode *node, unsigned int skip_tags);

void html_free(HTMLNode *node);
#ifdef __cplusplus
}
#endif

#endif
