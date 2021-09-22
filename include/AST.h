#ifndef HTML_AST_H
#define HTML_AST_H
#include <easystr.h>
#include <stdint.h>

typedef struct HTMLAST_STRUCT {
  enum {
    HTML_AST_ELEMENT,
    HTML_AST_STR,
    HTML_AST_NUMBER,
    HTML_AST_COMPOUND,
    HTML_AST_ASSIGNMENT,
    HTML_AST_ID
  } type;

  float value_float;
  EStr *value_str;

  struct HTMLASTLIST_STRUCT *options;
  struct HTMLASTLIST_STRUCT *children;
  struct HTMLAST_STRUCT *child;
  struct HTMLAST_STRUCT *left;
  struct HTMLAST_STRUCT *right;
} HTMLAST;

typedef struct HTMLASTLIST_STRUCT {
  HTMLAST **items;
  uint32_t length;
} HTMLASTList;

HTMLAST *init_html_ast(int type);

void html_ast_free(HTMLAST *ast);

HTMLASTList *init_html_ast_list();

void html_ast_list_append(HTMLASTList *list, HTMLAST *ast);

#endif
