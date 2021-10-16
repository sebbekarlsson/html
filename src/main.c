
#include <html.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_file(char *filepath) {
  char *buffer = NULL;
  size_t len;
  FILE *fp = fopen(filepath, "rb");
  getdelim(&buffer, &len, '\0', fp);
  fclose(fp);
  return buffer;
}

void deep(HTMLNode *node, int indent) {
  if (!node)
    return;

  if (node->type == HTML_AST_STR_ELEMENT) {
    //     if (node->sibling) {
    //    deep(node->sibling, indent);
    // }
    return;
  }

  char *name = html_get_value_str(node);
  char *inner = html_get_propvalue_str(node, "innerText");

  for (int i = 0; i < indent; i++)
    printf(" ");

  printf("<%s>\n%s", name, inner ? inner : "");

  if (node->children) {
    for (int i = 0; i < node->children->length; i++)
      deep(node->children->items[i], indent + 1);
  }

  for (int i = 0; i < indent; i++)
    printf(" ");
  printf("</%s>\n", name);
}

int main(int argc, char *argv[]) {
  char *contents = read_file("assets/example_org.html");
  HTMLNode *root = html(contents);

  //  deep(root, 0);

  html_free(root);
  free(contents);
  return 0;
}
