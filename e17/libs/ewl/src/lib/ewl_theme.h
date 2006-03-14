#ifndef EWL_THEME_H
#define EWL_THEME_H

/**
 * @page theming How Themes Work
 *
 * EWL relies on a graphical layout engine called Edje. Edje allows a themer
 * to layout images, in fairly advanced ways and provides methods for
 * animation and changing of images based on various signals. EWL uses an Edje
 * collection per-widget and has a set of keys for specifying which widget
 * uses a specific collection. Information about using Edje can be found at the
 * <a href="http://www.enlightenment.org/pages/edje.html">project page</a>.
 * The remainder of this section concentrates specifically on how EWL
 * interacts with Edje.
 *
 * The keys to lookup the group name for the widgets are top level data keys
 * in the themes edje file. Each widget looks up it's group name based on the
 * heirarchy it is placed inside other containers.
 */

/**
 * @addtogroup Ewl_Theme Ewl_Theme: Methods for Accessing and Modifying Theme Data
 * Provides methods for accessing theme data, global theme data or per-widget
 * theme data.
 *
 * @{
 */

int             ewl_theme_init(void);
void            ewl_theme_shutdown(void);
int             ewl_theme_widget_init(Ewl_Widget *w);
void            ewl_theme_widget_shutdown(Ewl_Widget *w);
void            ewl_theme_name_set(char *name);
char           *ewl_theme_name_get(void);
char           *ewl_theme_path_get(void);
Ecore_List     *ewl_theme_font_path_get(void);
void            ewl_theme_font_path_add(char *path);
char           *ewl_theme_image_get(Ewl_Widget *w, char *k);
char           *ewl_theme_data_str_get(Ewl_Widget *w, char *k);
int             ewl_theme_data_int_get(Ewl_Widget *w, char *k);
void            ewl_theme_data_str_set(Ewl_Widget *w, char *k, char *v);
void            ewl_theme_data_int_set(Ewl_Widget *w, char *k, int v);
void            ewl_theme_data_default_str_set(char *k, char *v);
void            ewl_theme_data_default_int_set(char *k, int v);

/**
 * @}
 */

#endif
