# html
> C library for parsing HTML

## Usage

``` C
#include <html.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  HTMLNode *root = html("<button value=\"false\"/>");

  int len = 0;
  char **propnames = html_get_propnames(root, &len);

  for (int i = 0; i < len; i++) {
    printf("%s ====> %s\n", propnames[i],
           html_get_propvalue_str(root, propnames[i]));
    free(propnames[i]);
  }

  free(propnames);
  html_free(root);

  return 0;
}
```

## Known issues
* Does not parse HTML comments.
