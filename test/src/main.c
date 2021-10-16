#include <html.h>
#include <test/test.h>


void test_example_org() {
  char* contents = read_file("assets/example_org.html");
  HTMLNode *root = html(contents);
  char* htmlstr = html_to_string(root, 0);

  if (htmlstr) {
    printf("%s\n", htmlstr);
    free(htmlstr);
  }

  html_free(root);
  free(contents);
  assert_reached();
}


int main(int argc, char* argv[]) {
  test_example_org();
}
