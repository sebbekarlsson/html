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
  HTMLOptions options = (HTMLOptions){ .xml_mode = 0 };
  char* src = read_file("assets/text.html");
  HTMLNode* root = html(src, &options);

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

  HTMLNode* last = (HTMLNode*)root->children->items[1];
  ASSERT(last != 0);

  ASSERT(last->children->length == 1);
  HTMLNode* textnode2 = (HTMLNode*)last->children->items[0];

  char* txt2 = textnode2->value_str;

  ASSERT(txt2 != 0);

  ASSERT(strcmp(txt2, "Click") == 0);

  html_free(root);
}


void test_example_org() {
  HTMLOptions options = (HTMLOptions){ .xml_mode = 0 };
  char* src = read_file("assets/text.html");
  HTMLNode* root = html(src, &options);
  ASSERT(root != 0);
  html_free(root);
}

void test_inner_text() {
  HTMLOptions options = (HTMLOptions){ .xml_mode = 0 };
  char* src = read_file("assets/innerText.html");
  HTMLNode* root = html(src, &options);
  ASSERT(root != 0);
  HTMLNode* div = html_find_tag_by_name(root, "div");
  ASSERT(div != 0);

  char* innerText = html_get_propvalue_str(div, "innerText");

  ASSERT(innerText != 0);

  ASSERT(strcmp(innerText, "Hello") == 0);

  html_free(root);
}

void test_props() {
  HTMLOptions options = (HTMLOptions){ .xml_mode = 0 };
  char* src = read_file("assets/props.html");
  HTMLNode* root = html(src, &options);
  ASSERT(root != 0);
  HTMLNode* img = html_find_tag_by_name(root, "img");
  ASSERT(img != 0);

  char* img_src = html_get_propvalue_str(img, "src");
  ASSERT(img_src != 0);

  ASSERT(strcmp(img_src, "myimage.png") == 0);

  html_free(root);
}

int main(int argc, char* argv[]) {
  test_text();
  test_example_org();
  test_inner_text();
  test_props();
  return 0;
}
