#ifndef XXHTML_H
#define XXHTML_H
#ifdef __cplusplus
extern "C" {
#endif

#include <html/html_lexer.h>
#include <html/html_parser.h>
  #include <html/html_options.h>

  typedef struct HTMLAST_STRUCT HTMLNode;



HTMLNode *html(char *src, HTMLOptions* options);

float html_get_value_number(HTMLNode *node);
char *html_get_value_str(HTMLNode *node);
char **html_get_propnames(HTMLNode *node, int *len);
float html_get_propvalue_number(HTMLNode *node, char *propname);
char *html_get_propvalue_str(HTMLNode *node, char *propname);
  HTMLNode *html_get_value(HTMLNode *node, char *key);

  const char* html_get_name(HTMLNode* node);

  HTMLNode* html_find_tag_by_name(HTMLNode* html, const char* name);

void html_find_tags_by_name(HTMLNode* html, const char* name, HTMLASTList* list);


void html_set_propvalue_str(HTMLNode *node, char *propname, char *value);
  void html_set_propvalue_number(HTMLNode *node, char *propname, float value);

unsigned int html_is_text(HTMLNode* node);

char *html_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent);

void html_free(HTMLNode *node);
#ifdef __cplusplus
}
#endif

#endif
