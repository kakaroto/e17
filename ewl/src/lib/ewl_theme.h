/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_THEME_H
#define EWL_THEME_H

/**
 * @addtogroup Ewl_Theme Ewl_Theme: Methods for Accessing and Modifying Theme Data
 * Provides methods for accessing theme data, global theme data or per-widget
 * theme data.
 *
 * @{
 */

int              ewl_theme_init(void);
void             ewl_theme_shutdown(void);

int              ewl_theme_widget_init(Ewl_Widget *w);
void             ewl_theme_widget_shutdown(Ewl_Widget *w);

const char      *ewl_theme_path_get(void);
int              ewl_theme_theme_set(const char *theme);

Ecore_List      *ewl_theme_font_path_get(void);
void             ewl_theme_font_path_add(const char *path);

char            *ewl_theme_image_get(Ewl_Widget *w, const char *k);

void             ewl_theme_data_reset(Ewl_Widget *w);

const char      *ewl_theme_data_str_get(Ewl_Widget *w, const char *k);
void             ewl_theme_data_str_set(Ewl_Widget *w, const char *k,
                                        const char *v);

int              ewl_theme_data_int_get(Ewl_Widget *w, const char *k);
void             ewl_theme_data_int_set(Ewl_Widget *w, const char *k, int v);

/**
 * @internal
 * @def EWL_THEME_KEY_NOMATCH
 * Marker used to set when a theme key has no matching value
 */
#define EWL_THEME_KEY_NOMATCH ((char *)0xdeadbeef)

/**
 * @}
 */

#endif
