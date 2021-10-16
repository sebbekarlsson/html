#include <html_token.h>
#include <stdlib.h>
#include <string.h>

HTMLToken *init_html_token(int type, char *value) {
  HTMLToken *tok = (HTMLToken *)calloc(1, sizeof(HTMLToken));
  tok->type = type;

  char *g = value != 0 ? strdup(value) : strdup("");
  tok->value = g;

  return tok;
}

void html_token_free(HTMLToken *token) {
  if (token->value) {
    free(token->value);
    token->value = 0;
  }

  free(token);
}
