#include <HTMLAST.h>
#include <html.h>
#include <stdlib.h>
#include <string.h>

HTMLNode *html(char *src) {
  HTMLLexer *lexer = init_html_lexer(src);
  HTMLParser *parser = init_html_parser(lexer);
  HTMLNode *root = html_parser_parse(parser);

  html_lexer_free(lexer);
  html_parser_free(parser);

  return root;
}

float html_get_value_number(HTMLNode *node) { return node->value_float; }
char *html_get_value_str(HTMLNode *node) {
  if (!node->value_str)
    return 0;
  return node->value_str->value;
}

void html_free(HTMLNode *node) { html_ast_free(node); }

char **html_get_propnames(HTMLNode *node, int *len) {
  *len = 0;
  if (!node->options || !node->options->items)
    return 0;
  char **strlist = (char **)calloc(node->options->length, sizeof(char *));
  int l = 0;

  for (int i = 0; i < node->options->length; i++) {
    HTMLNode *op = (HTMLNode *)node->options->items[i];
    if (!op || !op->left)
      continue;
    HTMLNode *left = op->left;
    char *name = html_get_value_str(left);
    if (!name)
      continue;

    strlist[l] = strdup(name);
    l++;
  }

  *len = l;

  return strlist;
}

float html_get_propvalue_number(HTMLNode *node, char *propname) {
  if (!node || !propname)
    return 0;
  for (int i = 0; i < node->options->length; i++) {
    HTMLNode *op = (HTMLNode *)node->options->items[i];
    if (!op || !op->left)
      continue;
    HTMLNode *left = op->left;
    HTMLNode *right = op->right;
    if (!right)
      continue;

    char *name = html_get_value_str(left);
    if (!name)
      continue;
    if (strcmp(name, propname) == 0) {
      return html_get_value_number(right);
    }
  }

  return 0;
}
char *html_get_propvalue_str(HTMLNode *node, char *propname) {
  if (!node || !propname)
    return 0;
  for (int i = 0; i < node->options->length; i++) {
    HTMLNode *op = (HTMLNode *)node->options->items[i];
    if (!op || !op->left)
      continue;
    HTMLNode *left = op->left;
    HTMLNode *right = op->right;
    if (!right)
      continue;

    char *name = html_get_value_str(left);
    if (!name)
      continue;
    if (strcmp(name, propname) == 0) {
      return html_get_value_str(right);
    }
  }

  return 0;
}
