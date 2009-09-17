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

/** @file etk_toolbar.h */
#ifndef _ETK_TOOLBAR_H_
#define _ETK_TOOLBAR_H_

#include "etk_box.h"
#include "etk_container.h"
#include "etk_stock.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - When the toolbar is too large, we should clip it and add a popup-menu for the clipped tool-items
 * - Keyboard navigation (the toolbar should be focusable)
 */

/**
 * @defgroup Etk_Toolbar Etk_Toolbar
 * @brief The Etk_Toolbar is a widget that can contain widgets, mainly buttons,
 * packed in one direction (horizontal of vertical)
 * @{
 */

/** Gets the type of a toolbar */
#define ETK_TOOLBAR_TYPE       (etk_toolbar_type_get())
/** Casts the object to an Etk_Toolbar */
#define ETK_TOOLBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_TOOLBAR_TYPE, Etk_Toolbar))
/** Checks if the object is an Etk_Toolbar */
#define ETK_IS_TOOLBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TOOLBAR_TYPE))

/** @brief The orientation of a toolbar */
typedef enum
{
   ETK_TOOLBAR_HORIZ,           /**< The toolbar is horizontal */
   ETK_TOOLBAR_VERT             /**< The toolbar is vertical */
} Etk_Toolbar_Orientation;

/** @brief The style of the toolbar's buttons (icon, text, both vertically, both horizontally) */
typedef enum
{
   ETK_TOOLBAR_DEFAULT,          /**< Use Etk's default policy */
   ETK_TOOLBAR_ICON,             /**< Only the icon is visible */
   ETK_TOOLBAR_TEXT,             /**< Only the label is visible */
   ETK_TOOLBAR_BOTH_HORIZ,       /**< Both the icon and the label are visible, the icon is at the left of the label */
   ETK_TOOLBAR_BOTH_VERT         /**< Both the icon and the label are visible, the icon is above the label */
} Etk_Toolbar_Style;

/**
 * @brief @widget A  widget that can contain widgets, mainly buttons, packed in one direction
 * @structinfo
 */
struct Etk_Toolbar
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Etk_Widget *box;

   Etk_Toolbar_Orientation orientation;
   Etk_Toolbar_Style style;
   Etk_Stock_Size stock_size;
   Etk_Bool reorientating:1;
};


Etk_Type   *etk_toolbar_type_get(void);
Etk_Widget *etk_toolbar_new(void);

void etk_toolbar_append(Etk_Toolbar *toolbar, Etk_Widget *widget, Etk_Box_Group group);
void etk_toolbar_prepend(Etk_Toolbar *toolbar, Etk_Widget *widget, Etk_Box_Group group);
void etk_toolbar_insert_at(Etk_Toolbar *toolbar, Etk_Widget *widget, Etk_Box_Group group, int pos);

void                    etk_toolbar_orientation_set(Etk_Toolbar *toolbar, Etk_Toolbar_Orientation orientation);
Etk_Toolbar_Orientation etk_toolbar_orientation_get(Etk_Toolbar *toolbar);

void              etk_toolbar_style_set(Etk_Toolbar *toolbar, Etk_Toolbar_Style style);
Etk_Toolbar_Style etk_toolbar_style_get(Etk_Toolbar *toolbar);

void           etk_toolbar_stock_size_set(Etk_Toolbar *toolbar, Etk_Stock_Size size);
Etk_Stock_Size etk_toolbar_stock_size_get(Etk_Toolbar *toolbar);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
