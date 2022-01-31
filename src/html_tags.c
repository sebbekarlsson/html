#include <string.h>
#include <html/html_tags.h>

const char* HTML_SELF_CLOSING_TAGS[] = {
"area",
"base",
"br",
"col",
"embed",
"hr",
"img",
"input",
"link",
"meta",
"param",
"source",
"track",
"wbr",
"command",
"keygen",
"menuitem",
"path"
};

const int HTML_SELF_CLOSING_TAGS_LEN = 18;

unsigned int html_is_self_closing(const char *tagname) {
  for (int i = 0; i < HTML_SELF_CLOSING_TAGS_LEN; i++) {
    const char *tag = HTML_SELF_CLOSING_TAGS[i];

    if (strcmp(tag, tagname) == 0)
      return 1;
  }

  return 0;
}
