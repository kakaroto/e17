#ifndef _EQUATE_STRUCTS
#define _EQUATE_STRUCTS

typedef enum {
  OP_VAL,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_BRK
} equate_operator;

typedef struct _equate_node {
  union {
    double val;
    struct _equate_node *left;
  } payload;
  equate_operator type;
  struct _equate_node *right;
} equate_node;

typedef struct _equate_root {
  struct _equate_node *root;
  struct _equate_node *lastop;
  struct _equate_root *prev;
  struct _equate_root *next;
} equate_root;

#endif
