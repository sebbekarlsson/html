#include <html/HTMLAST.h>
#include <html/html.h>
#include <html/html_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HTMLNode *html(char *src, HTMLOptions* options) {
  HTMLLexer *lexer = init_html_lexer(src);
  HTMLParser *parser = init_html_parser(lexer, options);
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
  if (!node || !propname)
    return 0;


  HTMLNode* prop = (HTMLNode*) map_get_value(node->props, propname);
  if (prop != 0 && prop->value_float != 0) return prop->value_float;


  char* strval = html_get_propvalue_str(node, propname);
  if (strval) {
    return atof(strval);
  }

  if (!node->options) return 0;

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

  HTMLNode* prop = (HTMLNode*) map_get_value(node->props, propname);

  if (prop != 0) return prop->value_str;

  if (node->options && node->options->items) {
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
  }

  if (strcmp(propname, "innerText") == 0) {
    return html_to_string(node, 1, 0);
  }

  return 0;
}

HTMLNode *html_get_value(HTMLNode *node, char *key) {
  if (!node || !key || !node->options || !node->options->items ||
      !node->options->length)
    return 0;

  HTMLNode* prop = (HTMLNode*) map_get_value(node->props, key);
  if (prop != 0) return prop;

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
  map_set(node->props, propname, right);
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

  map_set(node->props, propname, right);
}

char* generate_indent(unsigned int n) {
  if (n == 0) return strdup("");
  unsigned int t = n > 0 ? n : 1;
  char* buff = (char*)calloc(t + 1, sizeof(char*));

  memset(buff, '\t', n);
  return buff;
}

char *html_options_to_string(HTMLASTList *options, unsigned int skip_tags, unsigned int indent) {
  if (!options || (options && (options->items == 0 || options->length <= 0)))
    return strdup("");

  char *str = 0;

  for (uint32_t i = 0; i < options->length; i++) {
    HTMLNode *opt = options->items[i];
    char *optstr = html_to_string(opt, skip_tags, indent);

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

char *html_list_to_string(HTMLASTList *list, unsigned int skip_tags, unsigned int indent) {
  HTMLASTList *children = list;
  if (!children ||
      (children && (children->items == 0 || children->length <= 0)))
    return strdup("");

  char *str = 0;

  for (uint32_t i = 0; i < children->length; i++) {
    HTMLNode *child = children->items[i];
    char *childstr = html_to_string(child, skip_tags, indent);

    if (childstr) {
      html_str_append(&str, childstr);
      free(childstr);
    }
  }

  return str ? str : strdup("");
}

char *html_element_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  char *str = 0;

  //char* indentstr = generate_indent(indent);
  //html_str_append(&str, indentstr);

   // html_str_append(&str, "HTML(");
  if (skip_tags == 0) {
    html_str_append(&str, "<");
    if (node->value_str) {
      html_str_append(&str, node->value_str);
    }
    if (node->options && node->options->length) {
      char *optionstr = html_options_to_string(node->options, skip_tags, 0);
      if (optionstr) {
        html_str_append(&str, " ");
        html_str_append(&str, optionstr);
        free(optionstr);
      }
    }

    if (node->is_self_closing)
      html_str_append(&str, "/");

    html_str_append(&str, ">");
  }
  //html_str_append(&str, "\n");

  if (node->children) {
    char *childrenstr = html_list_to_string(node->children, skip_tags, indent+1);
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

  //html_str_append(&str, ")");

  //html_str_append(&str, "\n");
  //free(indentstr);

  return str ? str : strdup("");
}

char *_html_str_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  return node->value_str ? strdup(node->value_str) : strdup("");
}

char *html_str_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  char *s = 0;
  html_str_append_char(&s, node->c);
  char *value = node->value_str ? strdup(node->value_str) : strdup("");
  if (value) {
    html_str_append(&s, value);
    free(value);
  }
  html_str_append_char(&s, node->c);

  return s;
}
char *html_number_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  char buff[256];
  sprintf(buff, "%1.2f", node->value_float);
  return strdup(buff);
}
char *html_compound_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  return html_list_to_string(node->children, skip_tags, indent);
}
char *html_assignment_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  char *str = 0;

  if (node->left) {
    char *leftstr = html_to_string(node->left, skip_tags, 0);
    if (leftstr) {
      html_str_append(&str, leftstr);
      free(leftstr);
    }
  }

  if (node->right) {
    html_str_append(&str, "=");

    char *rightstr = html_to_string(node->right, skip_tags, 0);
    if (rightstr) {
      html_str_append(&str, rightstr);
      free(rightstr);
    }
  }

  return str ? str : strdup("");
}
char *html_str_element_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  char* str = 0;
  //char* indentstr = generate_indent(indent);
 // html_str_append(&str, indentstr);

