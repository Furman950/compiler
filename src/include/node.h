struct Node {
  enum { CONSTANT, ADD, MUL, OPEN_PARN, CLOSE_PARN } type;
  union {
    int value;
    struct {
      struct Node* left;
      struct Node* right;
    } op;
  };
};