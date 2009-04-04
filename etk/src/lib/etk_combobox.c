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

/** @file etk_combobox.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_combobox.h"

#include <stdlib.h>
#include <string.h>

#include "etk_container.h"
#include "etk_event.h"
#include "etk_image.h"
#include "etk_label.h"
#include "etk_popup_window.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_toggle_button.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Combobox
 * @{
 */

#define MIN_ITEM_HEIGHT 12
#define DEFAULT_ITEM_HEIGHT 24

int ETK_COMBOBOX_ITEM_ACTIVATED_SIGNAL;
int ETK_COMBOBOX_ACTIVE_ITEM_CHANGED_SIGNAL;

enum Etk_Combobox_Property_Id
{
   ETK_COMBOBOX_ITEMS_HEIGHT_PROPERTY,
   ETK_COMBOBOX_ACTIVE_ITEM_PROPERTY
};

static void _etk_combobox_constructor(Etk_Combobox *combobox);
static void _etk_combobox_destructor(Etk_Combobox *combobox);
static void _etk_combobox_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_combobox_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_combobox_item_constructor(Etk_Combobox_Item *item);
static void _etk_combobox_item_destructor(Etk_Combobox_Item *item);
static Etk_Bool _etk_combobox_item_destroyed_cb(Etk_Object *object, void *data);
static void _etk_combobox_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_combobox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_window_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_combobox_window_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_active_item_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_combobox_active_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_button_theme_signal_emit(Etk_Widget *widget, const char *name, Etk_Bool size_recalc);
static void _etk_combobox_item_theme_signal_emit(Etk_Widget *widget, const char *name, Etk_Bool size_recalc);

static Etk_Bool _etk_combobox_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_label_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_focused_cb(Etk_Widget *widget, void *data);
static Etk_Bool _etk_combobox_unfocused_cb(Etk_Widget *widget, void *data);
static Etk_Bool _etk_combobox_enabled_cb(Etk_Widget *widget, void *data);
static Etk_Bool _etk_combobox_disabled_cb(Etk_Widget *widget, void *data);
static Etk_Bool _etk_combobox_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_combobox_button_toggled_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_window_popped_down_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_window_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_combobox_item_entered_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_item_left_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_item_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);

static void _etk_combobox_selected_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item);
static void _etk_combobox_item_cells_render(Etk_Combobox *combobox, Etk_Widget **cells, Etk_Geometry geometry, Etk_Bool ignore_other);
static void _etk_combobox_widgets_emit_theme_signal(Etk_Combobox *combobox, Etk_Widget **widgets, const char *name, Etk_Bool size_recalc);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Combobox
 * @return Returns the type of an Etk_Combobox
 */
Etk_Type *etk_combobox_type_get(void)
{
   static Etk_Type *combobox_type = NULL;

   if (!combobox_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_COMBOBOX_ITEM_ACTIVATED_SIGNAL,
            "item-activated", etk_marshaller_OBJECT),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_COMBOBOX_ACTIVE_ITEM_CHANGED_SIGNAL,
            "active-item-changed", etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      combobox_type = etk_type_new("Etk_Combobox", ETK_WIDGET_TYPE,
         sizeof(Etk_Combobox), ETK_CONSTRUCTOR(_etk_combobox_constructor),
         ETK_DESTRUCTOR(_etk_combobox_destructor), signals);

      etk_type_property_add(combobox_type, "items-height", ETK_COMBOBOX_ITEMS_HEIGHT_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(DEFAULT_ITEM_HEIGHT));
      etk_type_property_add(combobox_type, "active-item", ETK_COMBOBOX_ACTIVE_ITEM_PROPERTY,
            ETK_PROPERTY_OBJECT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_object(NULL));

      combobox_type->property_set = _etk_combobox_property_set;
      combobox_type->property_get = _etk_combobox_property_get;
   }

   return combobox_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_Combobox_Item
 * @return Returns the type of an Etk_Combobox_Item
 */
Etk_Type *etk_combobox_item_type_get(void)
{
   static Etk_Type *combobox_item_type = NULL;

   if (!combobox_item_type)
   {
      combobox_item_type = etk_type_new("Etk_Combobox_Item", ETK_WIDGET_TYPE,
         sizeof(Etk_Combobox_Item),
         ETK_CONSTRUCTOR(_etk_combobox_item_constructor),
         ETK_DESTRUCTOR(_etk_combobox_item_destructor), NULL);
   }

   return combobox_item_type;
}

/**
 * @brief Creates a new combobox, with no column
 * @return Returns the new combobox widget
 */
