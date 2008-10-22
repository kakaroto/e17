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

/** @file etk_toolbar.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_toolbar.h"

#include <stdlib.h>

#include "etk_button.h"
#include "etk_separator.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_theme.h"
#include "etk_tool_button.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Toolbar
 * @{
 */

#define ETK_IS_TOOL_ITEM(obj) \
   (ETK_IS_TOOL_BUTTON(obj) || ETK_IS_TOOL_TOGGLE_BUTTON(obj))

enum Etk_Toolbar_Property_Id
{
   ETK_TOOLBAR_ORIENTATION_PROPERTY,
   ETK_TOOLBAR_STYLE_PROPERTY,
   ETK_TOOLBAR_STOCK_SIZE_PROPERTY
};

static void _etk_toolbar_constructor(Etk_Toolbar *toolbar);
static void _etk_toolbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toolbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toolbar_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_toolbar_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
void _etk_toolbar_child_add(Etk_Container *container, Etk_Widget *widget);
void _etk_toolbar_child_remove(Etk_Container *container, Etk_Widget *widget);
Eina_List *_etk_toolbar_children_get(Etk_Container *container);
static Etk_Bool _etk_toolbar_child_added_cb(Etk_Object *object, Etk_Widget *child, void *data);
static Etk_Bool _etk_toolbar_child_removed_cb(Etk_Object *object, Etk_Widget *child, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Toolbar
 * @return Returns the type of an Etk_Toolbar
 */
Etk_Type *etk_toolbar_type_get(void)
{
   static Etk_Type *toolbar_type = NULL;

   if (!toolbar_type)
   {
      toolbar_type = etk_type_new("Etk_Toolbar", ETK_CONTAINER_TYPE, sizeof(Etk_Toolbar),
         ETK_CONSTRUCTOR(_etk_toolbar_constructor), NULL, NULL);

      etk_type_property_add(toolbar_type, "orientation", ETK_TOOLBAR_ORIENTATION_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_TOOLBAR_HORIZ));
      etk_type_property_add(toolbar_type, "style", ETK_TOOLBAR_STYLE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_TOOLBAR_BOTH_VERT));
      etk_type_property_add(toolbar_type, "stock-size", ETK_TOOLBAR_STOCK_SIZE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_STOCK_MEDIUM));

      toolbar_type->property_set = _etk_toolbar_property_set;
      toolbar_type->property_get = _etk_toolbar_property_get;
   }

   return toolbar_type;
}

/**
 * @brief Creates a new toolbar (horizontal by default)
 * @return Returns the new toolbar
 */
Etk_Widget *etk_toolbar_new(void)
{
   return etk_widget_new(ETK_TOOLBAR_TYPE, "theme-group", "htoolbar", NULL);
}

/**
 * @brief Appends an item to the start of the toolbar
 * @param toolbar a toolbar
 * @param widget the item to append
 */
void etk_toolbar_append(Etk_Toolbar *toolbar, Etk_Widget *widget, Etk_Box_Group group)
{
   if (!toolbar || !widget)
      return;
   etk_box_append(ETK_BOX(toolbar->box), widget, group, ETK_BOX_NONE, 0);
}

/**
 * @brief Prepends an item to the end of the toolbar
 * @param toolbar a toolbar
 * @param widget the item to append
 */
void etk_toolbar_prepend(Etk_Toolbar *toolbar, Etk_Widget *widget, Etk_Box_Group group)
{
   if (!toolbar || !widget)
      return;
   etk_box_prepend(ETK_BOX(toolbar->box), widget, group, ETK_BOX_NONE, 0);
}

/**
 * @brief Adds an item to the toolbar at a specified position
 * @param toolbar a toolbar
 * @param widget the item to add
 * @param pos the position to add the item at
 */
void etk_toolbar_insert_at(Etk_Toolbar *toolbar, Etk_Widget *widget, Etk_Box_Group group, int pos)
{
   if (!toolbar || !widget)
      return;
   etk_box_insert_at(ETK_BOX(toolbar->box), widget, group, pos, ETK_BOX_NONE, 0);
}

/**
 * @brief Sets the toolbar's orientation (horizontal or vertical)
 * @param toolbar a toolbar
 * @param orientation the orientation to set
 */
