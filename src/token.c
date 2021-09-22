#include <stdlib.h>
#include <html_token.h>

HTMLToken *init_html_token(int type, char *value) {
  HTMLToken *tok = (HTMLToken *)calloc(1, sizeof(HTMLToken));
  tok->type = type;
  tok->value = e_fromstr(value);

  return tok;
}

void html_token_free(HTMLToken *token) {
  if (token->value)
    e_free(token->value);

  free(token);
}
