
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <html.h>


static char* read_file(const char* path) {
  FILE* file = fopen(path, "r");

  char* buff = 0;
  uint32_t bufflen = 0;

  char* tmp = 0;

  unsigned int bytes_read = 0;
  size_t len = 0;

  while ((bytes_read = getline(&tmp, &len, file)) != -1) {
    bufflen += len;
    buff = realloc(buff, bufflen * sizeof(char));
    strcat(buff, tmp);
  }

  return buff;
}

void deep(HTMLNode* node, int indent) {
  if (!node) return;

  if (node->type == HTML_AST_STR_ELEMENT) {
 //     if (node->sibling) {
//    deep(node->sibling, indent);
 // }
    return;
  }

  char* name = html_get_value_str(node);
  char* inner = html_get_propvalue_str(node, "innerText");

  for (int i = 0; i < indent; i++)
    printf(" ");

  printf("<%s>\n%s", name, inner ? inner : "");

  if (node->children) {
    for (int i = 0; i < node->children->length; i++)
      deep(node->children->items[i], indent+1);
  }

  for (int i = 0; i < indent; i++)
    printf(" ");
  printf("</%s>\n", name);

}

int main(int argc, char *argv[]) {
  char* contents = read_file("assets/index.html");
  HTMLNode *root = html(contents);

  deep(root, 0);

  html_free(root);

  free(contents);
  return 0;
}
