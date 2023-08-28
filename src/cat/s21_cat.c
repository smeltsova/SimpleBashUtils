#include "s21_cat.h"

int main(int argc, char* argv[]) {
  opt options = {0};
  int optind;
  if (parseOptions(argc, argv, &options, &optind)) {
    readFiles(argc, argv, options, optind);
  }
  return 0;
}

bool parseOptions(int argc, char* argv[], opt* options, int* optind) {
  int i = 1;
  int err = false;
  while (i < argc && argv[i][0] == '-') {
    if (strlen(argv[i]) > 1) {
      if (argv[i][1] == '-') {
        if (strcmp(argv[i], "--number-nonblank") == 0) {
          options->number_nonblank = true;
        } else if (strcmp(argv[i], "--number") == 0) {
          options->number_all = true;
        } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
          options->squeeze = true;
        } else {
          fprintf(stderr, "invalid option: %s", argv[i]);
          err = true;
        }
      } else {
        for (long unsigned int j = 1; j < strlen(argv[i]); j++) {
          switch (argv[i][j]) {
            case 'b':
              options->number_nonblank = true;
              break;
            case 'e':
              options->show_endl = true;
              options->show_all = true;
              break;
            case 'E':
              options->show_endl = true;
              break;
            case 'n':
              options->number_all = true;
              break;
            case 's':
              options->squeeze = true;
              break;
            case 't':
              options->show_tabs = true;
              options->show_all = true;
              break;
            case 'T':
              options->show_tabs = true;
              break;
            case 'v':
              options->show_all = true;
              break;
            default:
              fprintf(stderr, "invalid option: %s", argv[i]);
              err = true;
          }
        }
      }
    } else {
      fprintf(stderr, "invalid option: %s", argv[i]);
      err = true;
    }
    i++;
  }
  *optind = i;
  return !err;
}

void readFiles(int argc, char* argv[], opt options, int optind) {
  if (optind == argc) {
    catPrint(stdin, options);
  } else {
    for (int i = optind; i < argc; i++) {
      FILE* f = fopen(argv[i], "r");
      if (f) {
        catPrint(f, options);
        fclose(f);
      } else {
        fprintf(stderr, "cant open file: %s\n", argv[i]);
      }
    }
  }
}

void catPrint(FILE* stream, opt options) {
  int prev = '\n';
  int cur;
  int str_count = 0;
  bool is_prev_blank = false;
  bool cont = false;
  while ((cur = getc(stream)) != EOF) {
    if (options.squeeze) {
      squeeze(cur, prev, &cont, &is_prev_blank);
      if (cont) continue;
    }
    if ((options.number_all && !options.number_nonblank) ||
        (options.number_nonblank && cur != '\n')) {
      number_line(prev, &str_count);
    }
    if (options.show_endl) {
      show_endl(cur);
    }
    if (options.show_tabs) {
      show_tabs(&cur);
    }
    if (options.show_all) {
      show_all(&cur);
    }
    printf("%c", cur);
    prev = cur;
  }
}

void squeeze(int cur, int prev, bool* cont, bool* is_prev_blank) {
  *cont = false;
  if (prev == '\n' && cur == '\n') {
    if (*is_prev_blank) {
      *cont = true;
    }
    *is_prev_blank = true;
  } else {
    *is_prev_blank = false;
  }
}

void number_line(int prev, int* str_count) {
  if (prev == '\n') {
    *str_count = *str_count + 1;
    printf("%6d\t", *str_count);
  }
}

void show_endl(int cur) {
  if (cur == '\n') printf("$");
}

void show_tabs(int* cur) {
  if (*cur == '\t') {
    printf("^");
    *cur = 'I';
  }
}

void show_all(int* cur) {
  if ((0 <= *cur && *cur <= 8) || (11 <= *cur && *cur <= 31) || *cur == 127) {
    printf("^");
    *cur = (*cur + 64) % 128;
  }
  if (128 <= *cur && *cur <= 159) {
    printf("M-^");
    *cur = (*cur & 31) + 64;
  }
}