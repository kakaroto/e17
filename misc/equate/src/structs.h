#ifndef _EQUATE_MATH
#define _EQUATE_MATH

typedef enum {
  OP_VAL,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV
} equate_operator;

typedef struct _equate_node {
  union {
    double val;
    struct _equate_node *left;
  } payload;
  equate_operator type;
  struct _equate_node *right;
} equate_node;

#endif