Etk_Widget *etk_combobox_new(void)
{
   return etk_widget_new(ETK_COMBOBOX_TYPE, "theme-group", "combobox",
         "focusable", ETK_TRUE, "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new combobox, made up of a unique column containing a label,
 * and already built (no need to call etk_combobox_build())
 * @return Returns the new combobox widget
 */
Etk_Widget *etk_combobox_new_default(void)
{
   Etk_Widget *combobox;

   combobox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_LABEL, 100, ETK_COMBOBOX_EXPAND, 0.0);
   etk_combobox_build(ETK_COMBOBOX(combobox));

   return combobox;
}

/**
 * @brief Set the height of the combobox's items. The default height is 24, the minimum is 12
 * @param combobox a combobox
 * @param items_height the height that the combobox's items should have
 */
void etk_combobox_items_height_set(Etk_Combobox *combobox, int items_height)
{
   if (!combobox)
      return;

   items_height = ETK_MAX(MIN_ITEM_HEIGHT, items_height);
   if (items_height != combobox->items_height)
   {
      combobox->items_height = items_height;
      etk_widget_size_recalc_queue(ETK_WIDGET(combobox->button));
      etk_widget_size_recalc_queue(ETK_WIDGET(combobox->window));
      etk_object_notify(ETK_OBJECT(combobox), "items-height");
   }
}

/**
 * @brief Gets the height of the combobox's items
 * @param combobox a combobox
 * @return Returns the height of the combobox's items
 */
int etk_combobox_items_height_get(Etk_Combobox *combobox)
{
   if (!combobox)
      return 0;
   return combobox->items_height;
}

/**
 * @brief Adds a column to the combobox. The combobox should not be already be built
 * @param combobox a combobox
 * @param col_type the type of widget that will be packed in the column
 * @param width the width of the column. If @a expand is ETK_TRUE, the column may be bigger if it expands
 * @param fill_policy the fill-policy of the widget of the column, it indicates how it should fill the column
 * @param align the horizontal alignment of the widget of the column, from 0.0 (left) to 1.0 (right)
 */
void etk_combobox_column_add(Etk_Combobox *combobox, Etk_Combobox_Column_Type col_type, int width, Etk_Combobox_Fill_Policy fill_policy, float align)
{
   Etk_Combobox_Column *col;

   if (!combobox)
      return;
   if (combobox->built)
   {
      ETK_WARNING("Unable to add a new column to the combobox because the combobox has been built");
      return;
   }

   combobox->cols = realloc(combobox->cols, (combobox->num_cols + 1) * sizeof(Etk_Combobox_Column *));
   combobox->cols[combobox->num_cols] = malloc(sizeof(Etk_Combobox_Column));
   col = combobox->cols[combobox->num_cols];
   col->type = col_type;
   col->width = width;
   col->fill_policy = fill_policy;
   col->align = ETK_CLAMP(align, 0.0, 1.0);
   combobox->num_cols++;
}

/**
 * @brief Builds the combobox: you have to build a combobox after you have added all the columns to it and
 * before you start adding items. Items can not be added if the combobox is not built
 * @param combobox the combobox to build
 */
void etk_combobox_build(Etk_Combobox *combobox)
{
   int i;

   if (!combobox || combobox->built)
      return;

   combobox->active_item_widget = etk_widget_new(ETK_WIDGET_TYPE, "pass-mouse-events", ETK_TRUE,
         "visible", ETK_TRUE, "internal", ETK_TRUE, NULL);
   combobox->active_item_widget->size_request = _etk_combobox_active_item_size_request;
   combobox->active_item_widget->size_allocate = _etk_combobox_active_item_size_allocate;
   etk_object_data_set(ETK_OBJECT(combobox->active_item_widget), "_Etk_Combobox_Window::Combobox", combobox);
   etk_container_add(ETK_CONTAINER(combobox->button), combobox->active_item_widget);

   /* Build the active-item's children */
   if (combobox->num_cols > 0)
   {
      combobox->active_item_children = malloc(combobox->num_cols * sizeof(Etk_Widget *));
      for (i = 0; i < combobox->num_cols; i++)
      {
         switch (combobox->cols[i]->type)
         {
            case ETK_COMBOBOX_LABEL:
               combobox->active_item_children[i] = etk_label_new(NULL);
               etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(combobox->active_item_children[i]),
                     ETK_CALLBACK(_etk_combobox_label_realized_cb), combobox);
               etk_widget_theme_parent_set(combobox->active_item_children[i], combobox->button);
               break;
            case ETK_COMBOBOX_IMAGE:
               combobox->active_item_children[i] = etk_image_new();
               break;
            default:
               combobox->active_item_children[i] = NULL;
               break;
         }
         etk_widget_internal_set(combobox->active_item_children[i], ETK_TRUE);
         etk_widget_parent_set(combobox->active_item_children[i], combobox->active_item_widget);
         etk_widget_show(combobox->active_item_children[i]);
      }
   }
   else
      combobox->active_item_children = NULL;

   if (combobox->active_item)
      etk_combobox_active_item_set(combobox, combobox->active_item);

   combobox->built = ETK_TRUE;
}

/**
 * @brief Inserts a new item at the start of the combobox
 *
 * @param combobox a combobox
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox,
 * one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the
 *   argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the
 *   argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the
 *   argument must be an "Etk_Widget *"
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item will be automatically shown, so
 * you won't have to call etk_widget_show()
 */
Etk_Combobox_Item *etk_combobox_item_prepend(Etk_Combobox *combobox, ...)
{
   Etk_Combobox_Item *item;
   va_list args;

   va_start(args, combobox);
   item = etk_combobox_item_insert_valist(combobox, NULL, args);
   va_end(args);

   return item;
}

/**
 * @brief Inserts a new empty item at the start of the combobox
 *
 * @param combobox a combobox
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item will be automatically shown, so
 * you won't have to call etk_widget_show()
 */
inline Etk_Combobox_Item *etk_combobox_item_prepend_empty(
   Etk_Combobox *combobox)
{
   return etk_combobox_item_insert_empty(combobox, NULL);
}

/**
 * @brief Inserts a new item at the end of the combobox
 *
 * @param combobox a combobox
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox,
 * one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the
 *   argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the
 *   argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the
 *   argument must be an "Etk_Widget *"
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item will be automatically shown, so
 * you won't have to call etk_widget_show()
 */
Etk_Combobox_Item *etk_combobox_item_append(Etk_Combobox *combobox, ...)
{
   Etk_Combobox_Item *item;
   va_list args;

   va_start(args, combobox);
   item = etk_combobox_item_insert_valist(combobox, combobox->last_item, args);
   va_end(args);

   return item;
}

/**
 * @brief Inserts a new empty item at the end of the combobox
 *
 * @param combobox a combobox
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item will be automatically shown, so
 * you won't have to call etk_widget_show()
 */
inline Etk_Combobox_Item *etk_combobox_item_append_empty(Etk_Combobox *combobox)
{
   return etk_combobox_item_insert_empty(combobox, combobox->last_item);
}

/**
 * @brief Inserts a new item after an existing item of the combobox
 *
 * @param combobox a combobox
 * @param after the item after which the new item should be inserted. If @a
 * after is NULL, the new item will be inserted at the start of the combobox
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox,
 * one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the
 *   argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the
 *   argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the
 *   argument must be an "Etk_Widget *"
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item will be automatically shown, so
 * you won't have to call etk_widget_show()

*/
Etk_Combobox_Item *etk_combobox_item_insert(Etk_Combobox *combobox,
                                            Etk_Combobox_Item *after, ...)
{
   Etk_Combobox_Item *item;
   va_list args;

   va_start(args, after);
   item = etk_combobox_item_insert_valist(combobox, after, args);
   va_end(args);

   return item;
}

