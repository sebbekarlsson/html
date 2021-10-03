#include <html_parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <html.h>

HTMLParser *init_html_parser(HTMLLexer *lexer) {
  HTMLParser *parser = (HTMLParser *)calloc(1, sizeof(HTMLParser));
  parser->lexer = lexer;
  parser->token = html_lexer_get_next_token(lexer);

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
           parser->token->value->value, parser->token->type, token_type);
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
  return html_parser_parse_expr(parser);
  // return html_parser_parse_compound(parser);
}

HTMLAST *html_parser_parse_compound(HTMLParser *parser) {
  HTMLAST *ast = init_html_ast(HTML_AST_COMPOUND);
  ast->children = init_html_ast_list();

  while (parser->token->type != HTML_TOKEN_EOF) {
    HTMLAST *child = html_parser_parse_expr(parser);
    html_ast_list_append(ast->children, child);
  }

  return ast;
}

HTMLAST* html_parser_parse_element(HTMLParser* parser) {
  HTMLAST *ast = 0;


  if (parser->token->type == HTML_TOKEN_LT) {
    ast = init_html_ast(HTML_AST_ELEMENT);
    html_parser_eat(parser, HTML_TOKEN_LT);
    if (parser->token->type == HTML_TOKEN_DIV) {
      html_parser_eat(parser, HTML_TOKEN_DIV);
      html_parser_eat(parser, HTML_TOKEN_ID);
      ast->value_str = e_fromstr(parser->token->value->value);
      html_parser_eat(parser, HTML_TOKEN_GT);

      return ast;
    }
    ast->value_str = e_fromstr(parser->token->value->value);
    html_parser_eat(parser, HTML_TOKEN_ID);

    if (parser->token->type == HTML_TOKEN_ID) {
      ast->options = init_html_ast_list();
      while (parser->token->type == HTML_TOKEN_ID) {
        HTMLAST *child = html_parser_parse_assignment(parser);
        html_ast_list_append(ast->options, child);
      }
    }

    if (parser->token->type == HTML_TOKEN_DIV) {
      html_parser_eat(parser, HTML_TOKEN_DIV);
      html_parser_eat(parser, HTML_TOKEN_GT);

      return ast;
    }

    html_parser_eat(parser, HTML_TOKEN_GT);

    if (parser->token->type == HTML_TOKEN_ID) {
      HTMLToken* tok = html_lexer_parse_string_until(parser->lexer, '<');
      EStr* buff = e_fromstr(parser->token->value->value);
      if (parser->token)
        html_token_free(parser->token);
      parser->token = tok;

      if (tok != 0 && tok->value != 0 && tok->value->value != 0) {
        if (buff != 0 && buff->value != 0) {
          e_concat_str(buff, tok->value->value);
          html_set_propvalue_str(ast, "innerText", buff->value);
        }
      html_parser_eat(parser, HTML_TOKEN_STR);
      }

      if (buff !=0) e_free(buff);
  }

    ast->child = html_parser_parse(parser);

    return ast;
  }




  return ast;

}

HTMLAST *html_parser_parse_factor(HTMLParser *parser) {
  HTMLAST *left = 0;


  if (parser->token->type == HTML_TOKEN_LT) {
    HTMLAST* element = html_parser_parse_element(parser);
    left = element;
    while (element && parser->token->type == HTML_TOKEN_LT) {
      element->sibling = html_parser_parse_element(parser);
      element = element->sibling;
    }
  }

  if (left) return left;

  switch (parser->token->type) {
  case HTML_TOKEN_STR:
    left = html_parser_parse_string(parser);
    break;
  case HTML_TOKEN_NUMBER:
    left = html_parser_parse_number(parser);
    break;
  default: {
    printf("Parser.factor: Unexpected token `%s`\n",
           parser->token->value->value);
    exit(1);
  }
  }

  return left;
}

HTMLAST *html_parser_parse_term(HTMLParser *parser) {

  HTMLAST *left = html_parser_parse_factor(parser);

  return left;
}

HTMLAST *html_parser_parse_expr(HTMLParser *parser) {
  HTMLAST *left = html_parser_parse_term(parser);

  return left;
}
HTMLAST *html_parser_parse_number(HTMLParser *parser) {
  float v = atof(parser->token->value->value);
  html_parser_eat(parser, HTML_TOKEN_NUMBER);
  HTMLAST *ast = init_html_ast(HTML_AST_NUMBER);
  ast->value_float = v;
  return ast;
}

HTMLAST *html_parser_parse_string(HTMLParser *parser) {
  char *v = strdup(parser->token->value->value);
  html_parser_eat(parser, HTML_TOKEN_STR);
  HTMLAST *ast = init_html_ast(HTML_AST_STR);
  ast->value_str = e_fromstr(v);
  free(v);
  return ast;
}

HTMLAST *html_parser_parse_id(HTMLParser *parser) {
  char *v = strdup(parser->token->value->value);
  html_parser_eat(parser, HTML_TOKEN_ID);
  HTMLAST *ast = init_html_ast(HTML_AST_ID);
  ast->value_str = e_fromstr(v);
  free(v);
  return ast;
}

HTMLAST *html_parser_parse_assignment(HTMLParser *parser) {
  HTMLAST *ast = init_html_ast(HTML_AST_ASSIGNMENT);
  ast->left = html_parser_parse_id(parser);
  if (parser->token->type == HTML_TOKEN_EQUALS) {
    html_parser_eat(parser, HTML_TOKEN_EQUALS);
    ast->right = html_parser_parse_factor(parser);
  }

  return ast;
}
