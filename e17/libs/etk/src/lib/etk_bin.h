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

/** @file etk_bin.h */
#ifndef _ETK_BIN_H_
#define _ETK_BIN_H_

#include "etk_container.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Bin Etk_Bin
 * @brief The Etk_Bin widget is a container that can contain only one child
 * @{
 */

/** Gets the type of a bin */
#define ETK_BIN_TYPE       (etk_bin_type_get())
/** Casts the object to an Etk_Bin */
#define ETK_BIN(obj)       (ETK_OBJECT_CAST((obj), ETK_BIN_TYPE, Etk_Bin))
/** Checks if the object is an Etk_Bin */
#define ETK_IS_BIN(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_BIN_TYPE))


/**
 * @brief @widget A container that can contain only one child
 * @structinfo
 */
struct Etk_Bin
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Etk_Widget *child;
};


Etk_Type   *etk_bin_type_get(void);
void        etk_bin_child_set(Etk_Bin *bin, Etk_Widget *child);
Etk_Widget *etk_bin_child_get(Etk_Bin *bin);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
