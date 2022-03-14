#ifndef XHTML_AST_H
#define XHTML_AST_H
#include <stdint.h>
#include <hashmap/map.h>

typedef enum  {
HTML_UNIT_PERCENT,
HTML_UNIT_PX,
} EHTMLUnit;

typedef struct HTMLAST_STRUCT {
  enum {
    HTML_AST_ELEMENT,
    HTML_AST_STR,
    HTML_AST_NUMBER,
    HTML_AST_COMPOUND,
    HTML_AST_ASSIGNMENT,
    HTML_AST_COMMENT,
    HTML_AST_STR_ELEMENT,
    HTML_AST_ID,
    HTML_AST_COMPUTE,
    HTML_AST_DOCTYPE
  } type;

  float value_float;
  char* name;
  char *value_str;

  struct HTMLASTLIST_STRUCT *options;
  struct HTMLASTLIST_STRUCT *children;
  struct HTMLAST_STRUCT *doctype;
  struct HTMLAST_STRUCT *child;
  struct HTMLAST_STRUCT *left;
  struct HTMLAST_STRUCT *right;
  struct HTMLAST_STRUCT *closing;
  struct HTMLAST_STRUCT *parent;
  struct HTMLAST_STRUCT *next;
  struct HTMLAST_STRUCT *head; // <!doctype html ...>

  unsigned int is_closed;
  unsigned int is_complete;
  unsigned int is_end;
  unsigned int is_self_closing;
  unsigned int is_comment;
  unsigned int is_doctype;
  map_T* props;
  unsigned int render_end;
  EHTMLUnit unit;
  char c;
} HTMLAST;

typedef struct HTMLASTLIST_STRUCT {
  HTMLAST **items;
  uint32_t length;
} HTMLASTList;

HTMLAST *init_html_ast(int type);
HTMLAST *init_html_ast_with_name(int type, const char* name);

void html_ast_free(HTMLAST *ast);

HTMLASTList *init_html_ast_list();
void html_ast_list_free(HTMLASTList *list);

void html_ast_list_append(HTMLASTList *list, HTMLAST *ast);
void html_ast_list_clear_and_free_items(HTMLASTList *list);

#endif
