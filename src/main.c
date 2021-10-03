#include <html.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  HTMLNode *root = html("<button> bup I am cool<button/><input>Heja</input>");

  int len = 0;
  char **propnames = html_get_propnames(root, &len);

  HTMLASTList* li = html_get_siblings(root);

  if (li)
  for (int i = 0; i < li->length; i++) {
    printf("%d\n", i);

  }

  for (int i = 0; i < len; i++) {
    printf("%s ====> %s\n", propnames[i],
           html_get_propvalue_str(root, propnames[i]));
    free(propnames[i]);
  }

  free(propnames);
  html_free(root);

  if (li != 0) {
    if (li->items != 0)
      free(li->items);

    free(li);
  }

  return 0;
}
