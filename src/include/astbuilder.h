#include "stack.h"

int is_operator(char c);
int is_num(char c);
void build_str(int (*check)(char), char** ptr, char** output);
struct Node* build_ast_from_stacks(Stack* output, Stack* operator_stack);
void add_input_to_stacks(Stack** output, Stack** operator_stack, char* input);
struct Node* build_ast(Stack** main_stack, Stack** operator_stack, char* input);