
#ifndef __EWL_THEME_H__
#define __EWL_THEME_H__

int ewl_theme_init(void);
void ewl_theme_init_widget(Ewl_Widget * w);
void ewl_theme_deinit_widget(Ewl_Widget * w);
char *ewl_theme_path();
char *ewl_theme_font_path();
char *ewl_theme_image_get(Ewl_Widget * w, char *k);
char *ewl_theme_data_get_str(Ewl_Widget * w, char *k);
int ewl_theme_data_get_int(Ewl_Widget * w, char *k);
void ewl_theme_data_set(Ewl_Widget * w, char *k, char *v);
void ewl_theme_data_set_default(char *k, char *v);

#endif /* __EWL_THEME_H__ */
