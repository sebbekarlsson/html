#include <HTMLAST.h>
#include <html.h>
#include <stdlib.h>
#include <string.h>
#include <easystr.h>

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
  if (!node || !propname || node->options == 0 || node->options->items == 0)
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
  if (!node || !propname || node->options == 0 || node->options->items == 0)
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

HTMLNode* html_get_value(HTMLNode* node, char* key) {
   if (!node || !key || !node->options || !node->options->items || !node->options->length)
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
    if (strcmp(name, key) == 0) {
      return right;
    }
  }

  return 0;
}


void html_set_propvalue_str(HTMLNode* node, char* propname, char* value) {
  HTMLNode* val = html_get_value(node, propname);

  if (val != 0) {
    if (val->value_str) {
      e_free(val->value_str);
    }

    if (value != 0)  {
      val->value_str = e_fromstr(value);
    }

    return;
  }

  if (!node->options) {
      node->options = init_html_ast_list();
  }
  HTMLNode* opt = init_html_ast(HTML_AST_ASSIGNMENT);
  HTMLNode* left = init_html_ast(HTML_AST_ID);
  left->value_str = e_fromstr(propname);
  HTMLNode* right = init_html_ast(HTML_AST_STR);
  right->value_str = value != 0 ? e_fromstr(value) : 0;
  opt->left = left;
  opt->right = right;
  html_ast_list_append(node->options, opt);
}

void html_set_propvalue_number(HTMLNode* node, char* propname, float value) {
  HTMLNode* val = html_get_value(node, propname);

  if (val != 0) {
    val->value_float = value;

    return;
  }

  if (!node->options) {
      node->options = init_html_ast_list();
  }
  HTMLNode* opt = init_html_ast(HTML_AST_ASSIGNMENT);
  HTMLNode* left = init_html_ast(HTML_AST_ID);
  left->value_str = e_fromstr(propname);
  HTMLNode* right = init_html_ast(HTML_AST_NUMBER);
  right->value_float = value;
  opt->left = left;
  opt->right = right;
  html_ast_list_append(node->options, opt);
}

HTMLASTList* html_get_siblings(HTMLNode* node) {
  if (!node) return 0;
  HTMLASTList* list = init_html_ast_list();

  HTMLNode* n = node->sibling;

  while (n) {
    html_ast_list_append(list, n);
    n = n->sibling;
  }

  return list;
}
