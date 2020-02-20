#include "stack.h"
#include <stdlib.h>

Stack* create_stack(unsigned capacity) {
  int size = sizeof(Stack) + capacity * sizeof(struct Node*);
  Stack* stack = (Stack*)malloc(size);
  stack->capacity = capacity;
  stack->top = -1;

  return stack;
}

void free_stack(Stack* stack) {
  int count = stack->top + 1;

  for (int i = 0; i < count; i++) {
    free(stack->array[i]);
  }

  free(stack);
}

int is_full(Stack* stack) { return stack->top == stack->capacity - 1; }

int is_empty(Stack* stack) { return stack->top == -1; }

void push(Stack** stack, struct Node* node) {
  if (is_full(*stack)) {
    int capacity = (*stack)->capacity * 2;
    Stack* temp = create_stack(capacity);
    temp->top = (*stack)->top;
    temp->capacity = capacity;
    int node_count = (*stack)->top + 1;
    for (int i = 0; i < node_count; i++) {
      temp->array[i] = (*stack)->array[i];
    }

    free(*stack);
    *stack = temp;
  }

  (*stack)->array[++(*stack)->top] = node;
}

struct Node* pop(Stack* stack) {
  if (is_empty(stack)) {
    return NULL;
  }
  return stack->array[stack->top--];
}