#ifndef SRC_CAT_S21_GREP_H_
#define SRC_CAT_S21_GREP_H_

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  bool pattern;
  bool file;
  bool ignore;
  bool invert;
  bool count_matching_lines;
  bool only_matching_files;
  bool number_lines;
  bool no_file_names;
  bool suppress_error;
  bool matched_parts;
} opt;

#define MAX_ERR_LENGTH 80
void grepSearch(FILE* stream, opt options, regex_t* preg, int pat_count,
                char* file_name, bool several_files);
char** add_line(char** lines, char* new_line, int* line_num, int* noerr);
void add_patterns_from_file(char*** pat_uncomp, char* file, int* pat_count,
                            int* noerr);
bool parseOptions(int argc, char* argv[], opt* options, char*** pat_uncomp,
                  int* pat_count);
void readFiles(int argc, char* argv[], opt options, int* pat_count,
               regex_t* preg);
regex_t* compileReg(char** pat_uncomp, opt options, int pat_count,
                    int* compiled, int* err);
void grepExecute(int argc, char* argv[], opt options, char** pat_uncomp,
                 int* pat_count);
void checkEF(char*** pat_uncomp, opt options, int argc, char* argv[],
             int* pat_count, int* err);
bool checkFiles(int argc);
bool searchLines(int pat_count, regex_t* preg, char* line, bool* found_in_file,
                 opt options, int* match_lines, bool several_files,
                 char* file_name, int count_lines);
bool searchParts(int pat_count, regex_t* preg, char* line, opt options,
                 bool several_files, char* file_name, int count_lines);
void memoryFree(char** pat_uncomp, regex_t* preg, int pat_count, int compiled);
bool searchInside(char* line, regoff_t* offset, regex_t* preg, int* cur_pat,
                  int pat_count, regmatch_t* match, regoff_t* next_offset,
                  int* err);
void printMatch(char* line, regoff_t offset, regmatch_t match, opt options,
                bool first_in_line, bool several_files, char* file_name,
                int count_lines);
bool search(char* line, regoff_t offset, regex_t* preg, int* cur_pat,
            int pat_count, regmatch_t* match, regoff_t* next_offset,
            bool inside, int* err);

#endif  // SRC_CAT_S21_GREP_H_