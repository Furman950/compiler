
#include "astbuilder.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int is_operator(char c) { return c == '+' || c == '*'; }

int is_num(char c) { return isdigit(c); }

void build_str(int (*check)(char), char** ptr, char** output) {
  int currentSize = 2;
  int currentIndex = 0;
  char* tmp = *ptr;
  *(*output + currentIndex++) = *tmp++;

  while (check(*tmp)) {
    *output = (char*)realloc(*output, ++currentSize * sizeof(char));
    *(*output + currentIndex++) = *tmp++;
  }

  *(*output + currentIndex) = '\0';
  *ptr = tmp;
}

struct Node* build_ast_from_stacks(Stack* output, Stack* operator_stack) {
  while (!is_empty(operator_stack)) {
    struct Node* node = pop(operator_stack);

    struct Node* right = pop(output);
    struct Node* left = pop(output);

    if (right == NULL || left == NULL) {
      puts("Invalid expression");
      return NULL;
    }

    node->op.right = right;
    node->op.left = left;

    push(&output, node);
  }

  struct Node* root = pop(output);
  return root;
}

void build_ast_while_precedence_is_greater(Stack* output, Stack* operator_stack,
                                           int precedence,
                                           struct Node* top_of_stack) {
  if (top_of_stack == NULL || top_of_stack->type <= precedence ||
      top_of_stack->type == OPEN_PARN || top_of_stack->type == CLOSE_PARN) {
    return;
  }

  struct Node* node = pop(operator_stack);
  struct Node* right = pop(output);
  struct Node* left = pop(output);

  if (right == NULL || left == NULL) {
    puts("Invalid expression");
    return;
  }

  node->op.right = right;
  node->op.left = left;

  push(&output, node);

  build_ast_while_precedence_is_greater(output, operator_stack, precedence,
                                        peek(operator_stack));
}

void build_ast_until_open_parn(Stack* output, Stack* operator_stack) {
  struct Node* node = pop(operator_stack);

  if (node->type == OPEN_PARN) {
    return;
  }

  struct Node* right = pop(output);
  struct Node* left = pop(output);

  if (right == NULL || left == NULL) {
    puts("Invalid expression");
    return;
  }

  node->op.right = right;
  node->op.left = left;

  push(&output, node);

  build_ast_until_open_parn(output, operator_stack);
}

void add_input_to_stacks(Stack** output, Stack** operator_stack, char* input) {
  char* ptr = input;

  while (*ptr != '\0') {
    char* str = calloc(2, sizeof(char));

    if (*ptr == '(') {
      struct Node* node = (struct Node*)malloc(sizeof(struct Node*));
      node->type = OPEN_PARN;
      node->op.left = NULL, node->op.right = NULL;
      push(operator_stack, node);
      ptr++;
    } else if (*ptr == ')') {
      struct Node* top_of_stack = (struct Node*)malloc(sizeof(struct Node*));
      top_of_stack = peek(*operator_stack);

      if (top_of_stack->type == OPEN_PARN) {
        pop(*operator_stack);
        ptr++;
      } else {
        build_ast_until_open_parn(*output, *operator_stack);
        ptr++;
      }

    } else if (is_num(*ptr)) {
      build_str(is_num, &ptr, &str);
      int num = atoi(str);
      struct Node* node = (struct Node*)malloc(sizeof(struct Node*));
      node->type = CONSTANT;
      node->value = num;
      push(output, node);
    } else if (is_operator(*ptr)) {
      int precedence = operator_to_enum(*ptr);
      struct Node* top_of_stack = peek(*operator_stack);

      build_ast_while_precedence_is_greater(*output, *operator_stack,
                                            precedence, top_of_stack);

      struct Node* node = (struct Node*)malloc(sizeof(struct Node*));
      node->type = operator_to_enum(*ptr);
      node->op.left = NULL, node->op.right = NULL;
      push(operator_stack, node);
      ptr++;
    } else if (*ptr == ' ') {
      ptr++;
    } else {
      free(str);
      printf("Failed to parse '%c'", *ptr);
      return;
    }
  }
}

struct Node* build_ast(Stack** main_stack, Stack** operator_stack,
                       char* input) {
  char* ptr = input;
  add_input_to_stacks(main_stack, operator_stack, input);

  struct Node* root = build_ast_from_stacks(*main_stack, *operator_stack);
  return root;
}