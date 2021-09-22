#include <AST.h>
#include <stdlib.h>

HTMLAST *init_html_ast(int type) {
  HTMLAST *ast = (HTMLAST *)calloc(1, sizeof(HTMLAST));
  ast->type = type;
  ast->value_float = 0;
  return ast;
}

void html_ast_free(HTMLAST *ast) {
  if (!ast)
    return;
  if (ast->value_str)
    e_free(ast->value_str);

  if (ast->children) {
    for (int i = 0; i < ast->children->length; i++) {
      html_ast_free(ast->children->items[i]);
    }

    free(ast->children->items);
    free(ast->children);
  }

  if (ast->options) {
    for (int i = 0; i < ast->options->length; i++) {
      html_ast_free(ast->options->items[i]);
    }

    free(ast->options->items);
    free(ast->options);
  }

  if (ast->child)
    html_ast_free(ast->child);

  if (ast->left)
    html_ast_free(ast->left);

  if (ast->right)
    html_ast_free(ast->right);

  free(ast);
}

HTMLASTList *init_html_ast_list() {
  HTMLASTList *list = (HTMLASTList *)calloc(1, sizeof(HTMLASTList));
  list->items = 0;
  list->length = 0;
  return list;
}

void html_ast_list_append(HTMLASTList *list, HTMLAST *ast) {
  if (!list)
    return;
  if (!ast)
    return;

  list->length += 1;
  list->items =
      (HTMLAST **)realloc(list->items, list->length * sizeof(HTMLAST *));
  list->items[list->length - 1] = ast;
}
