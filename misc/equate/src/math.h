#include "structs.h"

double equate_eval();
double equate_eval_node(equate_node *input);

void equate_parse_val();
void equate_parse_mult();
void equate_parse_div();
void equate_parse_add();
void equate_parse_sub();

void equate_clear();
void equate_clear_node(equate_node *node);
