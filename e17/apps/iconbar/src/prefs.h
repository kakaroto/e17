#ifndef ICONBAR_PREFS_H_
#define ICONBAR_PREFS_H_

#include <Evas.h>
#include <Ecore_Evas.h>

/* start/stop */
void iconbar_config_init(void);
void iconbar_config_free(void);

/* modify */
void iconbar_config_ecore_evas_set(Ecore_Evas *ee);
void iconbar_config_home_set(char *home);
void iconbar_config_time_format_set(char *str);
void iconbar_config_font_path_append(char *str);
void iconbar_config_geometry_set(int x, int y, int w, int h);
void iconbar_config_icons_set(Evas_List *list);
void iconbar_config_theme_set(const char *theme);
void iconbar_config_raise_lower_set(int raise);
void iconbar_config_shaped_set(int on);
void iconbar_config_borderless_set(int on);
void iconbar_config_withdrawn_set(int on);
void iconbar_config_sticky_set(int on);

/* query */
const char *iconbar_config_theme_get(void);
const char *iconbar_config_home_get(void);
const char *iconbar_config_time_format_get(void);
Evas_List *iconbar_config_font_path_get(void);
Evas_List *iconbar_config_icons_get(void);
void iconbar_config_geometry_get(int *x, int *y, int *w, int *h);
int iconbar_config_raise_lower_get(void);
int iconbar_config_shaped_get(void);
int iconbar_config_borderless_get(void);
int iconbar_config_withdrawn_get(void);
int iconbar_config_sticky_get(void);

#endif
