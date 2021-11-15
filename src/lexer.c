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
  lexer->line = 1;
  lexer->column = 0;

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

    if (lexer->c == '\n') {
      lexer->line += 1;
      lexer->column = 0;
    } else {
      lexer->column += 1;
    }

  }
}

HTMLToken *html_lexer_get_next_token(HTMLLexer *lexer) {
  while (!LEXER_IS_DONE(lexer)) {
    html_lexer_skip_whitespace(lexer);

    if (isdigit(lexer->c))
      return html_lexer_parse_number(lexer);

    if (isalnum(lexer->c) || lexer->c == '@' || lexer->c == ':')
      return html_lexer_parse_id(lexer);

    if (lexer->c == '<' && html_lexer_peek(lexer, 1) == '!' && html_lexer_peek(lexer, 2) == '-' && html_lexer_peek(lexer, 3) == '-') {
      return html_lexer_parse_comment(lexer);
    }

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
    case ',':
      LEXER_TOK(lexer, HTML_TOKEN_COMMA);
    case '\'':
    case '"':
      return html_lexer_parse_string(lexer);
    default: {
      if (!LEXER_IS_DONE(lexer)) {
        LEXER_TOK(lexer, HTML_TOKEN_JUNK);
      }
    }
    }
  }

  LEXER_TOK(lexer, HTML_TOKEN_EOF);
}

HTMLToken *html_lexer_parse_id(HTMLLexer *lexer) {
  char *s = 0;

  while ((isalnum(lexer->c) || lexer->c == '-' || lexer->c == '_' || lexer->c == '@' || lexer->c == '.' || lexer->c == ':') &&
         !(LEXER_IS_DONE(lexer))) {
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
  }

  if (lexer->c == ':' && isalnum(html_lexer_peek(lexer, 1))) {
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
    while ((isalnum(lexer->c) || lexer->c == '-' || lexer->c == '_' || lexer->c == '@' || lexer->c == '.' || lexer->c == ':') &&
         !(LEXER_IS_DONE(lexer))) {
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
  }
  }

  int type = HTML_TOKEN_ID;
  if (s && (strcmp(s, "doctype") == 0 || strcmp(s, "DOCTYPE") == 0) ) type = HTML_TOKEN_DOCTYPE;
  HTMLToken *tok = init_html_token(type, s);
  free(s);
  return tok;
}

HTMLToken *html_lexer_parse_string(HTMLLexer *lexer) {
  char *s = 0;

  char delim = lexer->c;
  html_lexer_advance(lexer);

  while (lexer->c != delim && !(LEXER_IS_DONE(lexer))) {
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
  }

  HTMLToken *tok = init_html_token(HTML_TOKEN_STR, s);
  tok->c = delim;

  free(s);
  html_lexer_advance(lexer);
  return tok;
}


#define IS_END_OF_COMMENT() (lexer->c == '-' && html_lexer_peek(lexer, 1) == '-' && html_lexer_peek(lexer, 2) == '>')

HTMLToken *html_lexer_parse_comment(HTMLLexer *lexer) {
  char *s = 0;

  int type = HTML_TOKEN_COMMENT;

  while ((!IS_END_OF_COMMENT()) && !(LEXER_IS_DONE(lexer))) {
    html_str_append_char(&s, lexer->c);
    html_lexer_advance(lexer);
  }

  html_str_append_char(&s, lexer->c);
  html_lexer_advance(lexer);
  html_str_append_char(&s, lexer->c);
  html_lexer_advance(lexer);
  html_str_append_char(&s, lexer->c);
  html_lexer_advance(lexer);


  html_lexer_skip_whitespace(lexer);

  HTMLToken *tok = init_html_token(type, s);

  free(s);

  return tok;
}


static void next_word(HTMLLexer* lexer, uint32_t len, char* dest) {
  char* str = 0;
  uint32_t i = lexer->i;
  char c = lexer->src[i];


  while (i < lexer->i + len && i < lexer->length) {
    c = lexer->src[i];

    while (c == ' ' || c == '\t' || c == '\n') {
      c = lexer->src[i];
      i++;
    }

    html_str_append_char(&str, c);

    i++;
  }


  if (str != 0) {
    sprintf(dest, "%s", str);
    free(str);
  }

}

HTMLToken *html_lexer_parse_string_until(HTMLLexer *lexer, unsigned int allow_compute, char* word, char c) {
  char *s = 0;

  uint32_t wordlen = strlen(word);

  char delim = c;
  int type = HTML_TOKEN_STR;
  char tmp[256];

  while ((word ? strcmp(tmp, word) : 1) != 0 && lexer->c != delim && !(LEXER_IS_DONE(lexer))) {

    if (word) {
      next_word(lexer, wordlen, tmp);

      if (strcmp(word, tmp) == 0) {
        break;
      }
    }
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


void html_lexer_get_lineinfostr(HTMLLexer* lexer, char* dest) {
  sprintf(dest, "(%d:%d)", (int)lexer->line, (int)lexer->column);
}
