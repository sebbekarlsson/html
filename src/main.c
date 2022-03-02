
#include <html/html.h>
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

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Please specify input file.\n");
    return 1;
  }

  HTMLOptions options = (HTMLOptions){ .xml_mode = 1 };

  char *contents = read_file(argv[1]);
  HTMLNode *root = html(contents, &options);

  //printf("%s\n", html_to_string(root, 0, 0));

  HTMLNode* collada = html_find_tag_by_name(root, "COLLADA");
  if (!collada) return 0;

  HTMLASTList list = {0};
  html_find_tags_by_name(collada, "float_array", &list);


  for (uint32_t i = 0; i < list.length; i++) {
    HTMLNode* arr_node = list.items[i];

    float count = html_get_propvalue_number(arr_node, "count");
    printf("%12.6f\n", count);


  }

  //  deep(root, 0);

  html_free(root);
  free(contents);
  return 0;
}
