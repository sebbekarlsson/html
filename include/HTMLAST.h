#ifndef XHTML_AST_H
#define XHTML_AST_H
#include <stdint.h>

typedef struct HTMLAST_STRUCT {
  enum {
    HTML_AST_ELEMENT,
    HTML_AST_STR,
    HTML_AST_NUMBER,
    HTML_AST_COMPOUND,
    HTML_AST_ASSIGNMENT,
    HTML_AST_STR_ELEMENT,
    HTML_AST_ID,
    HTML_AST_COMPUTE
  } type;

  float value_float;
  char *value_str;

  struct HTMLASTLIST_STRUCT *options;
  struct HTMLASTLIST_STRUCT *children;
  struct HTMLAST_STRUCT *child;
  struct HTMLAST_STRUCT *left;
  struct HTMLAST_STRUCT *right;
  struct HTMLAST_STRUCT *closing;
  struct HTMLAST_STRUCT *parent;

  unsigned int is_closed;
  unsigned int is_complete;
  unsigned int is_end;
  unsigned int is_self_closing;
} HTMLAST;

typedef struct HTMLASTLIST_STRUCT {
  HTMLAST **items;
  uint32_t length;
} HTMLASTList;

HTMLAST *init_html_ast(int type);

void html_ast_free(HTMLAST *ast);

HTMLASTList *init_html_ast_list();
void html_ast_list_free(HTMLASTList *list);

void html_ast_list_append(HTMLASTList *list, HTMLAST *ast);
void html_ast_list_clear_and_free_items(HTMLASTList *list);

#endif
