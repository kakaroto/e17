/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_separator.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
 * @internal
 * @brief Gets the type of an Etk_Separator
 * @return Returns the type of an Etk_Separator
 */
Etk_Type *etk_separator_type_get(void)
{
   static Etk_Type *separator_type = NULL;

   if (!separator_type)
      separator_type = etk_type_new("Etk_Separator", ETK_WIDGET_TYPE,
         sizeof(Etk_Separator), NULL, NULL, NULL);

   return separator_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_HSeparator
 * @return Returns the type of an Etk_HSeparator
 */
Etk_Type *etk_hseparator_type_get(void)
{
   static Etk_Type *hseparator_type = NULL;

   if (!hseparator_type)
      hseparator_type = etk_type_new("Etk_HSeparator", ETK_SEPARATOR_TYPE,
         sizeof(Etk_HSeparator), NULL, NULL, NULL);

   return hseparator_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_VSeparator
 * @return Returns the type of an Etk_VSeparator
 */
Etk_Type *etk_vseparator_type_get(void)
{
   static Etk_Type *vseparator_type = NULL;

   if (!vseparator_type)
      vseparator_type = etk_type_new("Etk_VSeparator", ETK_SEPARATOR_TYPE,
         sizeof(Etk_VSeparator), NULL, NULL, NULL);

   return vseparator_type;
}

/**
 * @brief Creates a new hseparator
 * @return Returns the new hseparator widget
 */
Etk_Widget *etk_hseparator_new(void)
{
   return etk_widget_new(ETK_HSEPARATOR_TYPE, "theme-group", "hseparator", NULL);
}

/**
 * @brief Creates a new vseparator
 * @return Returns the new vseparator widget
 */
Etk_Widget *etk_vseparator_new(void)
{
   return etk_widget_new(ETK_VSEPARATOR_TYPE, "theme-group", "vseparator", NULL);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Separator
 *
 * @image html widgets/separator.png
 * Etk_Separator is the base class for Etk_HSeparator (horizontal separator) and Etk_VSeparator (vertical separator)
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Separator
 *       - Etk_HSeparator
 *       - Etk_VSeparator
 */
