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


const char* html_token_to_string(int type) {
  switch(type) {

   case HTML_TOKEN_ID: return "HTML_TOKEN_ID";
   case HTML_TOKEN_DOCTYPE: return "HTML_TOKEN_DOCTYPE";
   case HTML_TOKEN_COMMENT: return "HTML_TOKEN_COMMENT";
   case HTML_TOKEN_LT: return "HTML_TOKEN_LT";
   case HTML_TOKEN_GT: return "HTML_TOKEN_GT";
   case HTML_TOKEN_STR: return "HTML_TOKEN_STR";
   case HTML_TOKEN_EQUALS: return "HTML_TOKEN_EQUALS";
   case HTML_TOKEN_NUMBER: return "HTML_TOKEN_NUMBER";
   case HTML_TOKEN_DIV: return "HTML_TOKEN_DIV";
   case HTML_TOKEN_EOF: return "HTML_TOKEN_EOF";
   case HTML_TOKEN_QUESTION: return "HTML_TOKEN_QUESTION";
   case HTML_TOKEN_COMMA: return "HTML_TOKEN_COMMA";
   case HTML_TOKEN_LBRACE: return "HTML_TOKEN_LBRACE";
   case HTML_TOKEN_RBRACE: return "HTML_TOKEN_RBRACE";
   case HTML_TOKEN_EXCL: return "HTML_TOKEN_EXCL";
   case HTML_TOKEN_COMPUTE: return "HTML_TOKEN_COMPUTE";
   case HTML_TOKEN_DOT: return "HTML_TOKEN_DOT";
   case HTML_TOKEN_JUNK: return "HTML_TOKEN_JUNK";
  }
}
