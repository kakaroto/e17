#include "math.h"
#include "structs.h"

#include <stdlib.h>
#include <stdio.h>

equate_root *root=NULL;

int pres[6] ={
  9,2,2,3,3,4};

void parse_oper(equate_operator type);

double equate_eval(void) {
  double val;
  val = equate_eval_node(root->root);
  equate_clear();
  return val;
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
    return equate_eval_node(node->payload.left) / equate_eval_node(node->right);  else if (node->type == OP_BRK)
    return equate_eval_node(node->right);
  
  return 0;
}

void equate_print(void) {
  equate_print_node(root->root);
}

void equate_print_node(equate_node *node) {
  if (node->type == OP_VAL)
    printf("%le\n", node->payload.val);
  else if (node->type == OP_ADD) {
    equate_print_node(node->payload.left);
    printf("+\n");
    equate_print_node(node->right);
  } else if (node->type == OP_SUB) {
    equate_print_node(node->payload.left);
    printf("-\n");
    equate_eval_node(node->right);
  } else if (node->type == OP_MUL) {
    equate_print_node(node->payload.left);
    printf("*\n");
    equate_print_node(node->right);
  } else if (node->type == OP_DIV) {
    equate_print_node(node->payload.left);
    printf("/\n");
    equate_print_node(node->right);
  } else if (node->type == OP_BRK) {
    printf("(\n");
    equate_print_node(node->right);
    printf(")\n");
  }
}
                                        

void equate_parse_val(double val) {
  equate_node *node;
  if (!(node=malloc(sizeof(equate_node))))
    return;

  node->type=OP_VAL;
  node->payload.val=val;
  node->right=NULL;
  
  if (root->lastop) {
    root->lastop->right=node;
    root->lastop=NULL;
  }
  else
    root->root=node;
  printf("op_val %f\n", val);
}

void equate_parse_open_brak() {
  equate_root *new;
  
  parse_oper(OP_BRK);
  if (!(new=malloc(sizeof(equate_root))))
    return;
  new->prev=root;
  new->next=NULL;
  new->lastop=NULL;
  new->root=NULL;
  root->next=new;
  root=new;
  printf("op_obrk\n");
}

void equate_parse_close_brak() {
  equate_root *old;
  old=root;
  root=root->prev;
  
  root->lastop->right=old->root;
  root->lastop=NULL;
  root->next=NULL;
  free(old);
  printf("op_cbrk\n");
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

void equate_clear_root(equate_root *node) {
  if (!node) return;
  if (node->next)
    equate_clear_root(node->next);
  
  if (node->prev) /* don't free last root! */
    free(node);
  else {
    node->next=NULL;
    node->root=NULL;
    node->lastop=NULL;
  }
}

void equate_clear() {
  equate_clear_node(root->root);
  
  root->root=NULL;
  printf("clear\n");
}

void equate_clear_node(equate_node *node) {
  if (!node)
    return;
  if (node->payload.left)
    if ((node->type == OP_VAL) && (node->type == OP_BRK))
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

  if (root->lastop) {
    root->lastop->right=node;
  } else if (root->root == NULL) {
    root->root=node;
  } else {
            
    x=root->root;
    lastx = NULL;
    while(x) {
      if (pres[x->type] >= pres[type]) {
        if (lastx == NULL) {
          node->payload.left = root->root;
          root->root = node;
        } else {
          node->payload.left = lastx->right;
          lastx->right=node;
        }
        break;
      }
      lastx=x;
      x = x->right;
    }
  }
  root->lastop=node;
}

void equate_init(void) {
  if (!(root=malloc(sizeof(equate_root))))
    return;
  root->prev=NULL;
  root->next=NULL;
  root->root=NULL;
  root->lastop=NULL;
}
