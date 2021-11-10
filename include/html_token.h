#ifndef XHTML_HTML_TOKEN_H
#define XHTML_HTML_TOKEN_H
typedef struct {
  enum {
  HTML_TOKEN_ID,
  HTML_TOKEN_DOCTYPE,
  HTML_TOKEN_COMMENT,
    HTML_TOKEN_LT,
    HTML_TOKEN_GT,
    HTML_TOKEN_STR,
    HTML_TOKEN_EQUALS,
    HTML_TOKEN_NUMBER,
    HTML_TOKEN_DIV,
    HTML_TOKEN_EOF,
    HTML_TOKEN_QUESTION,
    HTML_TOKEN_COMMA,
    HTML_TOKEN_LBRACE,
    HTML_TOKEN_RBRACE,
    HTML_TOKEN_EXCL,
    HTML_TOKEN_COMPUTE,
    HTML_TOKEN_DOT,
    HTML_TOKEN_JUNK
  } type;
  char *value;
  char c;
} HTMLToken;

const char* html_token_to_string(int type);

HTMLToken *init_html_token(int type, char *value);

void html_token_free(HTMLToken *token);
#endif
