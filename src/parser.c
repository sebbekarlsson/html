#include <html.h>
#include <html_parser.h>
#include <html_utils.h>
#include <stdio.h>
#include <stdlib.h>

HTMLParser *init_html_parser(HTMLLexer *lexer) {
  HTMLParser *parser = (HTMLParser *)calloc(1, sizeof(HTMLParser));
  parser->lexer = lexer;
  parser->token = html_lexer_get_next_token(lexer);
  parser->parent = 0;

  return parser;
}

void html_parser_free(HTMLParser *parser) {
  if (parser->token)
    html_token_free(parser->token);

  free(parser);
}

void html_parser_eat(HTMLParser *parser, int token_type) {
  if (parser->token->type != token_type) {
    printf("Parser: Unexpected token `%s` (%d), was expecting (%d)\n",
           parser->token->value, parser->token->type, token_type);
    exit(1);
  } else {
    if (parser->token != 0) {
      html_token_free(parser->token);
      parser->token = 0;
    }
    parser->token = html_lexer_get_next_token(parser->lexer);
  }
}

HTMLAST *html_parser_parse(HTMLParser *parser) {
  return html_parser_parse_expr(parser, 0);
  // return html_parser_parse_compound(parser);
}

HTMLAST *html_parser_parse_compound(HTMLParser *parser, HTMLAST *parent) {
  HTMLAST *ast = init_html_ast(HTML_AST_COMPOUND);
  ast->children = init_html_ast_list();

  while (parser->token->type != HTML_TOKEN_EOF) {
    HTMLAST *child = 0;
    if (parser->token->type != HTML_TOKEN_LT) {
      child = html_parser_parse_raw(parser, parent);
    } else {
      child = html_parser_parse_expr(parser, parent);
    }

    if (child == 0)
      continue;

    html_ast_list_append(ast->children, child);
  }

  return ast;
}

static void collect_options(HTMLParser *parser, HTMLAST *ast, HTMLAST *parent) {
  if (parser->token->type == HTML_TOKEN_ID) {
    ast->options = init_html_ast_list();
    while (parser->token->type == HTML_TOKEN_ID) {
      HTMLAST *child = html_parser_parse_assignment(parser, parent);
      html_ast_list_append(ast->options, child);

      if (child->left && child->left->value_str != 0) {
        map_set(ast->props, child->left->value_str, child->right);
      }
    }
  }
}

HTMLAST *html_parser_parse_element(HTMLParser *parser, HTMLAST *parent) {
  if (parser->token->type != HTML_TOKEN_EOF &&
      parser->token->type != HTML_TOKEN_LT &&
      parser->token->type != HTML_TOKEN_GT) {
    return html_parser_parse_raw(parser, parent);
  }

  if (parser->token->type != HTML_TOKEN_LT)
    return 0;
  HTMLAST *ast = init_html_ast(HTML_AST_ELEMENT);
  ast->children = init_html_ast_list();

  html_parser_eat(parser, HTML_TOKEN_LT); // <
  if (parser->token->type == HTML_TOKEN_DIV) {
    html_parser_eat(parser, HTML_TOKEN_DIV); // /
    ast->is_end = 1;
  } else {
    ast->parent = parent;
  }

  char *extra = 0;
  if (parser->token->type == HTML_TOKEN_EXCL && parser->token->value) {
    extra = strdup(parser->token->value);
    html_parser_eat(parser, HTML_TOKEN_EXCL);
  }
  char *name = 0;
  if (extra) {
    html_str_append(&name, extra);
    free(extra);
  }
  if (parser->token->value) {
    html_str_append(&name, parser->token->value);
  }
  ast->value_str = name;

  if (parser->token->type == HTML_TOKEN_DOCTYPE) {

    html_parser_eat(parser, HTML_TOKEN_DOCTYPE);
    ast->is_self_closing = 1;
    ast->is_end = 1;
    ast->is_comment = 1;
    ast->is_doctype = 1;
  } else {
    html_parser_eat(parser, HTML_TOKEN_ID); // div
  }

  collect_options(parser, ast, ast);

  if (parser->token->type == HTML_TOKEN_DIV) {
    ast->is_self_closing = 1;
    html_parser_eat(parser, HTML_TOKEN_DIV);
  }

  html_parser_eat(parser, HTML_TOKEN_GT); // >

  if (!ast->is_end) {

    if (!ast->is_self_closing) {
      HTMLAST *child = html_parser_parse_element(parser, ast);

      if (child) {
        HTMLASTList *trash = init_html_ast_list();
        if (child->type != HTML_AST_STR &&
            child->type != HTML_AST_STR_ELEMENT) {

          if (child->parent == 0) {
            html_ast_list_append(trash, child);
          }
          char *child_name = html_get_value_str(child);
          while (name && child && child_name &&
                 !(child->is_end && strcmp(name, child_name) == 0)) {

            child = html_parser_parse_element(parser, ast);
            child_name = html_get_value_str(child);

            if (child && child->parent == 0) {
              html_ast_list_append(trash, child);
            }
          }

        } else {
          if (!ast->children)
            ast->children = init_html_ast_list();
          html_ast_list_append(ast->children, child);
        }

        html_ast_list_free(trash);
      }
    }
    if (parent && parent != ast) {
      if (!parent->children)
        parent->children = init_html_ast_list();
      html_ast_list_append(parent->children, ast);
    }
  }

  if (ast->is_doctype) {
    HTMLAST* document = init_html_ast(HTML_AST_COMPOUND);
    document->value_str = strdup("root");
    document->children = init_html_ast_list();
    html_ast_list_append(document->children, html_parser_parse_expr(parser, parent));
    return document;
  }

  return ast;
}

