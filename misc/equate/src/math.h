#include "structs.h"

#ifndef _EQUATE_MATH
#define _EQUATE_MATH

double equate_eval (void);
double equate_eval_node (equate_node * input);

void equate_print (void);
void equate_print_node (equate_node * input);

void equate_parse_val (double val);
void equate_parse_mult (void);
void equate_parse_div (void);
void equate_parse_add (void);
void equate_parse_sub (void);

void equate_clear (void);
void equate_clear_node (equate_node * node);

void equate_init (void);

#endif
