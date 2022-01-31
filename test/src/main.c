#include <html/html.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>


#define ASSERT(expr)                            \
  { printf("testing %s (%s)...\n", #expr, __func__); assert(expr); printf("OK\n"); }

static char *read_file(char *filepath) {
  char *buffer = NULL;
  size_t len;
  FILE *fp = fopen(filepath, "rb");
  getdelim(&buffer, &len, '\0', fp);
  fclose(fp);
  return buffer;
}


void test_text() {
  char* src = read_file("assets/text.html");
  HTMLNode* root = html(src);

  ASSERT(root != 0);
  ASSERT(root->children->length == 2);
  HTMLNode* first = (HTMLNode*)root->children->items[0];

  ASSERT(first != 0);

  ASSERT(first->children->length == 1);

  HTMLNode* textnode = (HTMLNode*)first->children->items[0];

  ASSERT(textnode != 0);

  ASSERT(textnode->type == HTML_AST_STR_ELEMENT);

  char* txt = textnode->value_str;

  ASSERT(txt != 0);

  ASSERT(strcmp(txt, "Hello") == 0);

  html_free(root);
}


void test_example_org() {
  char* src = read_file("assets/text.html");
  HTMLNode* root = html(src);
  ASSERT(root != 0);
  html_free(root);
}

int main(int argc, char* argv[]) {
  test_text();
  test_example_org();
  return 0;
}
