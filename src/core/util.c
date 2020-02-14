#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"

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