#include "math.h"
#include "structs.h"

#include <stdlib.h>

equate_node *root=NULL;
equate_node *lastop=NULL;

int pres[5] ={
  9,2,2,3,3};

void parse_oper(equate_operator type);

double equate_eval() {
  return equate_eval_node(root);
}

double equate_eval_node(equate_node *node) {
  if (node->type == OP_VAL)
    return node->payload.val;
  else if (node->type == OP_ADD)
    return equate_eval_node(node->payload.left) + equate_eval_node(node->right);
  else if (node->type == OP_SUB)
    return equate_eval_node(node->payload.left) - equate_eval_node(node->right);
  else if (node->type == OP_MUL)
    return equate_eval_node(node->payload.left) * equate_eval_node(node->right);
  else if (node->type == OP_DIV)
    return equate_eval_node(node->payload.left) / equate_eval_node(node->right);
  
  return 0;
}

void equate_parse_val(double val) {
  equate_node *node;
  if (!(node=malloc(sizeof(equate_node))))
    return;

  node->type=OP_VAL;
  node->payload.val=val;
  node->right=NULL;
  
  if (lastop) {
    lastop->right=node;
    lastop=NULL;
  }
  else
    root=node;
  printf("op_val %f\n", val);
}

void equate_parse_mult() {
  parse_oper(OP_MUL);
  printf("op_mul\n");
}

void equate_parse_div() {
  parse_oper(OP_DIV);
  printf("op_div\n");
} 
  
void equate_parse_add() {
  parse_oper(OP_ADD);
  printf("op_add\n");
} 
  
void equate_parse_sub() {
  parse_oper(OP_SUB);
  printf("op_sub\n");
}  

void equate_clear() {
  equate_clear_node(root);
  root=NULL;
  printf("clear\n");
}

void equate_clear_node(equate_node *node) {
  if (!node)
    return;
  if (node->payload.left) 
    equate_clear_node(node->payload.left);
  if (node->right)
    equate_clear_node(node->right);
  free(node);
}

void parse_oper(equate_operator type) {
  equate_node *node;
  equate_node *x;
  equate_node *lastx;
  if (!(node=malloc(sizeof(equate_node))))
    return;

  node->type=type;
  node->right=NULL;

  x=root;
  lastx = NULL;
  while(x) {
    if (pres[x->type] >= pres[type]) {
      if (lastx == NULL) {
        node->payload.left = root;
        root = node;
      } else {
        node->payload.left = lastx->right;
        lastx->right=node;
      }
      break;
    }
    lastx=x;
    x = x->right;
  }

  lastop=node;
}
