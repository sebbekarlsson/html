#ifndef XHTML_LEXER_H
#define XHTML_LEXER_H
#include <html_token.h>
#include <stdint.h>
typedef struct {
  char *src;
  uint32_t i;
  char c;
  uint32_t length;
} HTMLLexer;

HTMLLexer *init_html_lexer(char *src);

void html_lexer_free(HTMLLexer *lexer);

void html_lexer_advance(HTMLLexer *lexer);

HTMLToken *html_lexer_get_next_token(HTMLLexer *lexer);

HTMLToken *html_lexer_parse_id(HTMLLexer *lexer);

HTMLToken *html_lexer_parse_string(HTMLLexer *lexer);

HTMLToken *html_lexer_parse_string_until(HTMLLexer *lexer, char c, char b);

HTMLToken *html_lexer_parse_number(HTMLLexer *lexer);

char html_lexer_peek(HTMLLexer *lexer, int i);

void html_lexer_skip_whitespace(HTMLLexer *lexer);
#endif
