#include "node.h"

typedef struct {
  int top;
  unsigned capacity;
  struct Node *array[];
} Stack;

Stack *create_stack(unsigned capacity);
void free_stack(Stack *stack);
int is_full(Stack *stack);
int is_empty(Stack *stack);
void push(Stack **stack, struct Node *node);
struct Node *pop(Stack *stack);
struct Node *peek(Stack *stack);