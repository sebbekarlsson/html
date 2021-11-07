#include <ctype.h>
#include <html_lexer.h>
#include <html_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define LEXER_TOK(lexer, t)                                                    \
  {                                                                            \
    char *e = str_from_char(lexer->c);                                         \
    HTMLToken *tok = init_html_token(t, e);                                    \
    free(e);                                                                   \
    html_lexer_advance(lexer);                                                 \
    return tok;                                                                \
  }

#define LEXER_IS_DONE(lexer)                                                   \
  (lexer->i >= lexer->length || lexer->c == '\0' || lexer->c == 0)

HTMLLexer *init_html_lexer(char *src) {
  HTMLLexer *lexer = (HTMLLexer *)calloc(1, sizeof(HTMLLexer));
  lexer->src = strdup(src);
  lexer->c = 0;
  lexer->i = 0;
  lexer->c = lexer->src[lexer->i];
  lexer->length = strlen(src);

  return lexer;
}

void html_lexer_free(HTMLLexer *lexer) {
  //  if (lexer->src)
  //  free(lexer->src);
  if (lexer->src)
    free(lexer->src);
  free(lexer);
}

void html_lexer_advance(HTMLLexer *lexer) {
  if (!LEXER_IS_DONE(lexer)) {
    lexer->i += 1;
    lexer->c = lexer->src[lexer->i];
  }
}

HTMLToken *html_lexer_get_next_token(HTMLLexer *lexer) {
  while (!LEXER_IS_DONE(lexer)) {
    html_lexer_skip_whitespace(lexer);

    if (isdigit(lexer->c))
      return html_lexer_parse_number(lexer);

    if (isalnum(lexer->c))
      return html_lexer_parse_id(lexer);

    switch (lexer->c) {
    case '.':
      LEXER_TOK(lexer, HTML_TOKEN_DOT);
    case '<':
      LEXER_TOK(lexer, HTML_TOKEN_LT);
    case '>':
      LEXER_TOK(lexer, HTML_TOKEN_GT);
    case '=':
      LEXER_TOK(lexer, HTML_TOKEN_EQUALS);
    case '/':
      LEXER_TOK(lexer, HTML_TOKEN_DIV);
    case '!':
      LEXER_TOK(lexer, HTML_TOKEN_EXCL);
    case '{':
      LEXER_TOK(lexer, HTML_TOKEN_LBRACE);
    case '}':
      LEXER_TOK(lexer, HTML_TOKEN_RBRACE);
    case '\'':
    case '"':
      return html_lexer_parse_string(lexer);
    default: {
      if (!LEXER_IS_DONE(lexer)) {
        printf("Lexer: Unexpected character `%c`", lexer->c);
        exit(1);
      }
    }
    }
  }

  LEXER_TOK(lexer, HTML_TOKEN_EOF);
}

HTMLToken *html_lexer_parse_id(HTMLLexer *lexer) {
  char *s = 0;

  while ((isalnum(lexer->c) || lexer->c == '-' || lexer->c == '_') &&
         !(LEXER_IS_DONE(lexer))) {
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
  }

  HTMLToken *tok = init_html_token(HTML_TOKEN_ID, s);
  free(s);
  return tok;
}

HTMLToken *html_lexer_parse_string(HTMLLexer *lexer) {
  char *s = 0;
  ;

  char delim = lexer->c;
  html_lexer_advance(lexer);

  while (lexer->c != delim && !(LEXER_IS_DONE(lexer))) {
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
  }

  HTMLToken *tok = init_html_token(HTML_TOKEN_STR, s);

  free(s);
  html_lexer_advance(lexer);
  return tok;
}

HTMLToken *html_lexer_parse_string_until(HTMLLexer *lexer, char c, char b, unsigned int allow_compute) {
  char *s = 0;

  char delim = c;
  int type = HTML_TOKEN_STR;

  while (lexer->c != delim && lexer->c != b && !(LEXER_IS_DONE(lexer))) {
    if (lexer->c == '{' && allow_compute) {
      type = HTML_TOKEN_COMPUTE;
    }
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
  }

  HTMLToken *tok = init_html_token(type, s);

  free(s);

  return tok;
}

HTMLToken *html_lexer_parse_number(HTMLLexer *lexer) {
  char *s = 0;

  while ((isdigit(lexer->c)) && !(LEXER_IS_DONE(lexer))) {
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
  }

  if (lexer->c == '.') {
    while ((isdigit(lexer->c) || lexer->c == '.') && !(LEXER_IS_DONE(lexer))) {
      html_str_append_char(&s, lexer->c);
      html_lexer_advance(lexer);
    }
  }

  HTMLToken *tok = init_html_token(HTML_TOKEN_NUMBER, s);

  free(s);

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
  return lexer->src[MIN(lexer->length, lexer->i + i)];
}