void etk_toolbar_orientation_set(Etk_Toolbar *toolbar, Etk_Toolbar_Orientation orientation)
{
   Eina_List *children, *l;
   Etk_Widget *prev_box;

   if (!toolbar || toolbar->orientation == orientation)
      return;

   toolbar->reorientating = ETK_TRUE;
   prev_box = toolbar->box;
   toolbar->orientation = orientation;
   if (toolbar->orientation == ETK_TOOLBAR_VERT)
      toolbar->box = etk_vbox_new(ETK_FALSE, 0);
   else
      toolbar->box = etk_hbox_new(ETK_FALSE, 0);
   etk_widget_internal_set(toolbar->box, ETK_TRUE);
   etk_widget_show(toolbar->box);

   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(toolbar->box), ETK_CALLBACK(_etk_toolbar_child_added_cb), toolbar);
   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(toolbar->box), ETK_CALLBACK(_etk_toolbar_child_removed_cb), NULL);


   children = etk_container_children_get(ETK_CONTAINER(prev_box));
   for (l = children; l; l = l->next)
      etk_toolbar_append(toolbar, ETK_WIDGET(l->data), ETK_BOX_START);
   eina_list_free(children);
   etk_object_destroy(ETK_OBJECT(prev_box));

   if (toolbar->orientation == ETK_TOOLBAR_VERT)
      etk_widget_theme_group_set(ETK_WIDGET(toolbar), "vtoolbar");
   else
      etk_widget_theme_group_set(ETK_WIDGET(toolbar), "htoolbar");
   etk_widget_parent_set(toolbar->box, ETK_WIDGET(toolbar));

   toolbar->reorientating = ETK_FALSE;
   etk_object_notify(ETK_OBJECT(toolbar), "orientation");
}

/**
 * @brief Gets the toolbar's orientation (horizontal or vertical)
 * @param toolbar a toolbar
 * @return Returns the orientation of the toolbar
 */
Etk_Toolbar_Orientation etk_toolbar_orientation_get(Etk_Toolbar *toolbar)
{
   if (!toolbar)
      return ETK_FALSE;
   return toolbar->orientation;
}

/**
 * @brief Sets the style of toolbar's tool-buttons (icon, text, both vertically, both horizontally)
 * @param toolbar a toolbar
 * @param style the style to set
 */
void etk_toolbar_style_set(Etk_Toolbar *toolbar, Etk_Toolbar_Style style)
{
   Eina_List *children, *l;
   Etk_Button_Style button_style;

   if (!toolbar || toolbar->style == style)
      return;

   toolbar->style = style;
   switch (style)
   {
      case ETK_TOOLBAR_ICON:
         button_style = ETK_BUTTON_ICON;
         break;
      case ETK_TOOLBAR_TEXT:
         button_style = ETK_BUTTON_TEXT;
         break;
      case ETK_TOOLBAR_BOTH_VERT:
         button_style = ETK_BUTTON_BOTH_VERT;
         break;
      case ETK_TOOLBAR_BOTH_HORIZ:
         button_style = ETK_BUTTON_BOTH_HORIZ;
         break;
      default:
         button_style = ETK_BUTTON_BOTH_VERT;
         break;
   }

   children = etk_container_children_get(ETK_CONTAINER(toolbar->box));
   for (l = children; l; l = l->next)
   {
      if (ETK_IS_TOOL_ITEM(l->data))
         etk_button_style_set(ETK_BUTTON(l->data), button_style);
   }
   eina_list_free(children);

   etk_object_notify(ETK_OBJECT(toolbar), "style");
}

/**
 * @brief Gets the style of toolbar's tool-buttons (icon, text, both vertically, both horizontally)
 * @param toolbar a toolbar
 * @return Returns the style of toolbar's tool-buttons
 */
Etk_Toolbar_Style etk_toolbar_style_get(Etk_Toolbar *toolbar)
{
   if (!toolbar)
      return ETK_TOOLBAR_DEFAULT;
   return toolbar->style;
}

/**
 * @brief Sets the stock-size of the toolbar's tool-buttons
 * @param toolbar a toolbar
 * @param size the stock-size to use
 */
void etk_toolbar_stock_size_set(Etk_Toolbar *toolbar, Etk_Stock_Size size)
{
   Eina_List *children, *l;

   if (!toolbar || toolbar->stock_size == size)
      return;

   toolbar->stock_size = size;
   children = etk_container_children_get(ETK_CONTAINER(toolbar->box));
   for (l = children; l; l = l->next)
   {
      if (ETK_IS_TOOL_ITEM(l->data))
	 etk_button_stock_size_set(ETK_BUTTON(l->data), size);
   }
   eina_list_free(children);

   etk_object_notify(ETK_OBJECT(toolbar), "stock-size");
}

