
#ifndef __EWL_CONFIG_H
#define __EWL_CONFIG_H

int ewl_config_init(void);
int ewl_config_set_str(const char *k, const char *v);
int ewl_config_set_int(const char *k, const int v);
int ewl_config_set_float(const char *k, const float v);
char *ewl_config_get_str(const char *k);
int ewl_config_get_int(const char *k, int *v);
int ewl_config_get_float(const char *k, float *v);
Evas_Render_Method ewl_config_get_render_method();

#endif
