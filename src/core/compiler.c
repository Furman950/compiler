#include "compiler.h"
#include "astbuilder.h"
#include "constants.h"
#include "util.h"

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

  free(movw), free(movt);

  free(first_four), free(last_four);
}

void write_operator_instruction(int type, FILE *output) {
  fprintf(output, "    POP R2\n");
  fprintf(output, "    POP R1\n");

  if (type == ADD) {
    fprintf(output, "    ADD R0, R1, R2\n");
  } else if (type == MUL) {
    fprintf(output, "    MUL R0, R1, R2\n");
  }

  fprintf(output, "    PUSH R0\n\n\n");
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

  write_instructions(node->op.left, output);
  write_instructions(node->op.right, output);

  write_operator_instruction(node->type, output);
}

void proccess_program(char *file_buffer, FILE *output) {
  Stack *main_stack = create_stack(1);
  Stack *operator_stack = create_stack(1);
  struct Node *root = build_ast(&main_stack, &operator_stack, file_buffer);

  write_instructions(root, output);

  free(root);
  free_stack(main_stack);
  free_stack(operator_stack);
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
  proccess_program(buffer, output);

  write_footer(output);

  free(buffer), fclose(output);

  run_assembler();
}