HTMLAST *html_parser_parse_string_element(HTMLParser *parser, HTMLAST *parent) {
  char *v = strdup(parser->token->value);

  int type = HTML_AST_STR_ELEMENT;

  if (parser->token->type == HTML_TOKEN_STR) {
    html_parser_eat(parser, HTML_TOKEN_STR);
  } else {
    html_parser_eat(parser, HTML_TOKEN_COMPUTE);
    type = HTML_AST_COMPUTE;
  }
  HTMLAST *ast = init_html_ast(type);
  ast->value_str = strdup(v);
  free(v);

  // ast->sibling = html_parser_parse_factor(parser, parent);
  return ast;
}

HTMLAST *html_parser_parse_raw(HTMLParser *parser, HTMLAST *parent) {
  if (parser->token->type == HTML_TOKEN_LT || parser->lexer->c == 0)
    return 0;

  unsigned int allow_compute = 1;
  if (parent != 0 && parent->value_str != 0) {
    allow_compute = strcmp(parent->value_str, "style") != 0;
  }

  HTMLToken *tok = html_lexer_parse_string_until(parser->lexer, '<', allow_compute ? '{' : 0, allow_compute);
  char *buff = strdup(parser->token->value);
  html_str_append(&buff, tok->value);

  if (tok->value) {
    free(tok->value);
    tok->value = 0;
  }
  tok->value = buff;

  if (parser->token) {
    html_token_free(parser->token);
    parser->token = 0;
  }

  parser->token = tok;

  HTMLAST *ast = html_parser_parse_string_element(parser, parent);
  return ast;
}

HTMLAST *html_parser_parse_factor(HTMLParser *parser, HTMLAST *parent) {
  HTMLAST *left = 0;

  if (parser->token->type == HTML_TOKEN_LT) {
    left = html_parser_parse_element(parser, parent);
  }

  if (left)
    return left;

  HTMLAST *nextast = 0;
  // HTMLASTList *nextchildren = init_html_ast_list();
  switch (parser->token->type) {
  case HTML_TOKEN_STR:
    nextast = html_parser_parse_string(parser, parent);

    break;
  case HTML_TOKEN_NUMBER:
    nextast = html_parser_parse_number(parser, parent);
    break;
  default: {
    if (parser->token->type != HTML_TOKEN_EOF &&
        parser->token->type != HTML_TOKEN_LT) {
      nextast = html_parser_parse_compound(parser, parent);
    }
  } break;
  }

  left = nextast;

  /*
    if (left != 0 && nextast != 0) {
      if (left->is_closed || left->is_self_closing || left->is_complete) {
        if (left->parent) {
          if (left->parent->children == 0) {
            left->parent->children = init_html_ast_list();
          }
          html_ast_list_append(left->parent->children,  nextast);
        }
        //left->sibling =  nextast;
      } else {
        if (left->children == 0) {
          left->children = init_html_ast_list();
        }
        html_ast_list_append(left->children,  nextast);
      }
    }*/

  return left;
}

HTMLAST *html_parser_parse_term(HTMLParser *parser, HTMLAST *parent) {

  HTMLAST *left = html_parser_parse_factor(parser, parent);

  return left;
}

HTMLAST *html_parser_parse_expr(HTMLParser *parser, HTMLAST *parent) {
  HTMLAST *left = html_parser_parse_term(parser, parent);

  return left;
}
HTMLAST *html_parser_parse_number(HTMLParser *parser, HTMLAST *parent) {
  float v = atof(parser->token->value);
  html_parser_eat(parser, HTML_TOKEN_NUMBER);
  HTMLAST *ast = init_html_ast(HTML_AST_NUMBER);
  ast->value_float = v;
  return ast;
}

HTMLAST *html_parser_parse_string(HTMLParser *parser, HTMLAST *parent) {
  char *v = strdup(parser->token->value);
  html_parser_eat(parser, HTML_TOKEN_STR);
  HTMLAST *ast = init_html_ast(HTML_AST_STR);
  ast->value_str = strdup(v);
  free(v);
  return ast;
}

HTMLAST *html_parser_parse_id(HTMLParser *parser, HTMLAST *parent) {
  char *v = strdup(parser->token->value);
  html_parser_eat(parser, HTML_TOKEN_ID);
  HTMLAST *ast = init_html_ast(HTML_AST_ID);
  ast->value_str = strdup(v);
  free(v);
  return ast;
}

HTMLAST *html_parser_parse_assignment(HTMLParser *parser, HTMLAST *parent) {
  HTMLAST *ast = init_html_ast(HTML_AST_ASSIGNMENT);
  ast->left = html_parser_parse_id(parser, parent);
  if (parser->token->type == HTML_TOKEN_EQUALS) {
    html_parser_eat(parser, HTML_TOKEN_EQUALS);
    ast->right = html_parser_parse_factor(parser, parent);
  }

  return ast;
}
