struct Node {
  enum { VAR, CONSTANT, ADD, MUL, OPEN_PARN, CLOSE_PARN } type;
  union {
    int value;
    char* name;
    struct {
      struct Node* left;
      struct Node* right;
    } op;
  };
};