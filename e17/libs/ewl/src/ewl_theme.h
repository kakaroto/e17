
#ifndef __EWL_THEME_H__
#define __EWL_THEME_H__

void			ewl_theme_init(void);
char		  * ewl_theme_path();
char		  * ewl_theme_font_path();
char		  * ewl_theme_ebit_get(char * widget, char * type, char * state);
int				ewl_theme_int_get(char * key);
void			ewl_theme_int_set(char * key, int value);
float			ewl_theme_float_get(char * key);
void			ewl_theme_float_set(char * key, float value);
char		  * ewl_theme_str_get(char * key);
void			ewl_theme_str_set(char * key, char * value);

#endif
