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

/** @file etk_combobox.h */
#ifndef _ETK_COMBOBOX_H_
#define _ETK_COMBOBOX_H_

#include <stdarg.h>

#include "etk_widget.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO/FIXME list:
 * - Combobox item separator!
 * - selected/unselected signal for items
 * - Should items could have children (i.e sub-windows) ?
 * - make sure the items' children are not reparented
 * - etk_combobox_items_height_set() should maybe use theme-data to calculate the right item-height
 */

/**
 * @defgroup Etk_Combobox Etk_Combobox
 * @brief The Etk_Combobox widget is made up of a button that shows a popup menu-when it is clicked,
 * allowing the user to choose an item from a list
 * @{
 */

/** Gets the type of a combobox */
#define ETK_COMBOBOX_TYPE       (etk_combobox_type_get())
/** Casts the object to an Etk_Combobox */
#define ETK_COMBOBOX(obj)       (ETK_OBJECT_CAST((obj), ETK_COMBOBOX_TYPE, Etk_Combobox))
/** Check if the object is an Etk_Combobox */
#define ETK_IS_COMBOBOX(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_COMBOBOX_TYPE))

/** Gets the type of a combobox item */
#define ETK_COMBOBOX_ITEM_TYPE       (etk_combobox_item_type_get())
/** Casts the object to an Etk_Combobox_Item */
#define ETK_COMBOBOX_ITEM(obj)       (ETK_OBJECT_CAST((obj), ETK_COMBOBOX_ITEM_TYPE, Etk_Combobox_Item))
/** Check if the object is an Etk_Combobox_Item */
#define ETK_IS_COMBOBOX_ITEM(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_COMBOBOX_ITEM_TYPE))

extern int ETK_COMBOBOX_ITEM_ACTIVATED_SIGNAL;
extern int ETK_COMBOBOX_ACTIVE_ITEM_CHANGED_SIGNAL;

/** @brief The different types of widgets that can be contained by  a combobox's column */
typedef enum
{
   ETK_COMBOBOX_LABEL,          /**< The column's widget is a label */
   ETK_COMBOBOX_IMAGE,          /**< The column's widget is an image */
   ETK_COMBOBOX_OTHER           /**< The column's widget is any other type of widget */
} Etk_Combobox_Column_Type;

/** @brief Indicates how a widget of a combobox-column should fill and expand */
typedef enum
{
   ETK_COMBOBOX_NONE = 0,         /**< Don't resize the item */
   ETK_COMBOBOX_EXPAND = 1 << 0,  /**< Expand the item in it's container*/
   ETK_COMBOBOX_FILL = 1 << 1,    /**< Expand the item to fill the available space */
   ETK_COMBOBOX_EXPAND_FILL = ETK_COMBOBOX_EXPAND | ETK_COMBOBOX_FILL /**< Expand the item and fill the available space*/
} Etk_Combobox_Fill_Policy;


/**
 * @internal
 * @brief A column of a combobox: a combobox should have at least a column,
 * and each column contains a specific type of widget (see Etk_Combobox_Column_Type)
 */
struct Etk_Combobox_Column
{
   /* private: */
   Etk_Combobox_Column_Type type;
   Etk_Combobox_Fill_Policy fill_policy;
   int width;
   float align;
};

/**
 * @brief @widget A combobox's item made up of a button that shows a popup-menu when
 * it is clicked, allowing the user to choose an item from a list
 * @structinfo
 */
struct Etk_Combobox_Item
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Combobox *combobox;
   Etk_Combobox_Item *prev;
   Etk_Combobox_Item *next;

   Etk_Widget **widgets;
   void *data;
   void (*data_free_cb)(void *data);
};

/**
 * @brief @widget A combobox is a widget
 * @structinfo
 */
