typedef struct {
  int top;
  unsigned capacity;
  char *array[];
} Stack;

Stack *createStack();
int isFull(Stack *stack);
int isEmpty(Stack *stack);
void push(Stack **stack, char *item);
char *pop(Stack *stack);
char *peek(Stack *stack);
