#include "compiler.h"
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include "astbuilder.h"
#include "constants.h"
#include "hashmap.h"
#include "util.h"

static ht_hash_table *map;
static int stack_pointer = 1;

void write_pushing_const_to_stack(char *hex_value, FILE *output) {
  char *movw = (char *)malloc(5 * sizeof(char));
  char *movt = (char *)malloc(5 * sizeof(char));

  char *first_four = &hex_value[2];
  char *last_four = &hex_value[6];

  for (int i = 0; i < 4; i++) {
    *(movw + i) = *last_four++;
    *(movt + i) = *first_four++;
  }

  *(movw + 4) = '\n';
  *(movt + 4) = '\n';

  fprintf(output, "    MOVW R0, 0x%04s", movw);
  fprintf(output, "    MOVT R0, 0x%04s", movt);
  fprintf(output, "    PUSH R0\n\n\n");
  stack_pointer++;

  free(movw), free(movt);

  free(first_four), free(last_four);
}

void write_operator_instruction(int type, FILE *output) {
  fprintf(output, "    POP R2\n");
  fprintf(output, "    POP R1\n");
  stack_pointer--;
  stack_pointer--;

  if (type == ADD) {
    fprintf(output, "    ADD R0, R1, R2\n");
  } else if (type == MUL) {
    fprintf(output, "    MUL R0, R1, R2\n");
  }

  fprintf(output, "    PUSH R0\n\n\n");
  stack_pointer++;
}

void write_variable_instruction(int offset, FILE *output) {
  fprintf(output, "    LDR R10, R11, 0x%04x\n", offset * 4);
  fprintf(output, "    PUSH R10\n\n\n");
  stack_pointer++;
}

void write_instructions(struct Node *node, FILE *output) {
  if (node == NULL) return;

  if (node->type == CONSTANT) {
    char *hex_value = (char *)calloc(8, sizeof(char));
    sprintf(hex_value, "0x%08x", node->value);
    write_pushing_const_to_stack(hex_value, output);
    free(hex_value);
    return;
  }

  if (node->type == VAR) {
    int offset = ht_search(map, node->name);
    write_variable_instruction(offset, output);
    return;
  }

  write_instructions(node->op.left, output);
  write_instructions(node->op.right, output);

  write_operator_instruction(node->type, output);
}

void write_variable_to_stack(FILE *output) {
  fprintf(output, "    MOVW R10, 0x0000\n");
  fprintf(output, "    MOVT R10, 0x0000\n");
  fprintf(output, "    PUSH R10\n\n\n");
  stack_pointer++;
}

void write_value_to_variable(char *variable, FILE *output) {
  int offset = ht_search(map, variable) * 4;

  fprintf(output, "    POP R1\n");
  stack_pointer--;
  fprintf(output, "    STR R1, R11, 0x%04x\n\n\n", offset);
}

int proccess_program(char *line, FILE *output) {
  Stack *main_stack = create_stack(1);
  Stack *operator_stack = create_stack(1);

  if (isalpha(*line)) {
    char *variable = calloc(2, sizeof(char));
    char *tmp = line;
    if (extract_variable(&tmp, &variable) == -1) {
      printf("Failed to parse:: %s\n", line);
      free_stack(main_stack), free_stack(operator_stack);
      return -1;
    }

    int position_on_stack = ht_search(map, variable);

    if (position_on_stack == INT_MIN) {
      ht_insert(map, variable, stack_pointer);
      write_variable_to_stack(output);
    }

    struct Node *root = build_ast(&main_stack, &operator_stack, tmp);
    write_instructions(root, output);

    write_value_to_variable(variable, output);
    free(root);
  } else {
    struct Node *root = build_ast(&main_stack, &operator_stack, line);

    write_instructions(root, output);

    free(root);
  }

  free_stack(main_stack), free_stack(operator_stack);
  return 0;
}

void write_variable_to_top_of_stack(char *variable, FILE *output) {
  int offset = ht_search(map, variable) * 4;

  fprintf(output, "    LDR R1, R11, 0x%04x\n", offset);
  fprintf(output, "    PUSH R1\n\n");
  stack_pointer++;
}

int write_header(FILE *fp) {
  FILE *init_fp = open_file(HEADER_BLOCK_PATH, "r");

  if (init_fp == NULL) {
    printf("Failed to open file at %s", HEADER_BLOCK_PATH);
    return errno;
  }

  copy_from_to(init_fp, fp);
  fclose(init_fp);
  return 0;
}

int write_footer(FILE *fp) {
  FILE *footer = open_file(FOOTER_BLOCK_PATH, "r");

  if (footer == NULL) {
    printf("Failed to open file at %s", FOOTER_BLOCK_PATH);
    return errno;
  }

  copy_from_to(footer, fp);
  fclose(footer);
  return 0;
}

void startCompiler() {
  FILE *output = open_file(OUTPUT, "w");
  FILE *myprog = open_file(CODE_FILE_PATH, "r");
  char *buffer = NULL;

  if (output == NULL || myprog == NULL) {
    fclose(output), fclose(myprog), exit(EXIT_FAILURE);
  }

  buffer = read_file_into_buffer(myprog);
  fclose(myprog);

  if (buffer == NULL) fclose(output), exit(EXIT_FAILURE);

  if (write_header(output) != 0) {
    free(buffer), fclose(output);
  }

  char *currentSpot = buffer;
  map = ht_new();
  do {
    char *line = calloc(2, sizeof(char));
    read_line(&currentSpot, &line);
    int length = strlen(line) + 1;

    while (*currentSpot == '\n') currentSpot++;

    if (just_variable(line)) {
      char *variable = calloc(2, sizeof(char));
      extract_variable(&line, &variable);
      write_variable_to_top_of_stack(variable, output);
      free(variable);
    } else if (proccess_program(line, output) == -1) {
      free(line), free(buffer), fclose(output);
      return;
    }
    free(line);
    line = NULL;
  } while (*currentSpot != '\0');

  ht_del_hash_table(map);

  write_footer(output);

  free(buffer), fclose(output);

  run_assembler();
}