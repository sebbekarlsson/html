#include <html/html.h>
#include <html/html_parser.h>
#include <html/html_utils.h>
#include <html/html_tags.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MAX
#define MAX(a, b) (a > b ? a : b)
#endif

#ifndef MIN
#define MIN(a, b) (a < b ? a : b)
#endif

const unsigned int ALLOW_COMPUTE = 1;

HTMLParser *init_html_parser(HTMLLexer *lexer, HTMLOptions* options) {
  HTMLParser *parser = (HTMLParser *)calloc(1, sizeof(HTMLParser));
  parser->lexer = lexer;
  parser->token = html_lexer_get_next_token(lexer);
  parser->parent = 0;
  parser->options = options;

  return parser;
}

void html_parser_free(HTMLParser *parser) {
  if (parser->token)
    html_token_free(parser->token);

  free(parser);
}

void html_parser_eat(HTMLParser *parser, int token_type) {

  if (parser->token->type != token_type) {
    char tmp[256];
    html_lexer_get_lineinfostr(parser->lexer, tmp);
    printf("Parser: Unexpected token `%s` (%s) (%d), was expecting (%d) (%s) [%s]\n",
           parser->token->value, html_token_to_string(parser->token->type), parser->token->type, token_type, html_token_to_string(token_type), tmp);
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

HTMLAST* html_parser_parse_comment(HTMLParser* parser, HTMLAST* parent) {
  HTMLAST* ast = init_html_ast(HTML_AST_COMMENT);
  ast->value_str = strdup(parser->token->value);
  html_parser_eat(parser, HTML_TOKEN_COMMENT);
  return ast;
}


void _html_parser_parse_end_tag(HTMLParser *parser, HTMLAST *parent) {
  html_parser_eat(parser, HTML_TOKEN_LT); // <
  html_parser_eat(parser, HTML_TOKEN_DIV); // /
  html_parser_eat(parser, HTML_TOKEN_ID); // span
  html_parser_eat(parser, HTML_TOKEN_GT); // >
}

HTMLToken* html_parser_simulate_next_token(HTMLParser* parser, int type) {
  uint32_t offset = 0;
  char* buffer = &parser->lexer->src[MAX(0, parser->lexer->i)];
  if (!buffer) return 0;

  HTMLLexer lexer = (HTMLLexer){0};
  lexer.src = strdup(parser->lexer->src);
  lexer.length = parser->lexer->length;
  lexer.c = parser->lexer->c;
  lexer.i = parser->lexer->i;
  HTMLToken* token  = html_lexer_get_next_token(&lexer);

  while (token != 0 && token->type != HTML_TOKEN_EOF) {
    if (token->type == type) {
      break;
    } else {
      html_token_free(token);
      token = html_lexer_get_next_token(&lexer);
    }

  }

  if (token->type != type) {
    free(lexer.src);
    html_token_free(token);
    return 0;
  }

  free(lexer.src);

  return token;
}

unsigned int html_parser_next_tag(HTMLParser* parser, const char* word) {
  if (parser->lexer->i >= parser->lexer->length) return 0;
  uint32_t i = parser->lexer->i;
  char* buffer = &parser->lexer->src[i];
  if (!buffer) return 0;

  uint32_t wordlen = strlen(word);
  uint32_t bufflen = wordlen+1;

  while ((i+wordlen) < parser->lexer->length) {
    char c = buffer[i];
    char c2 = buffer[MIN(i+1, parser->lexer->length-1)];
    if (c == '<' && c2 == '/') {
      i+=2;
      char tmp[bufflen];
      memset(&tmp[0], 0, bufflen*sizeof(char));
      memcpy(&tmp[0], &buffer[i], wordlen*sizeof(char));
      if (strcmp(tmp, word) == 0) return 1;
    }
    i++;
  }

  return 0;
}

unsigned int html_parser_seek_string(HTMLParser* parser, const char* word) {
  if (parser->lexer->i >= parser->lexer->length) return 0;
  uint32_t i = parser->lexer->i;
  char* buffer = &parser->lexer->src[i];
  if (!buffer) return 0;

  uint32_t wordlen = strlen(word);
  uint32_t bufflen = wordlen+1;

  while ((i+wordlen) < parser->lexer->length) {
    char tmp[bufflen];
    memset(&tmp[0], 0, bufflen*sizeof(char));
    memcpy(&tmp[0], &buffer[i], wordlen*sizeof(char));

    if (str_fuzzy_compare(tmp, word)) {
      return 1;
    }

    i++;
  }

  return 0;
}

HTMLAST *html_parser_parse_doctype(HTMLParser *parser, HTMLAST *parent) {
  HTMLAST* ast = init_html_ast(HTML_AST_ELEMENT);

  ast->is_doctype = 1;
  ast->is_self_closing = 1;
  html_parser_eat(parser, HTML_TOKEN_LT); // <
  html_parser_eat(parser,  HTML_TOKEN_EXCL); // !
  ast->name = strdup(parser->token->value); // doctype
  ast->value_str = strdup(parser->token->value); // doctype
  html_parser_eat(parser, HTML_TOKEN_DOCTYPE);
  collect_options(parser, ast, ast); // ...options
  html_parser_eat(parser, HTML_TOKEN_GT); // >
  return ast;
}

HTMLAST *html_parser_parse_element(HTMLParser *parser, HTMLAST *parent) {
  HTMLAST* doctype = 0;
  if (parser->has_doctype == 0 && html_parser_seek_string(parser, "doctype")) {
    doctype = html_parser_parse_doctype(parser, parent);
    parser->has_doctype = 1;
  }
  HTMLAST* ast = init_html_ast(HTML_AST_ELEMENT);
  ast->doctype = doctype;
  if (parent && parent->children != 0)  {
    html_ast_list_append(parent->children, ast);
  }
  ast->children = init_html_ast_list();

  if (parser->token->type != HTML_TOKEN_LT) {
    return doctype ? doctype : init_html_ast_with_name(HTML_AST_ELEMENT, "doctype");
  }


  html_parser_eat(parser, HTML_TOKEN_LT); // <
  ast->name = strdup(parser->token->value); // span
  ast->value_str = strdup(ast->name);
  html_parser_eat(parser, HTML_TOKEN_ID);
  collect_options(parser, ast, ast); // ...options


    // we're self-closing
  if (parser->token->type == HTML_TOKEN_DIV) {  // />
    html_parser_eat(parser, HTML_TOKEN_DIV);
    html_parser_eat(parser, HTML_TOKEN_GT);
    ast->is_self_closing = 1;
    return ast;
  }

  // we're self-closing
  if (html_is_self_closing(ast->name, parser->options)) {
    html_parser_eat(parser, HTML_TOKEN_GT);
    ast->is_self_closing = 1;
    return ast;
  }



  html_parser_eat(parser, HTML_TOKEN_GT); // >

  while (parser->token->type != HTML_TOKEN_LT && parser->token->type != HTML_TOKEN_EOF) {
    HTMLAST* text_ast = html_parser_parse_raw(parser, ast);
    html_ast_list_append(ast->children, text_ast);
  }

  while (parser->token->type == HTML_TOKEN_LT && parser->lexer->c != '/') {

    HTMLAST* child = html_parser_parse_element(parser, ast);
  }


  while (parser->token->type != HTML_TOKEN_LT  && parser->token->type != HTML_TOKEN_EOF) {
    HTMLAST* text_ast = html_parser_parse_raw(parser, ast);
    html_ast_list_append(ast->children, text_ast);
  }

  if (parser->token->type == HTML_TOKEN_LT) {
    _html_parser_parse_end_tag(parser, parent);
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
    printf("haha\n");
    type = HTML_AST_COMPUTE;
  }
  HTMLAST *ast = init_html_ast(type);
  ast->value_str = strdup(v);
  free(v);

  // ast->sibling = html_parser_parse_factor(parser, parent);
  return ast;
}

#include <string.h>
HTMLAST *html_parser_parse_raw(HTMLParser *parser, HTMLAST *parent) {
//  if (parser->token->type == HTML_TOKEN_LT || parser->lexer->c == 0)
  //  return 0;

  unsigned int allow_compute = 0;//1 && ALLOW_COMPUTE == 1;
  if (parent != 0 && parent->value_str != 0) {
    const char* parent_name = html_get_name(parent);
    allow_compute = strcmp(parent_name, "style") != 0 && strcmp(parent_name, "script") != 0 && strcmp(parent_name, "pre") != 0;
  }

  const char* template = "</%s>";

  char* buff1 = 0;

  if (parent != 0 && parent->value_str && strlen(parent->value_str) < 256) {
    buff1 = (char*)calloc(strlen(template) + 1 + (parent->value_str ? strlen(parent->value_str) : 1) + 1, sizeof(char));
    sprintf(buff1, template, parent->value_str ? parent->value_str : "");
  }

  HTMLToken *tok = html_lexer_parse_string_until(parser->lexer, allow_compute, buff1, '<');

  if (buff1 != 0) free(buff1);
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

EHTMLUnit html_parser_parse_unit(HTMLParser* parser) {
  char* v = parser->token->value;
  html_parser_eat(parser, HTML_TOKEN_JUNK);
  EHTMLUnit unit = HTML_UNIT_PX;

  if (v && strcmp(v, "%") == 0) unit = HTML_UNIT_PERCENT;

  return unit;
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


  if (left && parser->token->type == HTML_TOKEN_JUNK) {
    left->unit = html_parser_parse_unit(parser);
  }

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
  char c = parser->token->c;
  html_parser_eat(parser, HTML_TOKEN_STR);
  HTMLAST *ast = init_html_ast(HTML_AST_STR);
  ast->c = c;
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

    if (parser->token->type == HTML_TOKEN_ID) {
      ast->right = html_parser_parse_id(parser, parent);
    } else {
      ast->right = html_parser_parse_factor(parser, parent);
    }
  }

  return ast;
}
