
#ifndef __EWL_CONFIG_H
#define __EWL_CONFIG_H

int ewl_config_init(void);
int ewl_config_set_str(char *k, char *v);
int ewl_config_set_int(char *k, int v);
int ewl_config_set_float(char *k, float v);
char *ewl_config_get_str(char *k);
int ewl_config_get_int(char *k, int *v);
int ewl_config_get_float(char *k, float *v);
Evas_Render_Method ewl_config_get_render_method();

#endif
