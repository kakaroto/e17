#ifndef _EVOLVE_PARSE_H
#define _EVOLVE_PARSE_H

void evolve_parse_name(char *name);
void evolve_parse_widget(char *type);
void evolve_parse_parent(char *parent);
int  evolve_parse_property_number(char *prop, char *value);
int  evolve_parse_property_string(char *prop, char *value);
void evolve_parse_packing_property_number(char *prop, char *value);
void evolve_parse_packing_property_string(char *prop, char *value);
void evolve_parse_signal();
void evolve_parse_signal_property(char *prop, char *value);
void evolve_parse_signal_data_property(char *prop, char *value);
void evolve_parse_edje(char *code);
  
#endif