/**
 * @brief Gets the stock-size of the toolbar's tool-buttons
 * @param toolbar a toolbar
 * @return Returns the stock-size of the toolbar's tool-buttons
 */
Etk_Stock_Size etk_toolbar_stock_size_get(Etk_Toolbar *toolbar)
{
   if (!toolbar)
      return ETK_STOCK_MEDIUM;
   return toolbar->stock_size;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members and build the toolbar */
static void _etk_toolbar_constructor(Etk_Toolbar *toolbar)
{
   if (!toolbar)
      return;

   toolbar->style = ETK_TOOLBAR_BOTH_VERT;
   toolbar->orientation = ETK_TOOLBAR_HORIZ;
   toolbar->stock_size = ETK_STOCK_MEDIUM;
   toolbar->reorientating = ETK_FALSE;

   ETK_WIDGET(toolbar)->size_request = _etk_toolbar_size_request;
   ETK_WIDGET(toolbar)->size_allocate = _etk_toolbar_size_allocate;
   ETK_CONTAINER(toolbar)->child_add = _etk_toolbar_child_add;
   ETK_CONTAINER(toolbar)->child_remove = _etk_toolbar_child_remove;
   ETK_CONTAINER(toolbar)->children_get = _etk_toolbar_children_get;

   toolbar->box = etk_hbox_new(ETK_FALSE, 0);
   etk_widget_parent_set(toolbar->box, ETK_WIDGET(toolbar));
   etk_widget_internal_set(ETK_WIDGET(toolbar->box), ETK_TRUE);
   etk_widget_show(toolbar->box);

   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(toolbar->box), ETK_CALLBACK(_etk_toolbar_child_added_cb), toolbar);
   etk_signal_connect_by_code(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(toolbar->box), ETK_CALLBACK(_etk_toolbar_child_removed_cb), NULL);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_toolbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Toolbar *toolbar;

   if (!(toolbar = ETK_TOOLBAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TOOLBAR_ORIENTATION_PROPERTY:
         etk_toolbar_orientation_set(toolbar, etk_property_value_int_get(value));
         break;
      case ETK_TOOLBAR_STYLE_PROPERTY:
         etk_toolbar_style_set(toolbar, etk_property_value_int_get(value));
         break;
      case ETK_TOOLBAR_STOCK_SIZE_PROPERTY:
         etk_toolbar_stock_size_set(toolbar, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_toolbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Toolbar *toolbar;

   if (!(toolbar = ETK_TOOLBAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TOOLBAR_ORIENTATION_PROPERTY:
         etk_property_value_int_set(value, toolbar->orientation);
         break;
      case ETK_TOOLBAR_STYLE_PROPERTY:
         etk_property_value_int_set(value, toolbar->style);
         break;
      case ETK_TOOLBAR_STOCK_SIZE_PROPERTY:
         etk_property_value_int_set(value, toolbar->stock_size);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the toolbar */
static void _etk_toolbar_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Toolbar *toolbar;

   if (!(toolbar = ETK_TOOLBAR(widget)) || !size)
      return;
   etk_widget_size_request(ETK_WIDGET(toolbar->box), size);
}

/* Resizes the toolbar to the allocated size */
static void _etk_toolbar_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Toolbar *toolbar;
   Etk_Size size;

   if (!(toolbar = ETK_TOOLBAR(widget)))
      return;

   etk_widget_size_request(toolbar->box, &size);
   geometry.w = ETK_MAX(geometry.w, size.w);
   etk_widget_size_allocate(ETK_WIDGET(toolbar->box), geometry);
}

/* Adds a child to the toolbar */
void _etk_toolbar_child_add(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Toolbar *toolbar;

   if (!(toolbar = ETK_TOOLBAR(container)) || !widget)
      return;
   etk_toolbar_append(toolbar, widget, ETK_BOX_START);
}

/* Removes the child from the toolbar */
void _etk_toolbar_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Toolbar *toolbar;

   if (!(toolbar = ETK_TOOLBAR(container)) || !widget)
      return;
   etk_container_remove(widget);
}

/* Gets the list of the children of the toolbar */
Eina_List *_etk_toolbar_children_get(Etk_Container *container)
{
   Etk_Toolbar *toolbar;

   if (!(toolbar = ETK_TOOLBAR(container)))
      return NULL;
   return etk_container_children_get(ETK_CONTAINER(toolbar->box));
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when a widget is added to the toolbar's box */
static Etk_Bool _etk_toolbar_child_added_cb(Etk_Object *object, Etk_Widget *child, void *data)
{
   Etk_Toolbar *toolbar;

   if (!(toolbar = ETK_TOOLBAR(data)) || !child)
      return ETK_TRUE;

   /* Sets the style and the stock size if the new child is a tool-item */
   if (ETK_IS_TOOL_ITEM(child))
   {
      Etk_Button_Style button_style;

      switch (toolbar->style)
      {
         case ETK_TOOLBAR_ICON:
            button_style = ETK_BUTTON_ICON;
            break;
         case ETK_TOOLBAR_TEXT:
            button_style = ETK_BUTTON_TEXT;
            break;
         case ETK_TOOLBAR_BOTH_VERT:
            button_style = ETK_BUTTON_BOTH_VERT;
            break;
         case ETK_TOOLBAR_BOTH_HORIZ:
            button_style = ETK_BUTTON_BOTH_HORIZ;
            break;
         default:
            button_style = ETK_BUTTON_BOTH_VERT;
            break;
      }
      etk_button_style_set(ETK_BUTTON(child), button_style);
      etk_button_stock_size_set(ETK_BUTTON(child), toolbar->stock_size);
   }
   /* Reorientate the separators */
   else if (toolbar->reorientating
      && (((toolbar->orientation == ETK_TOOLBAR_HORIZ) && ETK_IS_VSEPARATOR(child))
         || ((toolbar->orientation == ETK_TOOLBAR_VERT) && ETK_IS_HSEPARATOR(child))))
   {
      Etk_Bool visible;
      Etk_Box_Fill_Policy policy;
      Etk_Box_Group group;
      int pos, padding;

      visible = etk_widget_is_visible(child);
      etk_box_child_position_get(ETK_BOX(toolbar->box), child, &group, &pos);
      etk_box_child_packing_get(ETK_BOX(toolbar->box), child, &policy, &padding);

      etk_object_destroy(ETK_OBJECT(child));
      if (toolbar->orientation == ETK_TOOLBAR_HORIZ)
         child = etk_vseparator_new();
      else
         child = etk_hseparator_new();

      etk_box_insert_at(ETK_BOX(toolbar->box), child, group, pos, policy, padding);
      if (visible)
         etk_widget_show(child);
   }

   if (etk_theme_group_exists(etk_widget_theme_file_get(ETK_WIDGET(toolbar)),
      etk_widget_theme_group_get(child), etk_widget_theme_group_get(ETK_WIDGET(toolbar))))
   {
      etk_widget_theme_parent_set(child, ETK_WIDGET(toolbar));
   }
   //etk_signal_emit(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(toolbar), child);

   return ETK_TRUE;
}

/* Called when a widget is removed from the toolbar's box */
static Etk_Bool _etk_toolbar_child_removed_cb(Etk_Object *object, Etk_Widget *child, void *data)
{
   Etk_Toolbar *toolbar;

   if (!(toolbar = ETK_TOOLBAR(data)) || !child)
      return ETK_TRUE;

   if (etk_widget_theme_parent_get(child) == ETK_WIDGET(toolbar))
      etk_widget_theme_parent_set(child, NULL);
   etk_signal_emit(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(toolbar), child);

   return ETK_TRUE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Toolbar
 *
 * @image html widgets/toolbar.png
 * Some toolbar description here
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Toolbar
 *
 * \par Properties:
 * @prop_name "orientation": The orientation of the toolbar, can be ETK_TOOLBAR_HORIZ or ETK_TOOLBAR_VERT.
 * @prop_type Integer (Etk_Toolbar_Orientation)
 * @prop_rw
 * @prop_val ETK_TOOLBAR_HORIZ
 * \par
 * @prop_name "style": The buttons look in the toolbar, can be one of ETK_BUTTON_ICON, ETK_BUTTON_TEXT, ETK_BUTTON_BOTH_HORIZ or ETK_BUTTON_BOTH_VERT.
 * @prop_type Integer (Etk_Button_Style)
 * @prop_rw
 * @prop_val ETK_TOOLBAR_BOTH_VERT
 * \par
 * @prop_name "stock-size":  The size of the stock-icons used by buttons.
 * @prop_type Integer (Etk_Stock_Size)
 * @prop_rw
 * @prop_val ETK_STOCK_MEDIUM
 */
