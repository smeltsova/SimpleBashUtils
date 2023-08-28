#include "s21_grep.h"

int main(int argc, char* argv[]) {
  opt options = {0};
  char** pat_uncomp = NULL;
  int pat_count = 0;
  if (parseOptions(argc, argv, &options, &pat_uncomp, &pat_count) &&
      checkFiles(argc)) {
    grepExecute(argc, argv, options, pat_uncomp, &pat_count);
  } else {
    memoryFree(pat_uncomp, NULL, pat_count, 0);
  }
  return 0;
}

char** add_line(char** lines, char* new_line, int* line_num, int* err) {
  char** temp = NULL;

  temp = realloc(lines, (*line_num + 1) * sizeof(char*));
  if (temp) {
    lines = temp;
    lines[*line_num] = strdup(new_line);
    if (lines[*line_num]) {
      *line_num = *line_num + 1;
    } else {
      *err = 1;
    }
  } else {
    *err = 1;
  }
  return lines;
}

void add_patterns_from_file(char*** pat_uncomp, char* file, int* pat_count,
                            int* err) {
  FILE* f = fopen(file, "r");
  if (f) {
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f)) != -1) {
      if (strchr(line, '\n')) *(strchr(line, '\n')) = '\0';
      if (strlen(line) == 0) {
        char* temp = NULL;
        temp = (char*)realloc(line, 3 * sizeof(char));
        if (temp) {
          *line = '.';
          *(line + 1) = '*';
          *(line + 2) = '\0';
        } else {
          *err = 1;
        }
      }
      *pat_uncomp = add_line(*pat_uncomp, line, pat_count, err);
    }
    free(line);
    fclose(f);
  } else {
    fprintf(stderr, "cant open file: %s\n", file);
  }
}

bool parseOptions(int argc, char* argv[], opt* options, char*** pat_uncomp,
                  int* pat_count) {
  int err = 0;
  char arg;
  while ((arg = getopt_long(argc, argv, "e:ivclnhsof:", 0, NULL)) != -1 &&
         !err) {
    switch (arg) {
      case 'e':
        options->pattern = true;
        *pat_uncomp = add_line(*pat_uncomp, optarg, pat_count, &err);
        break;
      case 'i':
        options->ignore = true;
        break;
      case 'v':
        options->invert = true;
        break;
      case 'c':
        options->count_matching_lines = true;
        break;
      case 'l':
        options->only_matching_files = true;
        break;
      case 'n':
        options->number_lines = true;
        break;
      case 'h':
        options->no_file_names = true;
        break;
      case 's':
        options->suppress_error = true;
        break;
      case 'o':
        options->matched_parts = true;
        break;
      case 'f':
        options->file = true;
        add_patterns_from_file(pat_uncomp, optarg, pat_count, &err);
        break;
      default:
        fprintf(stderr, "invalid option: %s", argv[optind - 1]);
        err = 1;
    }
  }
  if (!err) checkEF(pat_uncomp, *options, argc, argv, pat_count, &err);
  return !err;
}

void checkEF(char*** pat_uncomp, opt options, int argc, char* argv[],
             int* pat_count, int* err) {
  if (!options.pattern && !options.file && optind < argc) {
    *pat_uncomp = add_line(*pat_uncomp, argv[optind], pat_count, err);
    optind++;
  }
  if (*err) {
    fprintf(stderr, "memory allocation failed");
  }
}
bool checkFiles(int argc) {
  int err = 0;
  if (optind == argc) {
    fprintf(stderr, "invalid input");
    err = 1;
  }
  return !err;
}

