#include <test/test.h>

char* read_file(const char* filepath) {
  char* buffer = NULL;
  size_t len;
  FILE* fp = fopen(filepath, "rb");
  getdelim( &buffer, &len, '\0', fp);
  fclose(fp);
  return buffer;
}

void assert_reached() {
  printf("assert_reached: OK\n");
}
