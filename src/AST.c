#include <AST.h>
#include <stdlib.h>


HTMLAST* init_html_ast(int type) {
  HTMLAST* ast = (HTMLAST*)calloc(1, sizeof(HTMLAST));
  ast->type = type;
  return ast;
}

HTMLASTList* init_html_ast_list() {
  HTMLASTList* list = (HTMLASTList*)calloc(1, sizeof(HTMLASTList));
  list->items = 0;
  list->length = 0;
  return list;
}

void html_ast_list_append(HTMLASTList* list, HTMLAST* ast) {
  if (!list) return;
  if (!ast) return;

  list->length += 1;
  list->items = (HTMLAST**)realloc(list->items, list->length * sizeof(HTMLAST*));
  list->items[list->length-1] = ast;
}
