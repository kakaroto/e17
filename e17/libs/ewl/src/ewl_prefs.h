
#ifndef __EWL_PREFS_H
#define __EWL_PREFS_H

void ewl_prefs_init(void);
int ewl_prefs_int_get(char *key);
int ewl_prefs_int_set(char *key, int value);
char *ewl_prefs_str_get(char *key);
int ewl_prefs_str_set(char *key, char *value);
int ewl_prefs_float_get(char *key, float *value);
int ewl_prefs_float_set(char *key, float value);
char *ewl_prefs_theme_name_get();
Evas_Render_Method ewl_prefs_render_method_get();
double ewl_prefs_get_fx_max_fps();
double ewl_prefs_get_fx_timeout();

#endif
