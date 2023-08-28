#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  bool number_nonblank;
  bool number_all;
  bool show_endl;
  bool squeeze;
  bool show_tabs;
  bool show_all;
} opt;

void catPrint(FILE* stream, opt options);
bool parseOptions(int argc, char* argv[], opt* options, int* optind);
void readFiles(int argc, char* argv[], opt options, int optind);
void squeeze(int cur, int prev, bool* cont, bool* is_prev_blank);
void number_line(int prev, int* str_count);
void show_endl(int cur);
void show_tabs(int* cur);
void show_all(int* cur);

#endif  // SRC_CAT_S21_CAT_H_