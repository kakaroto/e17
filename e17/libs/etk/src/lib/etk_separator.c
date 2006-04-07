/** @file etk_separator.c */
#include "etk_separator.h"
#include <stdlib.h>

/**
 * @addtogroup Etk_Separator
 * @{
 */

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_HSeparator
 * @return Returns the type on an Etk_HSeparator
 */
Etk_Type *etk_hseparator_type_get()
{
   static Etk_Type *hseparator_type = NULL;

   if (!hseparator_type)
      hseparator_type = etk_type_new("Etk_HSeparator", ETK_WIDGET_TYPE, sizeof(Etk_HSeparator), NULL, NULL);

   return hseparator_type;
}

/**
 * @brief Creates a new hseparator
 * @return Returns the new hseparator widget
 */
Etk_Widget *etk_hseparator_new()
{
   return etk_widget_new(ETK_HSEPARATOR_TYPE, "theme_group", "hseparator", NULL);
}

/**
 * @brief Gets the type of an Etk_VSeparator
 * @return Returns the type on an Etk_VSeparator
 */
Etk_Type *etk_vseparator_type_get()
{
   static Etk_Type *vseparator_type = NULL;

   if (!vseparator_type)
      vseparator_type = etk_type_new("Etk_VSeparator", ETK_WIDGET_TYPE, sizeof(Etk_VSeparator), NULL, NULL);

   return vseparator_type;
}

/**
 * @brief Creates a new vseparator
 * @return Returns the new vseparator widget
 */
Etk_Widget *etk_vseparator_new()
{
   return etk_widget_new(ETK_VSEPARATOR_TYPE, "theme_group", "vseparator", NULL);
}

/** @} */
