struct Node {
  enum { CONSTANT, ADD, MUL } type;
  union {
    int value;
    struct {
      struct Node* left;
      struct Node* right;
    } op;
  };
};