/**
 * @brief Inserts a new item after an existing item of the combobox
 *
 * @param combobox a combobox
 * @param after the item after which the new item should be inserted. If @a
 * after is NULL, the new item will be inserted at the start of the combobox
 * @param args the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox,
 * one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the
 *   argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the
 *   argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the
 *   argument must be an "Etk_Widget *"
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
Etk_Combobox_Item *etk_combobox_item_insert_valist(Etk_Combobox *combobox,
                                                   Etk_Combobox_Item *after,
                                                   va_list args)
{
   Etk_Combobox_Item *item;
   va_list args2;

   item = etk_combobox_item_insert_empty(combobox, after);
   if (!item)
      return NULL;

   va_copy(args2, args);
   etk_combobox_item_fields_set_valist(item, args2);
   va_end(args2);

   return item;
}

/**
 * @brief Inserts a new empty item after an existing item of the combobox
 *
 * @param combobox a combobox
 * @param after the item after which the new item should be inserted. If @a
 * after is NULL, the new item will be inserted at the start of the combobox
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
Etk_Combobox_Item *etk_combobox_item_insert_empty(Etk_Combobox *combobox,
                                                  Etk_Combobox_Item *after)
{
   Etk_Combobox_Item *item;

   if (!combobox)
      return NULL;
   if (!combobox->built)
   {
      ETK_WARNING("Unable to add a new item to the combobox because "
                  "etk_combobox_build() has not been called yet");
      return NULL;
   }
   if (after && combobox != after->combobox)
   {
      ETK_WARNING("Unable to add a new item after item %p because this item "
                  "does not belong to the combobox in which the new item "
                  "should be added", after);
      return NULL;
   }

   item = ETK_COMBOBOX_ITEM(etk_widget_new(ETK_COMBOBOX_ITEM_TYPE,
      "theme-group", "item", "theme-parent", combobox, "visible", ETK_TRUE,
      NULL));
   item->combobox = combobox;

   item->prev = after;
   item->next = after ? after->next : NULL;
   if (after)
      after->next = item;
   else
      combobox->first_item = item;
   if (combobox->last_item == after)
      combobox->last_item = item;

   /* Adds the corresponding widgets to the new item */
   item->widgets = calloc(combobox->num_cols, sizeof(Etk_Widget *));
   ETK_WIDGET(item)->size_allocate = _etk_combobox_item_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_ENTERED_SIGNAL, ETK_OBJECT(item),
      ETK_CALLBACK(_etk_combobox_item_entered_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_LEFT_SIGNAL, ETK_OBJECT(item),
      ETK_CALLBACK(_etk_combobox_item_left_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(item),
      ETK_CALLBACK(_etk_combobox_item_mouse_up_cb), NULL);

   etk_widget_parent_set(ETK_WIDGET(item), ETK_WIDGET(combobox->window));

   if (!combobox->active_item)
      etk_combobox_active_item_set(combobox, item);

   return item;
}

/**
 * @brief Removes an item from the combobox. The item will be freed and should not be used anymore
 * @param item the item to remove from its combobox
 * @note You can also call etk_object_destroy() to remove an item from a combobox, it has the same effect
 */
void etk_combobox_item_remove(Etk_Combobox_Item *item)
{
   etk_object_destroy(ETK_OBJECT(item));
}

/**
 * @brief Removes all the items of the combobox. All the items will be freed and should not be used anymore
 * @param combobox a combobox
 */
void etk_combobox_clear(Etk_Combobox *combobox)
{
   if (!combobox)
      return;

   while (combobox->first_item)
      etk_combobox_item_remove(combobox->first_item);
}

/**
 * @brief Sets the values of the cells of the combobox
 * @param combobox a combobox
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget *"
 */
void etk_combobox_fields_set(Etk_Combobox *combobox, ...)
{
   va_list args;

   if (!combobox)
      return;

   va_start(args, combobox);
   etk_combobox_fields_set_valist(combobox, args);
   va_end(args);
}

/**
 * @brief Sets the values of the cells of the combobox
 * @param combobox a combobox
 * @param args the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget *"
 */
void etk_combobox_fields_set_valist(Etk_Combobox *combobox, va_list args)
{
   int i;
   Etk_Widget *widget;

   if (!combobox)
      return;

   if (!combobox->built)
   {
      ETK_WARNING("Unable to set the specified fields. The combobox is not built yet.");
      return;
   }

   for (i = 0; i < combobox->num_cols; i++)
   {
      switch (combobox->cols[i]->type)
      {
         case ETK_COMBOBOX_LABEL:
            etk_label_set(ETK_LABEL(combobox->active_item_children[i]), va_arg(args, char *));
            break;
         case ETK_COMBOBOX_IMAGE:
            widget = ETK_WIDGET(va_arg(args, Etk_Widget *));
            if (widget)
               etk_image_copy(ETK_IMAGE(combobox->active_item_children[i]), ETK_IMAGE(widget));
            else
               etk_image_set_from_file(ETK_IMAGE(combobox->active_item_children[i]), NULL, NULL);
            break;
         case ETK_COMBOBOX_OTHER:
            widget = va_arg(args, Etk_Widget *);
            break;
         default:
            break;
      }
   }
}

/**
 * @brief Sets the values of the cells of the combobox item
 * @param item a combobox item
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget *"
 * @note The new widgets of the item will be automatically shown
 */
void etk_combobox_item_fields_set(Etk_Combobox_Item *item, ...)
{
   va_list args;

   va_start(args, item);
   etk_combobox_item_fields_set_valist(item, args);
   va_end(args);
}

/**
 * @brief Sets the values of the cells of the combobox item. The current widgets of item will be destroyed
 * @param item a combobox item
 * @param args the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget *"
 * @note The new widgets of the item will be automatically shown
 */
void etk_combobox_item_fields_set_valist(Etk_Combobox_Item *item, va_list args)
{
   Etk_Combobox *combobox;
   int i;

   if (!item || !(combobox = item->combobox))
      return;

   for (i = 0; i < combobox->num_cols; i++)
   {
      switch (combobox->cols[i]->type)
      {
         if (item->widgets[i])
            etk_object_destroy(ETK_OBJECT(item->widgets[i]));

         case ETK_COMBOBOX_LABEL:
            item->widgets[i] = etk_label_new(va_arg(args, char *));
            etk_widget_pass_mouse_events_set(item->widgets[i], ETK_TRUE);
            break;
         case ETK_COMBOBOX_IMAGE:
            item->widgets[i] = ETK_WIDGET(va_arg(args, Etk_Widget *));
            etk_widget_pass_mouse_events_set(item->widgets[i], ETK_TRUE);
            break;
         case ETK_COMBOBOX_OTHER:
            item->widgets[i] = ETK_WIDGET(va_arg(args, Etk_Widget *));
            break;
         default:
            item->widgets[i] = NULL;
            break;
      }
      etk_widget_parent_set(item->widgets[i], ETK_WIDGET(item));
      etk_widget_show(item->widgets[i]);
   }

   if (combobox->active_item == item)
      etk_combobox_active_item_set(combobox, item);
}

