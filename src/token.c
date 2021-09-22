#include <token.h>
#include <stdlib.h>


HTMLToken* init_html_token(int type, char* value) {
  HTMLToken* tok = (HTMLToken*)calloc(1, sizeof(HTMLToken));
  tok->type = type;
  tok->value = e_fromstr(value);

  return tok;
}
