#include <stdio.h>
#include <lexer.h>
#include <parser.h>


int main(int argc, char* argv[]) {
  HTMLLexer* lexer = init_html_lexer("<div class=\"navbar\"></div>");
  HTMLParser* parser = init_html_parser(lexer);

  HTMLAST* root = html_parser_parse(parser);

  printf("%s\n", root->value_str->value);

  return 0;
}
