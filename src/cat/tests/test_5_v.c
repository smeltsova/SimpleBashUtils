#include <stdio.h>

int main() {
  FILE *f = fopen("test_5_cat.txt", "a");
  for (int c = 0; c <= 127; c++) {
    fprintf(f, "%d = '%c'\n", c, c);
  }
  fclose(f);
  return 0;
}