/**
 * @brief Sets the value of the one column of the combobox item. The current widgets of item will be destroyed
 * @param item a combobox item
 * @param column the column to set the value of
 * @param value the different widget to attach to the column of the item:
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget *"
 * @note The new widgets of the item will be automatically shown
 */
void etk_combobox_item_field_set(Etk_Combobox_Item *item, int column, void *value)
{
   Etk_Combobox *combobox;

   if (!item || !(combobox = item->combobox) || (column >= combobox->num_cols))
      return;

   switch (combobox->cols[column]->type)
   {
      if (item->widgets[column])
         etk_object_destroy(ETK_OBJECT(item->widgets[column]));

      case ETK_COMBOBOX_LABEL:
         item->widgets[column] = etk_label_new((const char *) value);
         etk_widget_pass_mouse_events_set(item->widgets[column], ETK_TRUE);
         break;
      case ETK_COMBOBOX_IMAGE:
         item->widgets[column] = ETK_WIDGET((Etk_Widget *) value);
         etk_widget_pass_mouse_events_set(item->widgets[column], ETK_TRUE);
         break;
      case ETK_COMBOBOX_OTHER:
         item->widgets[column] = ETK_WIDGET((Etk_Widget *) value);
         break;
      default:
         item->widgets[column] = NULL;
         break;
   }
   etk_widget_parent_set(item->widgets[column], ETK_WIDGET(item));
   etk_widget_show(item->widgets[column]);

   if (combobox->active_item == item)
      etk_combobox_active_item_set(combobox, item);
}



/**
 * @brief Gets the values of the cells of the combobox item
 * @param item a combobox item
 * @param ... the location where to store the different values of the cells of the item:
 * there must be as many arguments as the number of columns in the combobox, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the argument must be a "const char **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget **"
 */
void etk_combobox_item_fields_get(Etk_Combobox_Item *item, ...)
{
   va_list args;

   va_start(args, item);
   etk_combobox_item_fields_get_valist(item, args);
   va_end(args);
}

/**
 * @brief Gets the values of the cells of the combobox item
 * @param item a combobox item
 * @param args the location where to store the different values of the cells of the item:
 * there must be as many arguments as the number of columns in the combobox, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the argument must be a "const char **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget **"
 */
void etk_combobox_item_fields_get_valist(Etk_Combobox_Item *item, va_list args)
{
   Etk_Combobox *combobox;
   int i;

   if (!item || !(combobox = item->combobox))
      return;

   for (i = 0; i < combobox->num_cols; i++)
   {
      switch (combobox->cols[i]->type)
      {
         case ETK_COMBOBOX_LABEL:
         {
            const char **label;
            if ((label = va_arg(args, const char **)))
               *label = etk_label_get(ETK_LABEL(item->widgets[i]));
            break;
         }
         case ETK_COMBOBOX_IMAGE:
         case ETK_COMBOBOX_OTHER:
         {
            Etk_Widget **widget;
            if ((widget = va_arg(args, Etk_Widget **)))
               *widget = item->widgets[i];
            break;
         }
         default:
         {
            void **data;
            if ((data = va_arg(args, void **)))
               *data = NULL;
            break;
         }
      }
      etk_widget_parent_set(item->widgets[i], ETK_WIDGET(item));
      etk_widget_show(item->widgets[i]);
   }
}

/**
 * @brief Gets the value of the cell of a column of the combobox item
 * @param item a combobox item
 * @param column the column to get the values of
 * @return The value of the cell as a void pointer that you must cast to the appropriate type:
 * - If the type of the corresponding column is ETK_COMBOBOX_LABEL, the argument must be a "const char **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget **"
 */
void * etk_combobox_item_field_get(Etk_Combobox_Item *item, int column)
{
   Etk_Combobox *combobox;

   if (!item || !(combobox = item->combobox) || (column >= combobox->num_cols))
      return NULL;

   switch (combobox->cols[column]->type)
   {
      case ETK_COMBOBOX_LABEL:
      {
         const char *label;
         label = etk_label_get(ETK_LABEL(item->widgets[column]));
	 return (void *)label;
         break;
      }
      case ETK_COMBOBOX_IMAGE:
      case ETK_COMBOBOX_OTHER:
      {
         Etk_Widget *widget;
         widget = item->widgets[column];
	 return (void *) widget;
         break;
      }
      default:
	 return NULL;
         break;
   }
// FIXME: are these needed here?
//   etk_widget_parent_set(item->widgets[column], ETK_WIDGET(item));
//   etk_widget_show(item->widgets[column]);
}


/**
 * @brief Sets the data associated to the combobox item
 * @param item a combobox item
 * @param data the data to associate to the combobox item
 */
void etk_combobox_item_data_set(Etk_Combobox_Item *item, void *data)
{
   if (!item)
      return;
   etk_combobox_item_data_set_full(item, data, NULL);
}

/**
 * @brief Sets the data associated to the combobox item
 * @param item a combobox item
 * @param data the data to associate to the combobox item
 * @param free_cb a function to call to free the data when the item is destroyed or when the data is changed
 */
void etk_combobox_item_data_set_full(Etk_Combobox_Item *item, void *data, void (*free_cb)(void *data))
{
   if (!item)
      return;

   if (item->data_free_cb && item->data && (item->data != data))
      item->data_free_cb(item->data);
   item->data = data;
   item->data_free_cb = free_cb;
}

/**
 * @brief Gets the data associated to the combobox item
 * @param item a combobox item
 * @return Returns the data associated to the combobox item
 */
void *etk_combobox_item_data_get(Etk_Combobox_Item *item)
{
   if (!item)
      return NULL;
   return item->data;
}

/**
 * @brief Sets the active item of the combobox
 * @param combobox a combobox
 * @param item the item to set as active
 */
void etk_combobox_active_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item)
{
   int i;

   if (!combobox)
      return;
   if ((item && item->combobox != combobox) || !combobox->built)
   {
      ETK_WARNING("Unable to change the active-item of the combobox. The specified item does not belong "
            "to the combobox or the combobox is not built yet.");
      return;
   }

   /* Changes the children of the active-item widget */
   for (i = 0; i < combobox->num_cols; i++)
   {
      switch (combobox->cols[i]->type)
      {
         case ETK_COMBOBOX_LABEL:
            etk_label_set(ETK_LABEL(combobox->active_item_children[i]),
               item ? etk_label_get(ETK_LABEL(item->widgets[i])) : NULL);
            break;
         case ETK_COMBOBOX_IMAGE:
            if (item)
               etk_image_copy(ETK_IMAGE(combobox->active_item_children[i]), ETK_IMAGE(item->widgets[i]));
            else
               etk_image_set_from_file(ETK_IMAGE(combobox->active_item_children[i]), NULL, NULL);
            break;
         default:
            break;
      }
   }

   if (item)
      etk_signal_emit(ETK_COMBOBOX_ITEM_ACTIVATED_SIGNAL, ETK_OBJECT(combobox), item);

   if (combobox->active_item != item)
   {
      combobox->active_item = item;
      etk_signal_emit(ETK_COMBOBOX_ACTIVE_ITEM_CHANGED_SIGNAL, ETK_OBJECT(combobox));
      etk_object_notify(ETK_OBJECT(combobox), "active-item");
   }
}

