
#ifndef __EWL_THEME_H__
#define __EWL_THEME_H__

/**
 * @defgroup Ewl_Theme Theme: Methods for Accessing and Modifying Theme Data
 * Provides methods for accessing theme data, global theme data or per-widget
 * theme data.
 *
 * @{
 */

int             ewl_theme_init(void);
int             ewl_theme_init_widget(Ewl_Widget * w);
void            ewl_theme_deinit_widget(Ewl_Widget * w);
char           *ewl_theme_path(void);
Ewd_List       *ewl_theme_font_path_get(void);
void            ewl_theme_font_path_add(char *path);
char           *ewl_theme_image_get(Ewl_Widget * w, char *k);
char           *ewl_theme_data_get_str(Ewl_Widget * w, char *k);
int             ewl_theme_data_get_int(Ewl_Widget * w, char *k);
void            ewl_theme_data_set_str(Ewl_Widget * w, char *k, char *v);
void            ewl_theme_data_set_int(Ewl_Widget * w, char *k, int v);
void            ewl_theme_data_set_default_str(char *k, char *v);
void            ewl_theme_data_set_default_int(char *k, int v);

/**
 * @}
 */

#endif				/* __EWL_THEME_H__ */
