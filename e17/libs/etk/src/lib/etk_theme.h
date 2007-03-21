/** @file etk_theme.h */
#ifndef _ETK_THEME_H_
#define _ETK_THEME_H_

#include <Evas.h>
#include "etk_types.h"

/* TODO/FIXME list:
 * - Use etk_config_dir_get() to get the dirs where the themes should be looked for
 * - Better default colors?
 */

/**
 * @defgroup Etk_Theme The theme system of Etk
 * @brief The theme system of Etk
 * @{
 */

/** @brief The different types of colors */
typedef enum Etk_Color_Type
{
   ETK_COLOR_FOREGROUND,          /**< The default foreground color */
   ETK_COLOR_BACKGROUND,          /**< The default background color */
   ETK_COLOR_IMPORTANT_FG,        /**< Foreground color to express importance */
   ETK_COLOR_IMPORTANT_BG,        /**< Background color to express importance */
   ETK_COLOR_WARNING_FG,          /**< Foreground color to express warning */
   ETK_COLOR_WARNING_BG,          /**< Background color to express warning */
   ETK_COLOR_INFO_FG,             /**< Foreground color to show informative areas */
   ETK_COLOR_INFO_BG,             /**< Background color to show informative areas */
   ETK_COLOR_DEFAULT1_FG,         /**< Default foreground color 1 */
   ETK_COLOR_DEFAULT1_BG,         /**< Default background color 1 */
   ETK_COLOR_DEFAULT2_FG,         /**< Default foreground color 2 */
   ETK_COLOR_DEFAULT2_BG,         /**< Default background color 2 */
   ETK_COLOR_DEFAULT3_FG,         /**< Default foreground color 3 */
   ETK_COLOR_DEFAULT3_BG,         /**< Default background color 3 */
   ETK_COLOR_DEFAULT4_FG,         /**< Default foreground color 4 */
   ETK_COLOR_DEFAULT4_BG,         /**< Default background color 4 */
   ETK_COLOR_DEFAULT5_FG,         /**< Default foreground color 5 */
   ETK_COLOR_DEFAULT5_BG,         /**< Default background color 5 */
   ETK_COLOR_NUM_COLORS
} Etk_Color_Type;


void etk_theme_init(void);
void etk_theme_shutdown(void);

Etk_Bool    etk_theme_widget_set_from_path(const char *theme_path);
Etk_Bool    etk_theme_widget_set_from_name(const char *theme_name);
const char *etk_theme_widget_path_get(void);
char       *etk_theme_widget_name_get(void);
Evas_List  *etk_theme_widget_available_themes_get(void);
char       *etk_theme_widget_find(const char *theme_name);

Etk_Bool    etk_theme_icon_set_from_path(const char *theme_path);
Etk_Bool    etk_theme_icon_set_from_name(const char *theme_name);
const char *etk_theme_icon_path_get(void);
char       *etk_theme_icon_name_get(void);
Evas_List  *etk_theme_icon_available_themes_get(void);
char       *etk_theme_icon_find(const char *theme_name);

void        etk_theme_available_themes_free(Evas_List *themes);

Etk_Bool etk_theme_group_exists(const char *file, const char *group, const char *parent_group);
Etk_Bool etk_theme_edje_object_set(Evas_Object *object, const char *file, const char *group, const char *parent_group);
Etk_Bool etk_theme_edje_object_set_from_parent(Evas_Object *object, const char *group, Etk_Widget *parent);

Etk_Bool etk_theme_color_get(const char *file, Etk_Color_Type color_type, int *r, int *g, int *b, int *a);

/** @} */

#endif