/**
 * @brief Gets the active item of the combobox
 * @param combobox a combobox
 * @return Returns the active item of combobox (NULL if none)
 */
Etk_Combobox_Item *etk_combobox_active_item_get(Etk_Combobox *combobox)
{
   if (!combobox)
      return NULL;
   return combobox->active_item;
}

/**
 * @brief Gets the index of the current active item of the combobox
 * @param combobox a combobox
 * @return Returns the index of the current active item, or -1 if there is no active item
 */
int etk_combobox_active_item_num_get(Etk_Combobox *combobox)
{
   Etk_Combobox_Item *item;
   int i;

   if (!combobox || !combobox->active_item)
      return -1;

   for (i = 0, item = combobox->first_item; item; i++, item = item->next)
   {
      if (item == combobox->active_item)
         return i;
   }
   return -1;
}

/**
 * @brief Gets the first item of a combobox
 * @param combobox a combobox
 * @return Returns the first item of a combobox
 */
Etk_Combobox_Item *etk_combobox_first_item_get(Etk_Combobox *combobox)
{
   if (!combobox)
      return NULL;
   return combobox->first_item;
}

/**
 * @brief Gets the last item of a combobox
 * @param combobox a combobox
 * @return Returns the last item of a combobox
 */
Etk_Combobox_Item *etk_combobox_last_item_get(Etk_Combobox *combobox)
{
   if (!combobox)
      return NULL;
   return combobox->last_item;
}

/**
 * @brief Gets the item previous to the given combobox item
 * @param item a combobox item
 * @return Returns the item previous to @a item, or NULL if the given item is the first one
 */
Etk_Combobox_Item *etk_combobox_item_prev_get(Etk_Combobox_Item *item)
{
   if (!item)
      return NULL;
   return item->prev;
}

/**
 * @brief Gets the item next to the given combobox item
 * @param item a combobox item
 * @return Returns the item next to @a item, or NULL if the given item is the last one
 */
Etk_Combobox_Item *etk_combobox_item_next_get(Etk_Combobox_Item *item)
{
   if (!item)
      return NULL;
   return item->next;
}

/**
 * @brief Gets the nth item of the combobox
 * @param combobox a combobox
 * @param index the index of the item to get, starting from 0. If @a index is < 0 or if @a index is greater than
 * the current number of items of the combobox, the function will return NULL
 * @return Returns the nth item of the combobox, or NULL on failure
 */
Etk_Combobox_Item *etk_combobox_nth_item_get(Etk_Combobox *combobox, int index)
{
   Etk_Combobox_Item *item;
   int i;

   if (!combobox || index < 0)
      return NULL;

   for (item = combobox->first_item, i = 0; item && i < index; item = item->next)
      i++;
   return item;
}

/**
 * @brief Gets the combobox containing the given item
 * @param item a combobox item
 * @return Returns the combobox containing the given item
 */
Etk_Combobox *etk_combobox_item_combobox_get(Etk_Combobox_Item *item)
{
   if (!item)
      return NULL;
   return item->combobox;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the combobox */
static void _etk_combobox_constructor(Etk_Combobox *combobox)
{
   if (!combobox)
      return;

   combobox->button = etk_widget_new(ETK_TOGGLE_BUTTON_TYPE, "theme-group", "button", "theme-parent", combobox,
         "visible", ETK_TRUE, "repeat-mouse-events", ETK_TRUE, "focusable", ETK_FALSE, "internal", ETK_TRUE, NULL);
   etk_object_data_set(ETK_OBJECT(combobox->button), "_Etk_Combobox_Button::Combobox", combobox);
   etk_widget_parent_set(combobox->button, ETK_WIDGET(combobox));
   ETK_WIDGET(combobox->button)->theme_signal_emit = _etk_combobox_button_theme_signal_emit;

   combobox->window = ETK_POPUP_WINDOW(etk_widget_new(ETK_POPUP_WINDOW_TYPE,
         "theme-group", "combobox/window", "theme-parent", combobox, NULL));
   etk_object_data_set(ETK_OBJECT(combobox->window), "_Etk_Combobox_Window::Combobox", combobox);
   ETK_WIDGET(combobox->window)->size_request = _etk_combobox_window_size_request;
   ETK_WIDGET(combobox->window)->size_allocate = _etk_combobox_window_size_allocate;

   combobox->popup_offset_x = 0;
   combobox->popup_offset_y = 0;
   combobox->popup_extra_w = 0;
   combobox->num_cols = 0;
   combobox->cols = NULL;
   combobox->first_item = NULL;
   combobox->last_item = NULL;
   combobox->selected_item = NULL;
   combobox->active_item = NULL;
   combobox->active_item_widget = NULL;
   combobox->active_item_children = NULL;
   combobox->items_height = -1;
   combobox->built = ETK_FALSE;

   ETK_WIDGET(combobox)->size_request = _etk_combobox_size_request;
   ETK_WIDGET(combobox)->size_allocate = _etk_combobox_size_allocate;

   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(combobox->button), ETK_CALLBACK(_etk_combobox_button_toggled_cb), combobox);
   etk_signal_connect_by_code(ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL, ETK_OBJECT(combobox->window), ETK_CALLBACK(_etk_combobox_window_popped_down_cb), combobox);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(combobox->window), ETK_CALLBACK(_etk_combobox_window_key_down_cb), combobox);

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(combobox), ETK_CALLBACK(_etk_combobox_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_FOCUSED_SIGNAL, ETK_OBJECT(combobox), ETK_CALLBACK(_etk_combobox_focused_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNFOCUSED_SIGNAL, ETK_OBJECT(combobox), ETK_CALLBACK(_etk_combobox_unfocused_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_ENABLED_SIGNAL, ETK_OBJECT(combobox), ETK_CALLBACK(_etk_combobox_enabled_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_DISABLED_SIGNAL, ETK_OBJECT(combobox), ETK_CALLBACK(_etk_combobox_disabled_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(combobox), ETK_CALLBACK(_etk_combobox_key_down_cb), NULL);
}

/* Destroys the combobox */
static void _etk_combobox_destructor(Etk_Combobox *combobox)
{
   int i;

   if (!combobox)
      return;

   combobox->selected_item = NULL;
   combobox->active_item = NULL;
   etk_combobox_clear(combobox);
   free(combobox->active_item_children);

   for (i = 0; i < combobox->num_cols; i++)
      free(combobox->cols[i]);
   free(combobox->cols);

   etk_object_destroy(ETK_OBJECT(combobox->window));
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_combobox_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_COMBOBOX_ITEMS_HEIGHT_PROPERTY:
         etk_combobox_items_height_set(combobox, etk_property_value_int_get(value));
         break;
      case ETK_COMBOBOX_ACTIVE_ITEM_PROPERTY:
         etk_combobox_active_item_set(combobox, ETK_COMBOBOX_ITEM(etk_property_value_object_get(value)));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_combobox_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_COMBOBOX_ITEMS_HEIGHT_PROPERTY:
         etk_property_value_int_set(value, combobox->items_height);
         break;
      case ETK_COMBOBOX_ACTIVE_ITEM_PROPERTY:
         etk_property_value_object_set(value, ETK_OBJECT(combobox->active_item));
         break;
      default:
         break;
   }
}

/* Initializes the combobox item */
static void _etk_combobox_item_constructor(Etk_Combobox_Item *item)
{
   if (!item)
      return;

   item->combobox = NULL;
   item->prev = NULL;
   item->next = NULL;
   item->widgets = NULL;
   item->data = NULL;
   item->data_free_cb = NULL;

   ETK_WIDGET(item)->theme_signal_emit = _etk_combobox_item_theme_signal_emit;
   etk_signal_connect_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_item_destroyed_cb), NULL);
}

