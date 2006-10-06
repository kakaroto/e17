/** @file etk_theme.h */
#ifndef _ETK_THEME_H_
#define _ETK_THEME_H_

/**
 * @defgroup Etk_Theme The theme system of Etk
 * @brief The theme system of Etk
 * @{
 */

#include <Evas.h>
#include "etk_types.h"

void etk_theme_init();
void etk_theme_shutdown();

const char *etk_theme_widget_get();
Etk_Bool    etk_theme_widget_set(const char *theme_name);
Evas_List  *etk_theme_widget_available_themes_get();

const char *etk_theme_icon_get();
Etk_Bool    etk_theme_icon_set(const char *theme_name);
Evas_List  *etk_theme_icon_available_themes_get();

Etk_Bool etk_theme_group_exists(const char *file, const char *group, const char *parent_group);
Etk_Bool etk_theme_edje_object_set(Evas_Object *object, const char *file, const char *group, const char *parent_group);
Etk_Bool etk_theme_edje_object_set_from_parent(Evas_Object *object, const char *group, Etk_Widget *parent);

/** @} */

#endif
