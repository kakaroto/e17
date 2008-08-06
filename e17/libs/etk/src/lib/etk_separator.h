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

/** @file etk_separator.h */
#ifndef _ETK_SEPARATOR_H_
#define _ETK_SEPARATOR_H_

#include "etk_types.h"
#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Separator Etk_Separator
 * @brief A separator is graphical bar used to separate two groups of widgets
 * @{
 */

/** Gets the type of a separator */
#define ETK_SEPARATOR_TYPE       (etk_separator_type_get())
/** Casts the object to an Etk_Separator */
#define ETK_SEPARATOR(obj)       (ETK_OBJECT_CAST((obj), ETK_SEPARATOR_TYPE, Etk_Separator))
/** Checks if the object is a Etk_Separator */
#define ETK_IS_SEPARATOR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SEPARATOR_TYPE))

/** Gets the type of a hseparator */
#define ETK_HSEPARATOR_TYPE       (etk_hseparator_type_get())
/** Casts the object to an Etk_HSeparator */
#define ETK_HSEPARATOR(obj)       (ETK_OBJECT_CAST((obj), ETK_HSEPARATOR_TYPE, Etk_HSeparator))
/** Checks if the object is an Etk_HSeparator */
#define ETK_IS_HSEPARATOR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HSEPARATOR_TYPE))

/** Gets the type of a vseparator */
#define ETK_VSEPARATOR_TYPE       (etk_vseparator_type_get())
/** Casts the object to an Etk_VSeparator */
#define ETK_VSEPARATOR(obj)       (ETK_OBJECT_CAST((obj), ETK_VSEPARATOR_TYPE, Etk_VSeparator))
/** Checks if the object is an Etk_VSeparator */
#define ETK_IS_VSEPARATOR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VSEPARATOR_TYPE))


/**
 * @brief @widget The structure of a separator
 * @structinfo
 */
struct Etk_Separator
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
};

/**
 * @brief @widget The structure of a hseparator
 * @structinfo
 */
struct Etk_HSeparator
{
   /* private: */
   /* Inherit from Etk_Separator */
   Etk_Separator separator;
};

/**
 * @brief @widget The structure of a vseparator
 * @structinfo
 */
struct Etk_VSeparator
{
   /* private: */
   /* Inherit from Etk_Separator */
   Etk_Separator separator;
};


Etk_Type   *etk_separator_type_get(void);
Etk_Type   *etk_hseparator_type_get(void);
Etk_Type   *etk_vseparator_type_get(void);

Etk_Widget *etk_hseparator_new(void);
Etk_Widget *etk_vseparator_new(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