/* Destroys the combobox item */
static void _etk_combobox_item_destructor(Etk_Combobox_Item *item)
{
   if (!item)
      return;

   if (item->data && item->data_free_cb)
      item->data_free_cb(item->data);
   free(item->widgets);
}

/* Called when a combobox-item is destroyed. We use this function and not the destructor because
 * the destructor is not called immediately after the etk_object_destroy() call, but we want to remove the item
 * immediately from the combobox (the destructor is still used the free the memory though) */
static Etk_Bool _etk_combobox_item_destroyed_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Item *item;
   Etk_Combobox *combobox;

   if (!(item = ETK_COMBOBOX_ITEM(object)) || !(combobox = item->combobox))
      return ETK_TRUE;

   if (item->combobox->selected_item == item)
      _etk_combobox_selected_item_set(item->combobox, NULL);
   if (item->combobox->active_item == item)
      etk_combobox_active_item_set(item->combobox, NULL);

   if (item->prev)
      item->prev->next = item->next;
   if (item->next)
      item->next->prev = item->prev;
   if (combobox->first_item == item)
      combobox->first_item = item->next;
   if (combobox->last_item == item)
      combobox->last_item = item->prev;

   return ETK_TRUE;
}

/* Calculates the ideal size of the combobox */
static void _etk_combobox_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(widget)) || !size)
      return;
   etk_widget_size_request(combobox->button, size);
}

/* Resizes the combobox to the allocated geometry */
static void _etk_combobox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(widget)))
      return;
   etk_widget_size_allocate(combobox->button, geometry);
}

/* Calculates the ideal size of the combobox window */
static void _etk_combobox_window_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Combobox *combobox;
   Etk_Combobox_Item *item;
   int i, num_visible_items;

   if (!widget || !size)
      return;
   if (!(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Window::Combobox"))))
      return;

   size->w = 0;
   for (i = 0; i < combobox->num_cols; i++)
      size->w += combobox->cols[i]->width;

   num_visible_items = 0;
   for (item = combobox->first_item; item; item = item->next)
   {
      if (etk_widget_is_visible(ETK_WIDGET(item)))
         num_visible_items++;
   }

   size->h = num_visible_items * combobox->items_height;
}

/* Resizes the combobox window to the allocated geometry */
static void _etk_combobox_window_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox *combobox;
   Etk_Combobox_Item *item;

   if (!widget || !(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Window::Combobox"))))
      return;

   geometry.h = combobox->items_height;
   for (item = combobox->first_item; item; item = item->next)
   {
      if (etk_widget_is_visible(ETK_WIDGET(item)))
      {
         etk_widget_size_allocate(ETK_WIDGET(item), geometry);
         geometry.y += combobox->items_height;
      }
   }
}

/* Resizes the combobox item to the allocated geometry */
static void _etk_combobox_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox_Item *item;
   Etk_Combobox *combobox;

   if (!(item = ETK_COMBOBOX_ITEM(widget)) || !(combobox = item->combobox))
      return;

   _etk_combobox_item_cells_render(combobox, item->widgets, geometry, ETK_FALSE);
}

/* Calculates the ideal size of the active item of the combobox (the item in the combobox button */
static void _etk_combobox_active_item_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Combobox *combobox;
   int i;

   if (!widget || !size)
      return;
   if (!(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Window::Combobox"))))
      return;

   size->w = 0;
   size->h = 0;
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (combobox->cols[i]->type != ETK_COMBOBOX_OTHER)
         size->w += combobox->cols[i]->width;
   }
}

/* Resizes the active item of the combobox to the allocated geometry */
static void _etk_combobox_active_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox *combobox;

   if (!widget)
      return;
   if (!(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Window::Combobox"))))
      return;

   _etk_combobox_item_cells_render(combobox, combobox->active_item_children, geometry, ETK_TRUE);
}

/* Called when a theme-signal is emitted on the combobox's button. We use this to make sure the labels of the
 * combobox's button receive the same theme-signal as the combobox-s button */
static void _etk_combobox_button_theme_signal_emit(Etk_Widget *widget, const char *name, Etk_Bool size_recalc)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Button::Combobox"))))
      return;
   _etk_combobox_widgets_emit_theme_signal(combobox, combobox->active_item_children, name, size_recalc);
}

/* Called when a theme-signal is emitted on an item of the combobox. We use this to make sure the labels of the
 * item receive the same theme-signal as the item itself */
static void _etk_combobox_item_theme_signal_emit(Etk_Widget *widget, const char *name, Etk_Bool size_recalc)
{
   Etk_Combobox_Item *item;

   if (!(item = ETK_COMBOBOX_ITEM(widget)))
      return;
   _etk_combobox_widgets_emit_theme_signal(item->combobox, item->widgets, name, size_recalc);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the combobox is realized */
static Etk_Bool _etk_combobox_realized_cb(Etk_Object *object, void *data)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(object)))
      return ETK_TRUE;

   /* Read the data defined in the theme */
   if (etk_widget_theme_data_get(ETK_WIDGET(combobox), "popup_offset_x", "%d", &combobox->popup_offset_x) != 1)
      combobox->popup_offset_x = 0;
   if (etk_widget_theme_data_get(ETK_WIDGET(combobox), "popup_offset_y", "%d", &combobox->popup_offset_y) != 1)
      combobox->popup_offset_y = 0;
   if (etk_widget_theme_data_get(ETK_WIDGET(combobox), "popup_extra_width", "%d", &combobox->popup_extra_w) != 1)
      combobox->popup_extra_w = 0;
   if (combobox->items_height == -1 && etk_widget_theme_data_get(ETK_WIDGET(combobox), "item_height", "%d", &combobox->items_height) != 1)
      combobox->items_height = DEFAULT_ITEM_HEIGHT;

   return ETK_TRUE;
}