regex_t* compileReg(char** pat_uncomp, opt options, int pat_count,
                    int* compiled, int* err) {
  regex_t* preg = NULL;
  preg = (regex_t*)malloc(pat_count * sizeof(regex_t));
  if (!preg) {
    *err = 1;
    fprintf(stderr, "memory allocation failed");
  } else {
    int regerr;
    char err_msg[MAX_ERR_LENGTH];
    for (int i = 0; i < pat_count && !*err; i++) {
      if (options.ignore) {
        regerr = regcomp(preg + i, pat_uncomp[i], REG_EXTENDED | REG_ICASE);
      } else {
        regerr = regcomp(preg + i, pat_uncomp[i], REG_EXTENDED);
      }
      if (regerr != 0) {
        regerror(regerr, preg + i, err_msg, MAX_ERR_LENGTH);
        fprintf(stderr, "Error analyzing regular expression '%s': %s.\n",
                pat_uncomp[i], err_msg);
        *err = 2;
      } else {
        *compiled = i + 1;
      }
    }
  }
  return preg;
}

void grepExecute(int argc, char* argv[], opt options, char** pat_uncomp,
                 int* pat_count) {
  int err = 0;
  int compiled = 0;
  regex_t* preg = compileReg(pat_uncomp, options, *pat_count, &compiled, &err);
  if (!err) {
    readFiles(argc, argv, options, pat_count, preg);
  }
  memoryFree(pat_uncomp, preg, *pat_count, compiled);
}

void memoryFree(char** pat_uncomp, regex_t* preg, int pat_count, int compiled) {
  for (int i = 0; i < pat_count; i++) {
    if (pat_uncomp[i]) free(pat_uncomp[i]);
  }
  if (pat_uncomp) free(pat_uncomp);
  for (int i = 0; i < compiled; i++) {
    if (preg + i) regfree(preg + i);
  }
  if (preg) free(preg);
}

void readFiles(int argc, char* argv[], opt options, int* pat_count,
               regex_t* preg) {
  bool several_files = (argc - optind > 1);
  while (optind < argc) {
    FILE* f = fopen(argv[optind], "r");
    if (f) {
      grepSearch(f, options, preg, *pat_count, argv[optind], several_files);
      fclose(f);
    } else if (!options.suppress_error) {
      fprintf(stderr, "cant open file: %s\n", argv[optind]);
    }
    optind++;
  }
}

void grepSearch(FILE* stream, opt options, regex_t* preg, int pat_count,
                char* file_name, bool several_files) {
  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  int match_lines = 0;
  int count_lines = 0;
  bool found_in_file = false;
  int err = 0;

  while (err == 0 && ((read = getline(&line, &len, stream)) != -1) &&
         !(options.only_matching_files && found_in_file)) {
    count_lines++;
    if (strchr(line, '\n')) *(strchr(line, '\n')) = '\0';
    if (options.matched_parts && !options.count_matching_lines &&
        !options.only_matching_files && !options.invert) {
      err = searchParts(pat_count, preg, line, options, several_files,
                        file_name, count_lines);
    } else {
      err = searchLines(pat_count, preg, line, &found_in_file, options,
                        &match_lines, several_files, file_name, count_lines);
    }
  }
  free(line);
  if (!err) {
    if (options.count_matching_lines) {
      if (several_files && !options.no_file_names) printf("%s:", file_name);
      printf("%d\n", match_lines);
    }
    if (options.only_matching_files && found_in_file) {
      printf("%s\n", file_name);
    }
  }
}

bool searchParts(int pat_count, regex_t* preg, char* line, opt options,
                 bool several_files, char* file_name, int count_lines) {
  regmatch_t match;
  regoff_t offset = 0, next_offset = 0;
  bool found_in_line = false;
  int cur_pat = 0;
  int err = 0;
  while ((size_t)offset <= strlen(line) && cur_pat < pat_count) {
    if (search(line, offset, preg, &cur_pat, pat_count, &match, &next_offset, 0,
               &err) == true) {
      printMatch(line, offset, match, options, !found_in_line, several_files,
                 file_name, count_lines);
      found_in_line = true;

      next_offset = offset + match.rm_eo;
      offset += match.rm_so;
      int next_pat = cur_pat;
      if (searchInside(line, &offset, preg, &cur_pat, pat_count, &match,
                       &next_offset, &err) == true) {
        cur_pat = 0;
        offset = next_offset;
      } else {
        cur_pat = next_pat;
        offset = next_offset;
      }
    }
  }
  return err;
}

