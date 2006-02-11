/** @file etk_theme.h */
#ifndef _ETK_THEME_H_
#define _ETK_THEME_H_

/**
 * @defgroup Etk_Theme Etk_Theme
 * @{
 */

#include <Evas.h>
#include "etk_types.h"

void etk_theme_init();
void etk_theme_shutdown();

const char *etk_theme_widget_theme_get();
Etk_Bool etk_theme_widget_theme_set(const char *theme_name);
const char *etk_theme_widget_default_theme_get();

const char *etk_theme_icon_theme_get();
Etk_Bool etk_theme_icon_theme_set(const char *theme_name);
const char *etk_theme_icon_default_theme_get();

Evas_Object *etk_theme_object_load(Evas *evas, const char *filename, const char *group);
Evas_Object *etk_theme_object_load_from_widget_theme(Evas *evas, const char *group);
Evas_Object *etk_theme_object_load_from_widget_default_theme(Evas *evas, const char *group);
Evas_Object *etk_theme_object_load_from_icon_theme(Evas *evas, const char *group);
Evas_Object *etk_theme_object_load_from_icon_default_theme(Evas *evas, const char *group);

/** @} */

#endif
