
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

  printf("%s\n", html_to_string(root, 0, 1));

  //  deep(root, 0);

  html_free(root);
  free(contents);
  return 0;
}