/* Called when a child-label of the combobox's button is realized */
static Etk_Bool _etk_combobox_label_realized_cb(Etk_Object *object, void *data)
{
   Etk_Widget *label;
   Etk_Combobox *combobox;

   if (!(label = ETK_WIDGET(data)) || !(combobox = ETK_COMBOBOX(data)))
      return ETK_TRUE;

   if (etk_widget_disabled_get(ETK_WIDGET(combobox)))
      etk_widget_theme_signal_emit(label, "etk,state,disabled", ETK_FALSE);
   if (etk_widget_is_focused(ETK_WIDGET(combobox)))
      etk_widget_theme_signal_emit(label, "etk,state,focused", ETK_FALSE);
   if (etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(combobox->button)))
      etk_widget_theme_signal_emit(label, "etk,state,on", ETK_FALSE);

   return ETK_TRUE;
}

/* Called when the combobox is focused */
static Etk_Bool _etk_combobox_focused_cb(Etk_Widget *widget, void *data)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(widget)))
      return ETK_TRUE;
   etk_widget_theme_signal_emit(combobox->button, "etk,state,focused", ETK_FALSE);
   return ETK_TRUE;
}

/* Called when the combobox is unfocused */
static Etk_Bool _etk_combobox_unfocused_cb(Etk_Widget *widget, void *data)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(widget)))
      return ETK_TRUE;
   etk_widget_theme_signal_emit(combobox->button, "etk,state,unfocused", ETK_FALSE);
   return ETK_TRUE;
}

/* Called when the combobox is enabled */
static Etk_Bool _etk_combobox_enabled_cb(Etk_Widget *widget, void *data)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(widget)))
      return ETK_TRUE;
   etk_widget_disabled_set(combobox->button, ETK_FALSE);
   return ETK_TRUE;
}

/* Called when the combobox is disabled */
static Etk_Bool _etk_combobox_disabled_cb(Etk_Widget *widget, void *data)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(widget)))
      return ETK_TRUE;
   etk_widget_disabled_set(combobox->button, ETK_TRUE);
   return ETK_TRUE;
}

/* Called when a key is pressed while the combobox is focused*/
static Etk_Bool _etk_combobox_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(object)))
      return ETK_TRUE;

   if (strcmp(event->keyname, "space") == 0)
   {
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(combobox->button), ETK_TRUE);
      return ETK_FALSE;
   }
   return ETK_TRUE;
}

/* Called when the combobox button is toggled */
static Etk_Bool _etk_combobox_button_toggled_cb(Etk_Object *object, void *data)
{
   int ex, ey, sx, sy;
   int bx, by, bw, bh;
   Etk_Combobox *combobox;
   Etk_Toplevel *toplevel;

   if (!(combobox = ETK_COMBOBOX(data)) || !(toplevel = etk_widget_toplevel_parent_get(combobox->button)))
      return ETK_TRUE;

   if (etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(combobox->button)))
   {
      etk_widget_geometry_get(combobox->button, &bx, &by, &bw, &bh);
      etk_toplevel_evas_position_get(toplevel, &ex, &ey);
      etk_toplevel_screen_position_get(toplevel, &sx, &sy);
      etk_popup_window_popup_at_xy(combobox->window,
            sx + (bx - ex) + combobox->popup_offset_x,
            sy + (by - ey) + bh + combobox->popup_offset_y);
      etk_window_resize(ETK_WINDOW(combobox->window), bw + combobox->popup_extra_w, 0);
   }

   return ETK_TRUE;
}

/* Called when the combobox window is popped down */
static Etk_Bool _etk_combobox_window_popped_down_cb(Etk_Object *object, void *data)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(data)))
      return ETK_TRUE;
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(combobox->button), ETK_FALSE);
   return ETK_TRUE;
}

/* Called when a key is pressed on the combobox window */
static Etk_Bool _etk_combobox_window_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Combobox *combobox;

   if (!(combobox = ETK_COMBOBOX(data)))
      return ETK_TRUE;

   if (strcmp(event->keyname, "Down") == 0)
   {
      if (!combobox->first_item)
         return ETK_TRUE;

      if (!combobox->selected_item || !combobox->selected_item->next)
         _etk_combobox_selected_item_set(combobox, combobox->first_item);
      else
         _etk_combobox_selected_item_set(combobox, combobox->selected_item->next);
   }
   else if (strcmp(event->keyname, "Up") == 0)
   {
      if (!combobox->first_item)
         return ETK_TRUE;

      if (!combobox->selected_item || !combobox->selected_item->next)
         _etk_combobox_selected_item_set(combobox, combobox->last_item);
      else
         _etk_combobox_selected_item_set(combobox, combobox->selected_item->prev);
   }
   else if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "space") == 0
         || strcmp(event->keyname, "KP_Enter") == 0)
   {
      if (combobox->selected_item)
      {
         etk_combobox_active_item_set(combobox, combobox->selected_item);
         if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "KP_Enter") == 0)
            etk_popup_window_popdown(combobox->window);
      }
   }
   else if (strcmp(event->keyname, "Escape") == 0)
      etk_popup_window_popdown(combobox->window);

   return ETK_TRUE;
}

