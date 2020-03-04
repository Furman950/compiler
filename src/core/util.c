#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "node.h"

void copy_from_to(FILE *from, FILE *to) {
  char c;
  while ((c = fgetc(from)) != EOF) {
    fputc(c, to);
  }
  fprintf(to, "\n\n");
}

void run_assembler() {
  int res = system("java -jar " PATH_TO_JAR " " OUTPUT);
  printf("system() code: %d\n", res);
  if (res != 0) {
    perror("Error trying to run jar:");
  }
}

FILE *open_file(const char *path, const char *args) {
  FILE *fp = fopen(path, args);

  if (fp == NULL) perror("Error opening file: ");

  return fp;
}

char *read_file_into_buffer(FILE *fp) {
  char *buffer;
  long nread;

  fseek(fp, 0L, SEEK_END);
  nread = ftell(fp);
  rewind(fp);

  if (nread == 0) {
    printf("File is empty!!\n");
    return buffer;
  }

  buffer = (char *)calloc(nread + 1, sizeof(char));

  if (!buffer) {
    perror("Error calloc: ");
    return buffer;
  }

  fread(buffer, sizeof(char), nread, fp);

  return buffer;
}

void read_line(char **buffer, char **line) {
  int currentSize = 2, currentIndex = 0;
  char *tmp = *buffer;
  *(*line + currentIndex++) = *tmp++;
  while (*tmp != '\n' && *tmp != '\0') {
    *line = (char *)realloc(*line, ++currentSize * sizeof(char));
    if (*tmp == '\r') {
      tmp++;
      continue;
    };
    *(*line + currentIndex++) = *tmp++;
  }

  *(*line + currentIndex) = '\0';
  *buffer = tmp;
}

int extract_variable(char **line, char **variable) {
  int currentSize = 2, currentIndex = 0;
  char *tmp = *line;
  *(*variable + currentIndex++) = *tmp++;

  // while (isalpha(*tmp)) {
  while (isalpha(*tmp) || isdigit(*tmp)) {
    *variable = (char *)realloc(*variable, ++currentSize * sizeof(char));
    *(*variable + currentIndex++) = *tmp++;
  }

  *(*variable + currentIndex) = '\0';

  while (*tmp == ' ') tmp++;
  if (*tmp == '=')
    tmp++;
  else
    return -1;
  while (*tmp == ' ') tmp++;
  *line = tmp;
  return 0;
}

int just_variable(char *line) {
  if (isalpha(*line++)) {
    while (isalpha(*line) || isdigit(*line)) {
      line++;
    }

    while (*line == ' ') line++;
    if (*line == '=') return 0;
    else return 1;
  }

  return 0;
}

char *get_operator(int op_value) {
  switch (op_value) {
    case ADD:
      return "ADD";
    case MUL:
      return "MUL";
    default:
      return "Invalid operator!!";
  }
}

int operator_to_enum(char c) {
  switch (c) {
    case '+':
      return ADD;
    case '*':
      return MUL;
    default:
      return -1;
  }
}