#if 0
  html_str_append(&str, "TEXT(");
  char* innerstr =  _html_str_to_string(node, skip_tags, indent);
  html_str_append(&str, innerstr);
  html_str_append(&str, ")");
#else
  char* innerstr =  _html_str_to_string(node, skip_tags, indent);
  html_str_append(&str, innerstr);
  #endif

  //free(indentstr);

  return str;
}
char *html_id_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  return _html_str_to_string(node, skip_tags, 0);
}

char *html_to_string(HTMLNode *node, unsigned int skip_tags, unsigned int indent) {
  if (!node)
    return strdup("");

  switch (node->type) {
  case HTML_AST_ELEMENT:
    return html_element_to_string(node, skip_tags, indent);
    break;
  case HTML_AST_STR:
    return html_str_to_string(node, skip_tags, indent);
    break;
  case HTML_AST_NUMBER:
    return html_number_to_string(node, skip_tags, indent);
    break;
  case HTML_AST_COMPOUND:
    return html_compound_to_string(node, skip_tags, indent);
    break;
  case HTML_AST_ASSIGNMENT:
    return html_assignment_to_string(node, skip_tags, 0);
    break;
  case HTML_AST_STR_ELEMENT:
    return html_str_element_to_string(node, skip_tags, indent);
    break;
  case HTML_AST_COMMENT:
  case HTML_AST_ID:
    return html_id_to_string(node, skip_tags, 0);
    break;
  default: { return strdup(""); }
  }

  return strdup("");
}

const char* html_get_name(HTMLNode* node) {
  if (!node) return 0;
  if (node->name) return node->name;
  return node->value_str;
}

unsigned int html_is_text(HTMLNode* node) {
  return node->type == HTML_AST_STR || node->type == HTML_AST_STR_ELEMENT;
}

HTMLNode* html_find_tag_by_name(HTMLNode* html, const char* name) {
  if (!html) return 0;

  const char* my_name = html_get_name(html);
  if (name && strcmp(my_name, name) == 0) return html;

  HTMLASTList *children = html->children;
  if (!children) return 0;
  char *str = 0;

  for (uint32_t i = 0; i < children->length; i++) {
    HTMLNode *child = children->items[i];
    if (!child) continue;
    if (!child->value_str) continue;
    if (strcmp(child->value_str, name) == 0) return child;
  }

  return 0;
}


void html_find_tags_by_name(HTMLNode* html, const char* name, HTMLASTList* list) {
    HTMLASTList *children = html->children;
  char *str = 0;

  HTMLNode** _nodes = 0;
  uint32_t len = 0;

  if (html->value_str && strcmp(html->value_str, name) == 0) {
    html_ast_list_append(list, html);
  }

  if (html->child && html->child->value_str && strcmp(html->child->value_str, name) == 0) {
    html_ast_list_append(list, html->child);
  }

  if (children) {
    for (uint32_t i = 0; i < children->length; i++) {
      HTMLNode *child = children->items[i];
      if (!child) continue;
      if (!child->value_str) continue;
      if (strcmp(child->value_str, name) == 0) {
        html_ast_list_append(list, child);
      }

      if (child->children) {
        html_find_tags_by_name(child, name, list);
      }
    }
  }

}
