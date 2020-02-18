#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Stack* createStack(unsigned capacity) {
  int size = sizeof(Stack) + capacity * sizeof(char*);
  Stack* stack = (Stack*)malloc(size);
  stack->capacity = capacity;
  stack->top = -1;
  return stack;
}

int isFull(Stack* stack) { return stack->top == stack->capacity - 1; }

int isEmpty(Stack* stack) { return stack->top == -1; }

void push(Stack** stack, char* str) {
  if (isFull(*stack)) {
    int new_capacity = (*stack)->capacity * 2;

    Stack* temp = createStack(new_capacity);
    temp->top = (*stack)->top;
    temp->capacity = new_capacity;
    int items = (*stack)->top + 1;
    for (int i = 0; i < items; i++) {
      temp->array[i] = (*stack)->array[i];
    }

    free(*stack);
    *stack = temp;
  }

  (*stack)->array[++(*stack)->top] = str;
}

char* pop(Stack* stack) {
  if (isEmpty(stack)) {
    return NULL;
  }
  return stack->array[stack->top--];
}