/* Called when the mouse enters the item */
static Etk_Bool _etk_combobox_item_entered_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Item *item;

   if (!(item = ETK_COMBOBOX_ITEM(object)))
      return ETK_TRUE;
   _etk_combobox_selected_item_set(item->combobox, item);
   return ETK_TRUE;
}

/* Called when the mouse leaves the item */
static Etk_Bool _etk_combobox_item_left_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Item *item;

   if (!(item = ETK_COMBOBOX_ITEM(object)))
      return ETK_TRUE;

   if (item->combobox->selected_item == item)
      _etk_combobox_selected_item_set(item->combobox, NULL);
   return ETK_TRUE;
}

/* Called when the mouse releases the item */
static Etk_Bool _etk_combobox_item_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Combobox_Item *item;

   if (!(item = ETK_COMBOBOX_ITEM(object)))
      return ETK_TRUE;
   if (event->button != 1 && event->button != 3)
      return ETK_TRUE;
   if (event->modifiers & ETK_MODIFIER_ON_HOLD)
      return ETK_TRUE;

   etk_combobox_active_item_set(item->combobox, item);
   if (event->button == 1)
      etk_popup_window_popdown(item->combobox->window);
   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Sets the selected item of the the combobox */
static void _etk_combobox_selected_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item)
{
   if (!combobox)
      return;

   if (combobox->selected_item)
   {
      etk_widget_theme_signal_emit(ETK_WIDGET(combobox->selected_item), "etk,state,unselected", ETK_FALSE);
      combobox->selected_item = NULL;
   }
   if (item)
   {
      etk_widget_theme_signal_emit(ETK_WIDGET(item), "etk,state,selected", ETK_FALSE);
      combobox->selected_item = item;
   }
}

/* Renders the different cells of a combobox item */
static void _etk_combobox_item_cells_render(Etk_Combobox *combobox, Etk_Widget **cells, Etk_Geometry geometry, Etk_Bool ignore_other)
{
   Etk_Geometry col_geometry;
   Etk_Geometry child_geometry;
   int num_expandable_cols = 0;
   int expandable_width = 0;
   int total_width = 0;
   int last_col = 0;
   int i;

   for (i = 0; i < combobox->num_cols; i++)
   {
      if (ignore_other && (combobox->cols[i]->type == ETK_COMBOBOX_OTHER))
         continue;

      if (combobox->cols[i]->fill_policy & ETK_COMBOBOX_EXPAND)
      {
         num_expandable_cols++;
         expandable_width += combobox->cols[i]->width;
      }
      total_width += combobox->cols[i]->width;
      last_col = i;
   }

   col_geometry.x = geometry.x;
   col_geometry.y = geometry.y;
   col_geometry.h = geometry.h;
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (ignore_other && (combobox->cols[i]->type == ETK_COMBOBOX_OTHER))
         continue;

      if (num_expandable_cols == 0)
      {
         if (i == last_col)
            col_geometry.w = geometry.w - (col_geometry.x - geometry.x);
         else
            col_geometry.w = combobox->cols[i]->width;
      }
      else
      {
         if (combobox->cols[i]->fill_policy & ETK_COMBOBOX_EXPAND)
         {
            col_geometry.w = (combobox->cols[i]->width / (float)expandable_width) *
                  (geometry.w - (total_width - expandable_width));
         }
         else
            col_geometry.w = combobox->cols[i]->width;
      }

      if (cells[i])
      {
         child_geometry = col_geometry;
         etk_container_child_space_fill(cells[i], &child_geometry,
               combobox->cols[i]->fill_policy & ETK_COMBOBOX_FILL, ETK_TRUE, combobox->cols[i]->align, 0.5);
         etk_widget_size_allocate(cells[i], child_geometry);
      }
      col_geometry.x += col_geometry.w;
   }
}

/* Emits the given theme-signal to the labels among "widgets" */
static void _etk_combobox_widgets_emit_theme_signal(Etk_Combobox *combobox, Etk_Widget **widgets, const char *name, Etk_Bool size_recalc)
{
   int i;

   if (!combobox || !widgets)
      return;

   for (i = 0; i < combobox->num_cols; i++)
   {
      if (combobox->cols[i]->type == ETK_COMBOBOX_LABEL)
         etk_widget_theme_signal_emit(widgets[i], name, size_recalc);
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Combobox
 *
 * @image html widgets/combobox.png
 * Once the combobox is created, at least one column must be added to it with etk_combobox_column_add().
 * Each column can contain a specific widget type. @n
 * Once all the column are created, the combobox has to be built with etk_combobox_build(),
 * in order to be able to add items to it. No item can be added to a combobox if it is not built,
 * and no columns can be added anymore to the combobox once it is built. @n
 * Items can be added to the combobox with etk_combobox_item_prepend() and etk_combobox_item_append().
 * The parameters of those functions depend on the type of widget of the column. @n @n
 * Here is a simple example, where we create a combobox with three columns (one for the icon, one for the label and
 * one with a checkbox), and then, we add two items to it:
 * @code
 * Etk_Combobox *combobox;
 *
 * combobox = ETK_COMBOBOX(etk_combobox_new());
 * etk_combobox_column_add(combobox, ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
 * etk_combobox_column_add(combobox, ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_EXPAND, 0.0);
 * etk_combobox_column_add(combobox, ETK_COMBOBOX_OTHER, 32, ETK_COMBOBOX_NONE, 1.0);
 * etk_combobox_build(combobox);
 *
 * etk_combobox_item_append(combobox, image1, "item 1", checkbox1);
 * etk_combobox_item_append(combobox, image2, "item 2", checkbox1);
 * @endcode @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Combobox
 *
 * \par Signals:
 * @signal_name "item-activated": Emitted when an item is activated
 * @signal_cb Etk_Bool callback(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data)
 * @signal_arg combobox: the combobox whose item has been activated
 * @signal_arg item: the activated item
 * @signal_data
 * \par
 * @signal_name "active-item-changed": Emitted when the active item is changed (when the user selects another items)
 * @signal_cb Etk_Bool callback(Etk_Combobox *combobox, void *data)
 * @signal_arg combobox: the combobox whose active item has been changed
 * @signal_data
 *
 * \par Properties:
 * @prop_name "items-height": The height of an item of the combobox (should be > 0)
 * @prop_type Integer
 * @prop_rw
 * @prop_val 24
 * \par
 * @prop_name "active-item": The active item of the combobox (the item displayed in the combobox button)
 * @prop_type Pointer (Etk_Combobox_Item *)
 * @prop_rw
 * @prop_val NULL
 */
