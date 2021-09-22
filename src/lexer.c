#include <ctype.h>
#include <html_lexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define LEXER_TOK(lexer, t)                                                    \
  {                                                                            \
    EStr *e = e_fromchar(lexer->c);                                            \
    HTMLToken *tok = init_html_token(t, e->value);                             \
    e_free(e);                                                                 \
    html_lexer_advance(lexer);                                                 \
    return tok;                                                                \
  }

#define LEXER_IS_DONE(lexer) (lexer->i >= lexer->src.length || lexer->c == '\0')

HTMLLexer *init_html_lexer(char *src) {
  HTMLLexer *lexer = (HTMLLexer *)calloc(1, sizeof(HTMLLexer));
  lexer->src = ESTR(src);
  lexer->i = 0;
  lexer->c = 0;
  lexer->i = 0;
  lexer->c = lexer->src.value[lexer->i];

  return lexer;
}

void html_lexer_free(HTMLLexer *lexer) {
  //  if (lexer->src.value)
  //  free(lexer->src.value);
  free(lexer);
}

void html_lexer_advance(HTMLLexer *lexer) {
  if (!LEXER_IS_DONE(lexer)) {
    lexer->i += 1;
    lexer->c = lexer->src.value[lexer->i];
  }
}

HTMLToken *html_lexer_get_next_token(HTMLLexer *lexer) {
  while (!LEXER_IS_DONE(lexer)) {
    html_lexer_skip_whitespace(lexer);

    if (isalnum(lexer->c))
      return html_lexer_parse_id(lexer);
    if (isdigit(lexer->c))
      return html_lexer_parse_number(lexer);

    switch (lexer->c) {
    case '<':
      LEXER_TOK(lexer, HTML_TOKEN_LT);
    case '>':
      LEXER_TOK(lexer, HTML_TOKEN_GT);
    case '=':
      LEXER_TOK(lexer, HTML_TOKEN_EQUALS);
    case '/':
      LEXER_TOK(lexer, HTML_TOKEN_DIV);
    case '\'':
    case '"':
      return html_lexer_parse_string(lexer);
    default: {
      printf("Lexer: Unexpected character `%c`", lexer->c);
      exit(1);
    }
    }
  }

  LEXER_TOK(lexer, HTML_TOKEN_EOF);
}

HTMLToken *html_lexer_parse_id(HTMLLexer *lexer) {
  EStr *s = e_init();

  while ((isalnum(lexer->c) || lexer->c == '-' || lexer->c == '_') &&
         !(LEXER_IS_DONE(lexer))) {
    s = e_concat_char(s, lexer->c);
    html_lexer_advance(lexer);
  }

  HTMLToken *tok = init_html_token(HTML_TOKEN_ID, s->value);

  e_free(s);

  return tok;
}

HTMLToken *html_lexer_parse_string(HTMLLexer *lexer) {
  EStr *s = e_init();

  char delim = lexer->c;
  html_lexer_advance(lexer);

  while (lexer->c != delim && !(LEXER_IS_DONE(lexer))) {
    s = e_concat_char(s, lexer->c);
    html_lexer_advance(lexer);
  }

  HTMLToken *tok = init_html_token(HTML_TOKEN_STR, s->value);

  e_free(s);

  html_lexer_advance(lexer);
  return tok;
}

HTMLToken *html_lexer_parse_number(HTMLLexer *lexer) {
  EStr *s = e_init();

  while ((isdigit(lexer->c)) && !(LEXER_IS_DONE(lexer))) {
    s = e_concat_char(s, lexer->c);
    html_lexer_advance(lexer);
  }

  if (lexer->c == '.') {
    while ((isdigit(lexer->c) || lexer->c == '.') && !(LEXER_IS_DONE(lexer))) {
      s = e_concat_char(s, lexer->c);
      html_lexer_advance(lexer);
    }
  }

  HTMLToken *tok = init_html_token(HTML_TOKEN_NUMBER, s->value);

  e_free(s);

  return tok;
}

void html_lexer_skip_whitespace(HTMLLexer *lexer) {
  while ((lexer->c == ' ' || lexer->c == '\r' || lexer->c == '\n' ||
          lexer->c == '\t') &&
         !(LEXER_IS_DONE(lexer))) {
    html_lexer_advance(lexer);
  }
}

char html_lexer_peek(HTMLLexer *lexer, int i) {
  return lexer->src.value[MIN(lexer->src.length, lexer->i + i)];
}