bool searchLines(int pat_count, regex_t* preg, char* line, bool* found_in_file,
                 opt options, int* match_lines, bool several_files,
                 char* file_name, int count_lines) {
  bool found_in_line = false;
  int err = 0;
  for (int i = 0; i < pat_count && !err; i++) {
    int regerr;
    if ((regerr = regexec(preg + i, line, 0, NULL, 0)) == 0) {
      found_in_line = true;
      break;
    } else if (regerr != REG_NOMATCH) {
      char err_msg[MAX_ERR_LENGTH];
      regerror(regerr, preg + i, err_msg, MAX_ERR_LENGTH);
      printf("%s\n", err_msg);
      err = regerr;
    }
  }
  if (!err) {
    if ((options.invert && !found_in_line) ||
        (!options.invert && found_in_line)) {
      (*match_lines)++;
      *found_in_file = true;
      if (!options.count_matching_lines && !options.only_matching_files) {
        if (several_files && !options.no_file_names) {
          printf("%s:", file_name);
        }
        if (options.number_lines) {
          printf("%d:", count_lines);
        }
        printf("%s\n", line);
      }
    }
  }
  return err;
}

bool searchInside(char* line, regoff_t* offset, regex_t* preg, int* cur_pat,
                  int pat_count, regmatch_t* match, regoff_t* next_offset,
                  int* err) {
  bool found = false;
  (*cur_pat)++;
  while (*cur_pat < pat_count) {
    if (search(line, *offset, preg, cur_pat, pat_count, match, next_offset, 1,
               err)) {
      printMatch(line, *offset, *match, (opt){0}, 0, 0, NULL, 0);
      found = true;
      (*cur_pat)++;
      *offset += match->rm_so;
    }
  }
  return found;
}

bool search(char* line, regoff_t offset, regex_t* preg, int* cur_pat,
            int pat_count, regmatch_t* match, regoff_t* next_offset,
            bool inside, int* err) {
  int regerr = 0;
  bool found = false;
  regoff_t prev_match_end = 0;
  if (inside) {
    prev_match_end = offset + match->rm_eo - match->rm_so;
  }
  int cflags = 0;
  if (offset > 0) cflags = REG_NOTBOL;
  while (
      *cur_pat < pat_count &&
      ((regerr = regexec(preg + *cur_pat, line + offset, 1, match, cflags)) ==
           REG_NOMATCH ||
       (regerr == 0 && match->rm_eo - match->rm_so <= 0 && strlen(line) > 0) ||
       (regerr == 0 && inside && offset + match->rm_eo > prev_match_end))) {
    (*cur_pat)++;
  }
  if (*cur_pat != pat_count) {
    if (regerr != 0) {
      char err_msg[MAX_ERR_LENGTH];
      regerror(regerr, preg + *cur_pat, err_msg, MAX_ERR_LENGTH);
      printf("%s\n", err_msg);
      *err = regerr;
    } else {
      found = true;
      *next_offset = offset + match->rm_eo;
      if (match->rm_eo - match->rm_so == 0) {
        (*cur_pat)++;
      }
    }
  }
  return found;
}

void printMatch(char* line, regoff_t offset, regmatch_t match, opt options,
                bool first_in_line, bool several_files, char* file_name,
                int count_lines) {
  regoff_t match_len = match.rm_eo - match.rm_so;
  regoff_t off = offset + match.rm_so;
  if (!options.no_file_names) {
    if (several_files && first_in_line) {
      printf("%s:", file_name);
    }
  }
  if (options.number_lines) {
    if (first_in_line) printf("%d:", count_lines);
  }
  printf("%.*s\n", (int)match_len, line + off);
}
