#ifndef HTML_PARSER_H
#define HTML_PARSER_H
#include <lexer.h>
#include <token.h>
#include <AST.h>
typedef struct {
  HTMLLexer* lexer;
  HTMLToken* token;
} HTMLParser;

HTMLParser* init_html_parser(HTMLLexer* lexer);

void html_parser_eat(HTMLParser* parser, int token_type);

HTMLAST* html_parser_parse(HTMLParser* parser);

HTMLAST* html_parser_parse_compound(HTMLParser* parser);

HTMLAST* html_parser_parse_factor(HTMLParser* parser);

HTMLAST* html_parser_parse_term(HTMLParser* parser);

HTMLAST* html_parser_parse_expr(HTMLParser* parser);

HTMLAST* html_parser_parse_number(HTMLParser* parser);

HTMLAST* html_parser_parse_string(HTMLParser* parser);

HTMLAST* html_parser_parse_assignment(HTMLParser* parser);

HTMLAST* html_parser_parse_id(HTMLParser* parser);
#endif
