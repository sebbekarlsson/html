#include <HTMLAST.h>
#include <stdlib.h>

HTMLAST *init_html_ast(int type) {
  HTMLAST *ast = (HTMLAST *)calloc(1, sizeof(HTMLAST));
  ast->type = type;
  ast->value_float = 0;
  ast->is_complete = 0;
  ast->is_end = 0;
  ast->is_self_closing = 0;
  ast->closing = 0;
  return ast;
}

void html_ast_free(HTMLAST *ast) {
  if (!ast)
    return;
  if (ast->value_str) {
    free(ast->value_str);
    ast->value_str = 0;
  }

  if (ast->children) {
    for (int i = 0; i < ast->children->length; i++) {
      HTMLAST *child = ast->children->items[i];

      if (child != 0)
        html_ast_free(child);
    }

    free(ast->children->items);
    free(ast->children);
  }

  if (ast->options) {
    for (int i = 0; i < ast->options->length; i++) {
      HTMLAST *child = ast->options->items[i];

      if (child != 0)
        html_ast_free(child);
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

void html_ast_list_free(HTMLASTList *list) {
  if (!list)
    return;

  if (list->items) {
    for (uint32_t i = 0; i < list->length; i++) {
      HTMLAST *child = list->items[i];
      if (child != 0)
        html_ast_free(child);
    }

    free(list->items);
  }
  free(list);
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

void html_ast_list_clear_and_free_items(HTMLASTList *list) {
  if (!list || !list->length || !list->items)
    return;

  for (int i = 0; i < list->length; i++) {
    html_ast_free(list->items[i]);
  }

  free(list->items);
  list->length = 0;
}
