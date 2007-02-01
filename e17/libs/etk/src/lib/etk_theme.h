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

/** @brief The different types of colors */
typedef enum Etk_Color_Type
{
   ETK_COLOR_FOREGROUND,          /**< The default foreground color */
   ETK_COLOR_BACKGROUND,          /**< The default background color */
   ETK_COLOR_IMPORTANT_FG,        /**< Foreground color to express importance */
   ETK_COLOR_IMPORTANT_BG,        /**< Background color to express importance */
   ETK_COLOR_WARNING_FG,          /**< Foreground color to express warning */
   ETK_COLOR_WARNING_BG,          /**< Background color to express warning */
   ETK_COLOR_INFO_FG,     	  /**< Foreground color to show informative areas */
   ETK_COLOR_INFO_BG,      	  /**< Background color to show informative areas */
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

Etk_Color etk_theme_color_get(Etk_Color_Type color_type);

/** @} */

#endif
