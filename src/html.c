#include <HTMLAST.h>
#include <html.h>
#include <html_utils.h>
#include <stdio.h>
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
  if (node == 0)
    return 0;
  if (!node->value_str)
    return 0;
  return node->value_str;
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

  if (strcmp(propname, "innerText") == 0) {
    return html_to_string(node, 1);
  }

  return 0;
}

HTMLNode *html_get_value(HTMLNode *node, char *key) {
  if (!node || !key || !node->options || !node->options->items ||
      !node->options->length)
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

void html_set_propvalue_str(HTMLNode *node, char *propname, char *value) {
  HTMLNode *val = html_get_value(node, propname);

  if (val != 0) {
    if (val->value_str) {
      free(val->value_str);
    }

    if (value != 0) {
      val->value_str = strdup(value);
    }

    return;
  }

  if (!node->options) {
    node->options = init_html_ast_list();
  }

  HTMLNode *opt = init_html_ast(HTML_AST_ASSIGNMENT);
  HTMLNode *left = init_html_ast(HTML_AST_ID);
  left->value_str = strdup(propname);
  HTMLNode *right = init_html_ast(HTML_AST_STR);
  right->value_str = value != 0 ? strdup(value) : 0;
  opt->left = left;
  opt->right = right;
  html_ast_list_append(node->options, opt);
}

void html_set_propvalue_number(HTMLNode *node, char *propname, float value) {
  HTMLNode *val = html_get_value(node, propname);

  if (val != 0) {
    val->value_float = value;

    return;
  }

  if (!node->options) {
    node->options = init_html_ast_list();
  }
  HTMLNode *opt = init_html_ast(HTML_AST_ASSIGNMENT);
  HTMLNode *left = init_html_ast(HTML_AST_ID);
  left->value_str = strdup(propname);
  HTMLNode *right = init_html_ast(HTML_AST_NUMBER);
  right->value_float = value;
  opt->left = left;
  opt->right = right;
  html_ast_list_append(node->options, opt);
}

char *html_options_to_string(HTMLASTList *options, unsigned int skip_tags) {
  if (!options || (options && (options->items == 0 || options->length <= 0)))
    return strdup("");

  char *str = 0;

  for (uint32_t i = 0; i < options->length; i++) {
    HTMLNode *opt = options->items[i];
    char *optstr = html_to_string(opt, skip_tags);

    if (optstr) {
      html_str_append(&str, optstr);
      free(optstr);
    }

    if (i < options->length - 1) {
      html_str_append(&str, " ");
    }
  }

  return str ? str : strdup("");
}

char *html_list_to_string(HTMLASTList *list, unsigned int skip_tags) {
  HTMLASTList *children = list;
  if (!children ||
      (children && (children->items == 0 || children->length <= 0)))
    return strdup("");

  char *str = 0;

  for (uint32_t i = 0; i < children->length; i++) {
    HTMLNode *child = children->items[i];
    char *childstr = html_to_string(child, skip_tags);

    if (childstr) {
      html_str_append(&str, childstr);
      free(childstr);
    }
  }

  return str ? str : strdup("");
}

char *html_element_to_string(HTMLNode *node, unsigned int skip_tags) {
  char *str = 0;

  if (skip_tags == 0) {
    html_str_append(&str, "<");
    if (node->value_str) {
      html_str_append(&str, node->value_str);
    }
    if (node->options && node->options->length) {
      char *optionstr = html_options_to_string(node->options, skip_tags);
      if (optionstr) {
        html_str_append(&str, " ");
        html_str_append(&str, optionstr);
        free(optionstr);
      }
    }

    html_str_append(&str, ">");
  }

  if (node->children) {
    char *childrenstr = html_list_to_string(node->children, skip_tags);
    if (childrenstr) {
      html_str_append(&str, childrenstr);
      free(childrenstr);
    }
  }

  if (skip_tags == 0) {
    if (node->is_self_closing == 0) {
      html_str_append(&str, "<");
      html_str_append(&str, "/");
      if (node->value_str) {
        html_str_append(&str, node->value_str);
      }
      html_str_append(&str, ">");
    }
  }

  return str ? str : strdup("");
}

char *_html_str_to_string(HTMLNode *node, unsigned int skip_tags) {
  return node->value_str ? strdup(node->value_str) : strdup("");
}

char *html_str_to_string(HTMLNode *node, unsigned int skip_tags) {
  char *s = 0;
  html_str_append(&s, "\"");
  char *value = node->value_str ? strdup(node->value_str) : strdup("");
  if (value) {
    html_str_append(&s, value);
    free(value);
  }
  html_str_append(&s, "\"");
  return s;
}
char *html_number_to_string(HTMLNode *node, unsigned int skip_tags) {
  char buff[256];
  sprintf(buff, "%1.2f", node->value_float);
  return strdup(buff);
}
char *html_compound_to_string(HTMLNode *node, unsigned int skip_tags) {
  return html_list_to_string(node->children, skip_tags);
}
char *html_assignment_to_string(HTMLNode *node, unsigned int skip_tags) {
  char *str = 0;

  if (node->left) {
    char *leftstr = html_to_string(node->left, skip_tags);
    if (leftstr) {
      html_str_append(&str, leftstr);
      free(leftstr);
    }
  }

  if (node->right) {
    html_str_append(&str, "=");

    char *rightstr = html_to_string(node->right, skip_tags);
    if (rightstr) {
      html_str_append(&str, rightstr);
      free(rightstr);
    }
  }

  return str ? str : strdup("");
}
char *html_str_element_to_string(HTMLNode *node, unsigned int skip_tags) {
  return _html_str_to_string(node, skip_tags);
}
char *html_id_to_string(HTMLNode *node, unsigned int skip_tags) {
  return _html_str_to_string(node, skip_tags);
}

char *html_to_string(HTMLNode *node, unsigned int skip_tags) {
  if (!node)
    return strdup("");

  switch (node->type) {
  case HTML_AST_ELEMENT:
    return html_element_to_string(node, skip_tags);
    break;
  case HTML_AST_STR:
    return html_str_to_string(node, skip_tags);
    break;
  case HTML_AST_NUMBER:
    return html_number_to_string(node, skip_tags);
    break;
  case HTML_AST_COMPOUND:
    return html_compound_to_string(node, skip_tags);
    break;
  case HTML_AST_ASSIGNMENT:
    return html_assignment_to_string(node, skip_tags);
    break;
  case HTML_AST_STR_ELEMENT:
    return html_str_element_to_string(node, skip_tags);
    break;
  case HTML_AST_ID:
    return html_id_to_string(node, skip_tags);
    break;
  default: { return strdup(""); }
  }

  return strdup("");
}
