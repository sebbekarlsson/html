
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

int main(int argc, char *argv[]) {
  char *contents = read_file(argv[1]);
  HTMLNode *root = html(contents);

  char *rootstr = html_to_string(root, 0);

  html_get_propvalue_str(root, "lang");

  if (rootstr) {
    printf("%s\n", rootstr);
    free(rootstr);
  }
  //  deep(root, 0);

  html_free(root);
  free(contents);
  return 0;
}