struct Etk_Combobox
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *button;
   Etk_Popup_Window *window;
   int popup_offset_x;
   int popup_offset_y;
   int popup_extra_w;

   int num_cols;
   Etk_Combobox_Column **cols;

   Etk_Combobox_Item *first_item;
   Etk_Combobox_Item *last_item;

   Etk_Combobox_Item *selected_item;
   Etk_Combobox_Item *active_item;
   Etk_Widget *active_item_widget;
   Etk_Widget **active_item_children;

   int items_height;
   Etk_Bool built:1;
};


Etk_Type          *etk_combobox_type_get(void);
Etk_Type          *etk_combobox_item_type_get(void);

Etk_Widget        *etk_combobox_new(void);
Etk_Widget        *etk_combobox_new_default(void);
void               etk_combobox_items_height_set(Etk_Combobox *combobox, int items_height);
int                etk_combobox_items_height_get(Etk_Combobox *combobox);

void               etk_combobox_column_add(Etk_Combobox *combobox, Etk_Combobox_Column_Type col_type, int width, Etk_Combobox_Fill_Policy fill_policy, float align);
void               etk_combobox_build(Etk_Combobox *combobox);

Etk_Combobox_Item *etk_combobox_item_prepend(Etk_Combobox *combobox, ...);
inline Etk_Combobox_Item *etk_combobox_item_prepend_empty(Etk_Combobox *combobox);
Etk_Combobox_Item *etk_combobox_item_append(Etk_Combobox *combobox, ...);
inline Etk_Combobox_Item *etk_combobox_item_append_empty(Etk_Combobox *combobox);
Etk_Combobox_Item *etk_combobox_item_insert(Etk_Combobox *combobox, Etk_Combobox_Item *after, ...);
Etk_Combobox_Item *etk_combobox_item_insert_valist(Etk_Combobox *combobox, Etk_Combobox_Item *after, va_list args);
Etk_Combobox_Item *etk_combobox_item_insert_empty(Etk_Combobox *combobox,
                                                  Etk_Combobox_Item *after);
void               etk_combobox_item_remove(Etk_Combobox_Item *item);
void               etk_combobox_clear(Etk_Combobox *combobox);

void               etk_combobox_fields_set(Etk_Combobox *combobox, ...);
void               etk_combobox_fields_set_valist(Etk_Combobox *combobox, va_list args);

void               etk_combobox_item_fields_set(Etk_Combobox_Item *item, ...);
void               etk_combobox_item_fields_set_valist(Etk_Combobox_Item *item, va_list args);
void               etk_combobox_item_field_set(Etk_Combobox_Item *item, int column, void *value);
void               etk_combobox_item_fields_get(Etk_Combobox_Item *item, ...);
void               etk_combobox_item_fields_get_valist(Etk_Combobox_Item *item, va_list args);
void *             etk_combobox_item_field_get(Etk_Combobox_Item *item, int column);

void               etk_combobox_item_data_set(Etk_Combobox_Item *item, void *data);
void               etk_combobox_item_data_set_full(Etk_Combobox_Item *item, void *data, void (*free_cb)(void *data));
void              *etk_combobox_item_data_get(Etk_Combobox_Item *item);

void               etk_combobox_active_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item);
Etk_Combobox_Item *etk_combobox_active_item_get(Etk_Combobox *combobox);
int                etk_combobox_active_item_num_get(Etk_Combobox *combobox);

Etk_Combobox_Item *etk_combobox_first_item_get(Etk_Combobox *combobox);
Etk_Combobox_Item *etk_combobox_last_item_get(Etk_Combobox *combobox);
Etk_Combobox_Item *etk_combobox_item_prev_get(Etk_Combobox_Item *item);
Etk_Combobox_Item *etk_combobox_item_next_get(Etk_Combobox_Item *item);
Etk_Combobox_Item *etk_combobox_nth_item_get(Etk_Combobox *combobox, int index);
Etk_Combobox      *etk_combobox_item_combobox_get(Etk_Combobox_Item *item);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
