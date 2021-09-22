#ifndef HTML_TOKEN_H
#define HTML_TOKEN_H
#include <easystr.h>
typedef struct {
  enum {
  TOKEN_ID,
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_STR,
  TOKEN_EQUALS,
  TOKEN_NUMBER,
  TOKEN_DIV,
  TOKEN_EOF
  } type;
  EStr* value;
} HTMLToken;


HTMLToken* init_html_token(int type, char* value);
#endif
