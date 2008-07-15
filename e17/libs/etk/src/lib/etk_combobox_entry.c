/** @file etk_combobox_entry.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_combobox_entry.h"

#include <stdlib.h>
#include <string.h>

#include "etk_box.h"
#include "etk_container.h"
#include "etk_entry.h"
#include "etk_event.h"
#include "etk_image.h"
#include "etk_label.h"
#include "etk_popup_window.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_toggle_button.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Combobox_Entry
 * @{
 */

#define MIN_ITEM_HEIGHT 12
#define DEFAULT_ITEM_HEIGHT 24

int ETK_COMBOBOX_ENTRY_ACTIVE_ITEM_CHANGED_SIGNAL;

enum Etk_Combobox_Entry_Property_Id
{
   ETK_COMBOBOX_ENTRY_ITEMS_HEIGHT_PROPERTY,
   ETK_COMBOBOX_ENTRY_ACTIVE_ITEM_PROPERTY,
};

static void _etk_combobox_entry_constructor(Etk_Combobox_Entry *combobox_entry);
static void _etk_combobox_entry_destructor(Etk_Combobox_Entry *combobox_entry);
static void _etk_combobox_entry_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_combobox_entry_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_combobox_entry_item_constructor(Etk_Combobox_Entry_Item *item);
static void _etk_combobox_entry_item_destructor(Etk_Combobox_Entry_Item *item);
static Etk_Bool _etk_combobox_entry_item_destroyed_cb(Etk_Object *object, void *data);
static void _etk_combobox_entry_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_combobox_entry_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_entry_window_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_combobox_entry_window_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_entry_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static Etk_Bool _etk_combobox_entry_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_entry_focused_cb(Etk_Widget *widget, void *data);
static Etk_Bool _etk_combobox_entry_unfocused_cb(Etk_Widget *widget, void *data);
static Etk_Bool _etk_combobox_entry_enabled_cb(Etk_Widget *widget, void *data);
static Etk_Bool _etk_combobox_entry_disabled_cb(Etk_Widget *widget, void *data);
static Etk_Bool _etk_combobox_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_combobox_entry_hbox_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_combobox_entry_button_toggled_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_entry_button_pressed_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_entry_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_combobox_entry_window_popped_down_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_entry_window_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_combobox_entry_item_entered_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_entry_item_left_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_combobox_entry_item_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data);
static Etk_Bool _etk_combobox_entry_entry_text_changed_cb(Etk_Object *object, void *data);

static void _etk_combobox_entry_selected_item_set(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *item);
static void _etk_combobox_entry_item_cells_render(Etk_Combobox_Entry *combobox_entry, Etk_Widget **cells, Etk_Geometry geometry, Etk_Bool ignore_other);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Combobox_Entry
 * @return Returns the type of an Etk_Combobox_Entry
 */
Etk_Type *etk_combobox_entry_type_get(void)
{
   static Etk_Type *combobox_entry_type = NULL;

   if (!combobox_entry_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_COMBOBOX_ENTRY_ACTIVE_ITEM_CHANGED_SIGNAL,
            "active-item-changed", etk_marshaller_VOID),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      combobox_entry_type = etk_type_new("Etk_Combobox_Entry", ETK_WIDGET_TYPE,
         sizeof(Etk_Combobox_Entry),
         ETK_CONSTRUCTOR(_etk_combobox_entry_constructor),
         ETK_DESTRUCTOR(_etk_combobox_entry_destructor), signals);

      etk_type_property_add(combobox_entry_type, "items-height", ETK_COMBOBOX_ENTRY_ITEMS_HEIGHT_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(DEFAULT_ITEM_HEIGHT));
      etk_type_property_add(combobox_entry_type, "active-item", ETK_COMBOBOX_ENTRY_ACTIVE_ITEM_PROPERTY,
            ETK_PROPERTY_OBJECT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_object(NULL));

      combobox_entry_type->property_set = _etk_combobox_entry_property_set;
      combobox_entry_type->property_get = _etk_combobox_entry_property_get;
   }

   return combobox_entry_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_Combobox_Entry_Item
 * @return Returns the type of an Etk_Combobox_Entry_Item
 */
Etk_Type *etk_combobox_entry_item_type_get(void)
{
   static Etk_Type *combobox_entry_item_type = NULL;

   if (!combobox_entry_item_type)
   {
      combobox_entry_item_type = etk_type_new("Etk_Combobox_Entry_Item",
         ETK_WIDGET_TYPE, sizeof(Etk_Combobox_Entry_Item),
         ETK_CONSTRUCTOR(_etk_combobox_entry_item_constructor),
         ETK_DESTRUCTOR(_etk_combobox_entry_item_destructor), NULL);
   }

   return combobox_entry_item_type;
}

/**
 * @brief Creates a new combobox_entry, with no column
 * @return Returns the new combobox_entry widget
 */
Etk_Widget *etk_combobox_entry_new(void)
{
   return etk_widget_new(ETK_COMBOBOX_ENTRY_TYPE, "theme-group", "combobox_entry",
         "focusable", ETK_TRUE, "focus-on-click", ETK_FALSE, NULL);
   // focus-on-click set to false, to allow subwidgets to be focused
}

/**
 * @brief Creates a new combobox_entry, made up of a unique column containing a label,
 * and already built (no need to call etk_combobox_entry_build())
 * @return Returns the new combobox_entry widget
 */
Etk_Widget *etk_combobox_entry_new_default(void)
{
   Etk_Widget *combobox_entry;

   combobox_entry = etk_combobox_entry_new();
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(combobox_entry), ETK_COMBOBOX_ENTRY_LABEL, 100, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(combobox_entry));

   return combobox_entry;
}

/**
 * @brief Gets the combobox_entry's entry widget
 * @param item a combobox_entry item
 * @return Returns the combobox_entry's entry widget
 */
Etk_Widget *etk_combobox_entry_entry_get(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry)
      return NULL;
   return combobox_entry->entry;
}

/**
 * @brief Set the height of the combobox_entry's items. The default height is 24, the minimum is 12
 * @param combobox_entry a combobox_entry
 * @param items_height the height that the combobox_entry's items should have
 */
void etk_combobox_entry_items_height_set(Etk_Combobox_Entry *combobox_entry, int items_height)
{
   if (!combobox_entry)
      return;

   items_height = ETK_MAX(MIN_ITEM_HEIGHT, items_height);
   if (items_height != combobox_entry->items_height)
   {
      combobox_entry->items_height = items_height;
      etk_widget_size_recalc_queue(ETK_WIDGET(combobox_entry->hbox));
      etk_widget_size_recalc_queue(ETK_WIDGET(combobox_entry->window));
      etk_object_notify(ETK_OBJECT(combobox_entry), "items-height");
   }
}

/**
 * @brief Gets the height of the combobox_entry's items
 * @param combobox_entry a combobox_entry
 * @return Returns the height of the combobox_entry's items
 */
int etk_combobox_entry_items_height_get(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry)
      return 0;
   return combobox_entry->items_height;
}

/**
 * @brief Adds a column to the combobox_entry. The combobox_entry should not be already be built
 * @param combobox_entry a combobox_entry
 * @param col_type the type of widget that will be packed in the column
 * @param width the width of the column. If @a expand is ETK_TRUE, the column may be bigger if it expands
 * @param fill_policy the fill-policy of the widget of the column, it indicates how it should fill the column
 * @param align the horizontal alignment of the widget of the column, from 0.0 (left) to 1.0 (right)
 */
void etk_combobox_entry_column_add(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Column_Type col_type, int width, Etk_Combobox_Entry_Fill_Policy fill_policy, float align)
{
   Etk_Combobox_Entry_Column *col;

   if (!combobox_entry)
      return;
   if (combobox_entry->built)
   {
      ETK_WARNING("Unable to add a new column to the combobox_entry because the combobox_entry has been built");
      return;
   }

   combobox_entry->cols = realloc(combobox_entry->cols, (combobox_entry->num_cols + 1) * sizeof(Etk_Combobox_Entry_Column *));
   combobox_entry->cols[combobox_entry->num_cols] = malloc(sizeof(Etk_Combobox_Entry_Column));
   col = combobox_entry->cols[combobox_entry->num_cols];
   col->type = col_type;
   col->width = width;
   col->fill_policy = fill_policy;
   col->align = ETK_CLAMP(align, 0.0, 1.0);
   combobox_entry->num_cols++;
}

/**
 * @brief Builds the combobox_entry: you have to build a combobox_entry after you have added all the columns to it and
 * before you start adding items. Items can not be added if the combobox_entry is not built
 * @param combobox_entry the combobox_entry to build
 */
void etk_combobox_entry_build(Etk_Combobox_Entry *combobox_entry)
{

   if (!combobox_entry || combobox_entry->built)
      return;

   /* FIXME: do we want to set an active item ourselves? Is this function still needed? */
#if 0
   if (combobox_entry->active_item)
      etk_combobox_entry_active_item_set(combobox_entry, combobox_entry->active_item);
#endif

   combobox_entry->built = ETK_TRUE;
}

/**
 * @brief Inserts a new item at the start of the combobox_entry
 *
 * @param combobox_entry a combobox_entry
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the
 * combobox_entry, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the
 *   argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the
 *   argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the
 *   argument must be an "Etk_Widget *"
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_item_prepend(Etk_Combobox_Entry *combobox_entry, ...)
{
   Etk_Combobox_Entry_Item *item;
   va_list args;

   va_start(args, combobox_entry);
   item = etk_combobox_entry_item_insert_valist(combobox_entry, NULL, args);
   va_end(args);

   return item;
}

/**
 * @brief Inserts a new empty item at the start of the combobox_entry
 *
 * @param combobox_entry a combobox_entry
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
inline Etk_Combobox_Entry_Item *etk_combobox_entry_item_prepend_empty(Etk_Combobox_Entry *combobox_entry)
{
   return etk_combobox_entry_item_insert_empty(combobox_entry, NULL);
}

/**
 * @brief Inserts a new item at the end of the combobox_entry
 *
 * @param combobox_entry a combobox_entry
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the
 * combobox_entry, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the
 *   argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the
 *   argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the
 *   argument must be an "Etk_Widget *"
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_item_append(Etk_Combobox_Entry *combobox_entry, ...)
{
   Etk_Combobox_Entry_Item *item;
   va_list args;

   va_start(args, combobox_entry);
   item = etk_combobox_entry_item_insert_valist(combobox_entry, combobox_entry->last_item, args);
   va_end(args);

   return item;
}

/**
 * @brief Inserts a new empty item at the end of the combobox_entry
 *
 * @param combobox_entry a combobox_entry
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
inline Etk_Combobox_Entry_Item *etk_combobox_entry_item_append_empty(Etk_Combobox_Entry *combobox_entry)
{
   return etk_combobox_entry_item_insert_empty(combobox_entry, combobox_entry->last_item);
}

/**
 * @brief Inserts a new item after an existing item of the combobox_entry
 *
 * @param combobox_entry a combobox_entry
 * @param after the item after which the new item should be inserted. If @a
 * after is NULL, the new item will be inserted at the start of the
 * combobox_entry
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the
 * combobox_entry, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the
 *   argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the
 *   argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the
 *   argument must be an "Etk_Widget *"
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_item_insert(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *after, ...)
{
   Etk_Combobox_Entry_Item *item;
   va_list args;

   va_start(args, after);
   item = etk_combobox_entry_item_insert_valist(combobox_entry, after, args);
   va_end(args);

   return item;
}

/**
 * @brief Inserts a new empty item after an existing item of the combobox_entry
 *
 * @param combobox_entry a combobox_entry
 * @param after the item after which the new item should be inserted. If @a
 * after is NULL, the new item will be inserted at the start of the
 * combobox_entry
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_item_insert_empty(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *after)
{
   Etk_Combobox_Entry_Item *item;

   if (!combobox_entry)
      return NULL;
   if (!combobox_entry->built)
   {
      ETK_WARNING("Unable to add a new item to the combobox_entry because "
                  "etk_combobox_entry_build() has not been called yet");
      return NULL;
   }
   if (after && combobox_entry != after->combobox_entry)
   {
      ETK_WARNING("Unable to add a new item after item %p because this item "
                  "does not belong to the combobox_entry in which the new "
                  "item should be added", after);
      return NULL;
   }

   item = ETK_COMBOBOX_ENTRY_ITEM(etk_widget_new(ETK_COMBOBOX_ENTRY_ITEM_TYPE,
      "theme-group", "item", "theme-parent", combobox_entry, "visible",
      ETK_TRUE, NULL));
   item->combobox_entry = combobox_entry;

   item->prev = after;
   item->next = after ? after->next : combobox_entry->first_item;
   if (after)
      after->next = item;
   else
      combobox_entry->first_item = item;
   if (combobox_entry->last_item == after)
      combobox_entry->last_item = item;

   /* Adds the corresponding widgets to the new item */
   item->widgets = calloc(combobox_entry->num_cols, sizeof(Etk_Widget *));
   ETK_WIDGET(item)->size_allocate = _etk_combobox_entry_item_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_ENTERED_SIGNAL, ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_entry_item_entered_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_LEFT_SIGNAL, ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_entry_item_left_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_entry_item_mouse_up_cb), NULL);

   etk_widget_parent_set(ETK_WIDGET(item), ETK_WIDGET(combobox_entry->window));

   /* FIXME: Do we need to set an active item? Might result in recursive loops */
#if 0
   if (!combobox_entry->active_item)
      etk_combobox_entry_active_item_set(combobox_entry, item);
#endif

   return item;
}

/**
 * @brief Inserts a new item after an existing item of the combobox_entry
 *
 * @param combobox_entry a combobox_entry
 * @param after the item after which the new item should be inserted. If @a
 * after is NULL, the new item will be inserted at the start of the
 * combobox_entry
 * @param args the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the
 * combobox_entry, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the
 *   argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the
 *   argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the
 *   argument must be an "Etk_Widget *"
 *
 * @return Returns the new item
 *
 * @note Unlike other widgets, the new item and its children will be
 * automatically shown, so you won't have to call etk_widget_show()
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_item_insert_valist(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *after, va_list args)
{
   Etk_Combobox_Entry_Item *item;
   va_list args2;

   item = etk_combobox_entry_item_insert_empty(combobox_entry, after);
   if (!item)
      return NULL;

   va_copy(args2, args);
   etk_combobox_entry_item_fields_set_valist(item, args2);
   va_end(args2);

   return item;
}

/**
 * @brief Removes an item from the combobox_entry. The item will be freed and should not be used anymore
 * @param item the item to remove from its combobox_entry
 * @note You can also call etk_object_destroy() to remove an item from a combobox_entry, it has the same effect
 */
void etk_combobox_entry_item_remove(Etk_Combobox_Entry_Item *item)
{
   etk_object_destroy(ETK_OBJECT(item));
}

/**
 * @brief Removes all the items of the combobox_entry. All the items will be freed and should not be used anymore
 * @param combobox_entry a combobox_entry
 */
void etk_combobox_entry_clear(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry)
      return;

   while (combobox_entry->first_item)
      etk_combobox_entry_item_remove(combobox_entry->first_item);
}

/**
 * @brief Sets the values of the cells of the combobox_entry item
 * @param item a combobox_entry item
 * @param ... the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox_entry, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the argument must be an "Etk_Widget *"
 * @note The new widgets of the item will be automatically shown
 */
void etk_combobox_entry_item_fields_set(Etk_Combobox_Entry_Item *item, ...)
{
   va_list args;

   va_start(args, item);
   etk_combobox_entry_item_fields_set_valist(item, args);
   va_end(args);
}

/**
 * @brief Sets the values of the cells of the combobox_entry item. The current widgets of item will be destroyed
 * @param item a combobox_entry item
 * @param args the different widgets to attach to the columns of the item:
 * there must be as many arguments as the number of columns in the combobox_entry, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the argument must be an "Etk_Widget *"
 * @note The new widgets of the item will be automatically shown
 */
void etk_combobox_entry_item_fields_set_valist(Etk_Combobox_Entry_Item *item, va_list args)
{
   Etk_Combobox_Entry *combobox_entry;
   int i;

   if (!item || !(combobox_entry = item->combobox_entry))
      return;

   for (i = 0; i < combobox_entry->num_cols; i++)
   {
      switch (combobox_entry->cols[i]->type)
      {
         if (item->widgets[i])
            etk_object_destroy(ETK_OBJECT(item->widgets[i]));

         case ETK_COMBOBOX_ENTRY_LABEL:
            item->widgets[i] = etk_label_new(va_arg(args, char *));
            etk_widget_pass_mouse_events_set(item->widgets[i], ETK_TRUE);
            break;
         case ETK_COMBOBOX_ENTRY_IMAGE:
            item->widgets[i] = ETK_WIDGET(va_arg(args, Etk_Widget *));
            etk_widget_pass_mouse_events_set(item->widgets[i], ETK_TRUE);
            break;
         case ETK_COMBOBOX_ENTRY_OTHER:
            item->widgets[i] = ETK_WIDGET(va_arg(args, Etk_Widget *));
            break;
         default:
            item->widgets[i] = NULL;
            break;
      }
      etk_widget_parent_set(item->widgets[i], ETK_WIDGET(item));
      etk_widget_show(item->widgets[i]);
   }

   /* FIXME: Do we need to set an active item? Might result in recursive loops */
#if 0
   if (combobox_entry->active_item == item)
      etk_combobox_entry_active_item_set(combobox_entry, item);
#endif
}

/**
 * @brief Sets the value of the cell of one column of the combobox_entry item. The current widgets of item will be destroyed
 * @param item a combobox_entry item
 * @param column the column to set the value of
 * @param data the value of the column to set:
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the argument must be a "const char *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the argument must be an "Etk_Image *" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the argument must be an "Etk_Widget *"
 * @note The new widgets of the item will be automatically shown
 */
void etk_combobox_entry_item_field_set(Etk_Combobox_Entry_Item *item, int column, void *value)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!item || !(combobox_entry = item->combobox_entry) || (column >= combobox_entry->num_cols))
      return;

   switch (combobox_entry->cols[column]->type)
   {
      if (item->widgets[column])
         etk_object_destroy(ETK_OBJECT(item->widgets[column]));

      case ETK_COMBOBOX_ENTRY_LABEL:
         item->widgets[column] = etk_label_new((char *)value);
         etk_widget_pass_mouse_events_set(item->widgets[column], ETK_TRUE);
         break;
      case ETK_COMBOBOX_ENTRY_IMAGE:
         item->widgets[column] = ETK_WIDGET((Etk_Widget *)value);
         etk_widget_pass_mouse_events_set(item->widgets[column], ETK_TRUE);
         break;
      case ETK_COMBOBOX_ENTRY_OTHER:
         item->widgets[column] = ETK_WIDGET((Etk_Widget *)value);
         break;
      default:
         item->widgets[column] = NULL;
         break;
   }
   etk_widget_parent_set(item->widgets[column], ETK_WIDGET(item));
   etk_widget_show(item->widgets[column]);
}


/**
 * @brief Gets the values of the cells of the combobox_entry item
 * @param item a combobox_entry item
 * @param ... the location where to store the different values of the cells of the item:
 * there must be as many arguments as the number of columns in the combobox_entry, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the argument must be a "const char **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the argument must be an "Etk_Image **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the argument must be an "Etk_Widget **"
 */
void etk_combobox_entry_item_fields_get(Etk_Combobox_Entry_Item *item, ...)
{
   va_list args;

   va_start(args, item);
   etk_combobox_entry_item_fields_get_valist(item, args);
   va_end(args);
}

/**
 * @brief Gets the values of the cells of the combobox_entry item
 * @param item a combobox_entry item
 * @param args the location where to store the different values of the cells of the item:
 * there must be as many arguments as the number of columns in the combobox_entry, one for each column. @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the argument must be a "const char **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the argument must be an "Etk_Image **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the argument must be an "Etk_Widget **"
 */
void etk_combobox_entry_item_fields_get_valist(Etk_Combobox_Entry_Item *item, va_list args)
{
   Etk_Combobox_Entry *combobox_entry;
   int i;

   if (!item || !(combobox_entry = item->combobox_entry))
      return;

   for (i = 0; i < combobox_entry->num_cols; i++)
   {
      switch (combobox_entry->cols[i]->type)
      {
         case ETK_COMBOBOX_ENTRY_LABEL:
         {
            const char **label;
            if ((label = va_arg(args, const char **)))
               *label = etk_label_get(ETK_LABEL(item->widgets[i]));
            break;
         }
         case ETK_COMBOBOX_ENTRY_IMAGE:
         case ETK_COMBOBOX_ENTRY_OTHER:
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
 * @brief Gets the value of the cell of a column of the combobox_entry item
 * @param item a combobox_entry item
 * @param column the column to get the value of
 * @return the value of the column as a void pointer that must be cast to the correct type:
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_LABEL, the argument must be a "const char **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_IMAGE, the argument must be an "Etk_Image **" @n
 * - If the type of the corresponding column is ETK_COMBOBOX_ENTRY_OTHER, the argument must be an "Etk_Widget **"
 */
void * etk_combobox_entry_item_field_get(Etk_Combobox_Entry_Item *item, int column)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!item || !(combobox_entry = item->combobox_entry) || (column >= combobox_entry->num_cols))
      return NULL;

   switch (combobox_entry->cols[column]->type)
   {
      case ETK_COMBOBOX_ENTRY_LABEL:
      {
         const char *label;
         label = etk_label_get(ETK_LABEL(item->widgets[column]));
	 return (void *)label;
         break;
      }
      case ETK_COMBOBOX_ENTRY_IMAGE:
      case ETK_COMBOBOX_ENTRY_OTHER:
      {
         Etk_Widget *widget;
         widget = item->widgets[column];
	 return (void *)widget;
         break;
      }
      default:
         return NULL;
         break;
   }
// FIXME: do we need this?
//   etk_widget_parent_set(item->widgets[column], ETK_WIDGET(item));
//   etk_widget_show(item->widgets[column]);
}


/**
 * @brief Sets the data associated to the combobox_entry item
 * @param item a combobox_entry item
 * @param data the data to associate to the combobox_entry item
 */
void etk_combobox_entry_item_data_set(Etk_Combobox_Entry_Item *item, void *data)
{
   if (!item)
      return;
   etk_combobox_entry_item_data_set_full(item, data, NULL);
}

/**
 * @brief Sets the data associated to the combobox_entry item
 * @param item a combobox_entry item
 * @param data the data to associate to the combobox_entry item
 * @param free_cb a function to call to free the data when the item is destroyed or when the data is changed
 */
void etk_combobox_entry_item_data_set_full(Etk_Combobox_Entry_Item *item, void *data, void (*free_cb)(void *data))
{
   if (!item)
      return;

   if (item->data_free_cb && item->data && (item->data != data))
      item->data_free_cb(item->data);
   item->data = data;
   item->data_free_cb = free_cb;
}

/**
 * @brief Gets the data associated to the combobox_entry item
 * @param item a combobox_entry item
 * @return Returns the data associated to the combobox_entry item
 */
void *etk_combobox_entry_item_data_get(Etk_Combobox_Entry_Item *item)
{
   if (!item)
      return NULL;
   return item->data;
}

/**
 * @brief Sets the active item of the combobox_entry
 * @param combobox_entry a combobox_entry
 * @param item the item to set as active
 */
void etk_combobox_entry_active_item_set(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *item)
{
   if (!combobox_entry)
      return;
   if ((item && item->combobox_entry != combobox_entry) || !combobox_entry->built)
   {
      ETK_WARNING("Unable to change the active-item of the combobox_entry. The specified item does not belong "
            "to the combobox_entry or the combobox_entry is not built yet.");
      return;
   }

   /* Changes the children of the active-item widget?? */

   if (combobox_entry->active_item != item)
   {
      combobox_entry->active_item = item;
      etk_signal_emit(ETK_COMBOBOX_ENTRY_ACTIVE_ITEM_CHANGED_SIGNAL, ETK_OBJECT(combobox_entry));
      etk_object_notify(ETK_OBJECT(combobox_entry), "active-item");
   }
}

/**
 * @brief Gets the active item of the combobox_entry
 * @param combobox_entry a combobox_entry
 * @return Returns the active item of combobox_entry (NULL if none)
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_active_item_get(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry)
      return NULL;
   return combobox_entry->active_item;
}

/**
 * @brief Gets the index of the current active item of the combobox_entry
 * @param combobox_entry a combobox_entry
 * @return Returns the index of the current active item, or -1 if there is no active item
 */
int etk_combobox_entry_active_item_num_get(Etk_Combobox_Entry *combobox_entry)
{
   Etk_Combobox_Entry_Item *item;
   int i;

   if (!combobox_entry || !combobox_entry->active_item)
      return -1;

   for (i = 0, item = combobox_entry->first_item; item; i++, item = item->next)
   {
      if (item == combobox_entry->active_item)
         return i;
   }
   return -1;
}

/**
 * @brief Gets the first item of a combobox_entry
 * @param combobox_entry a combobox_entry
 * @return Returns the first item of a combobox_entry
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_first_item_get(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry)
      return NULL;
   return combobox_entry->first_item;
}

/**
 * @brief Gets the last item of a combobox_entry
 * @param combobox_entry a combobox_entry
 * @return Returns the last item of a combobox_entry
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_last_item_get(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry)
      return NULL;
   return combobox_entry->last_item;
}

/**
 * @brief Gets the item previous to the given combobox_entry item
 * @param item a combobox_entry item
 * @return Returns the item previous to @a item, or NULL if the given item is the first one
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_item_prev_get(Etk_Combobox_Entry_Item *item)
{
   if (!item)
      return NULL;
   return item->prev;
}

/**
 * @brief Gets the item next to the given combobox_entry item
 * @param item a combobox_entry item
 * @return Returns the item next to @a item, or NULL if the given item is the last one
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_item_next_get(Etk_Combobox_Entry_Item *item)
{
   if (!item)
      return NULL;
   return item->next;
}

/**
 * @brief Gets the nth item of the combobox_entry
 * @param combobox_entry a combobox_entry
 * @param index the index of the item to get, starting from 0. If @a index is < 0 or if @a index is greater than
 * the current number of items of the combobox_entry, the function will return NULL
 * @return Returns the nth item of the combobox_entry, or NULL on failure
 */
Etk_Combobox_Entry_Item *etk_combobox_entry_nth_item_get(Etk_Combobox_Entry *combobox_entry, int index)
{
   Etk_Combobox_Entry_Item *item;
   int i;

   if (!combobox_entry || index < 0)
      return NULL;

   for (item = combobox_entry->first_item, i = 0; item && i < index; item = item->next)
      i++;
   return item;
}

/**
 * @brief Gets the combobox_entry containing the given item
 * @param item a combobox_entry item
 * @return Returns the combobox_entry containing the given item
 */
Etk_Combobox_Entry *etk_combobox_entry_item_combobox_entry_get(Etk_Combobox_Entry_Item *item)
{
   if (!item)
      return NULL;
   return item->combobox_entry;
}

/**
 * @brief Checks if the combobox_entry is open / popped up.
 * @param combobox_entry a combobox_entry
 * @return Returns true if the combobox is popped up, false otherwise.
 */
Etk_Bool etk_combobox_entry_is_popped_up(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry || !combobox_entry->button)
      return ETK_FALSE;
   return etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(combobox_entry->button));
}

/**
 * @brief Pops up the combobox's menu / window.
 * @param combobox_entry a combobox_entry
 * @return Returns true if the combobox is popped up, false otherwise.
 */
void etk_combobox_entry_pop_up(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry || !combobox_entry->built || !combobox_entry->button)
      return;
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(combobox_entry->button), ETK_TRUE);
}

/**
 * @brief Pops down the combobox's menu / window.
 * @param combobox_entry a combobox_entry
 * @return Returns true if the combobox is popped up, false otherwise.
 */
void etk_combobox_entry_pop_down(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry || !combobox_entry->built || !combobox_entry->button)
      return;
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(combobox_entry->button), ETK_FALSE);
}

/**
 * @brief Enable or disable autosearch on the combobox entry.
 * When a combo_Entry is set as searchable typing in the entry will show the popup
 * window with a filtered set of option.
 * @param combobox_entry a combobox_entry
 * @param enable ETK_TRUE = enable, ETK_FALSE = disable
 */
void etk_combobox_entry_autosearch_enable_set(Etk_Combobox_Entry *combobox_entry, int enable)
{
   if (!combobox_entry)
      return;
   
   if (enable)
      etk_signal_connect_by_code(ETK_ENTRY_TEXT_CHANGED_SIGNAL, ETK_OBJECT(combobox_entry->entry), ETK_CALLBACK(_etk_combobox_entry_entry_text_changed_cb), combobox_entry);
   else
      etk_signal_disconnect_by_code(ETK_ENTRY_TEXT_CHANGED_SIGNAL, ETK_OBJECT(combobox_entry->entry), ETK_CALLBACK(_etk_combobox_entry_entry_text_changed_cb), combobox_entry);      
   
   combobox_entry->autosearch_enable = enable ? ETK_TRUE : ETK_FALSE;
}

/**
 * @brief Set the colum number to use for the search, must be a string column.
 * @param combobox_entry a combobox_entry
 * @param col_num the string column to use for searching
 */
void etk_combobox_entry_autosearch_column_set(Etk_Combobox_Entry *combobox_entry, int col_num)
{
   if (!combobox_entry)
      return;
   
   combobox_entry->autosearch_column = col_num;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the combobox_entry */
static void _etk_combobox_entry_constructor(Etk_Combobox_Entry *combobox_entry)
{
   if (!combobox_entry)
      return;

   combobox_entry->hbox = etk_widget_new(ETK_HBOX_TYPE, "theme-group", "hbox", "theme-parent", combobox_entry,
         "visible", ETK_TRUE, "internal", ETK_TRUE, NULL);

   combobox_entry->button = etk_widget_new(ETK_TOGGLE_BUTTON_TYPE, "theme-group", "button", "theme-parent", combobox_entry,
         "visible", ETK_TRUE, "repeat-mouse-events", ETK_FALSE, "focusable", ETK_FALSE, "internal", ETK_TRUE, NULL);
   etk_object_data_set(ETK_OBJECT(combobox_entry->button), "_Etk_Combobox_Entry_Button::Combobox_Entry", combobox_entry);
   etk_widget_parent_set(combobox_entry->hbox, ETK_WIDGET(combobox_entry));

   /* FIXME: When we try to use this to custom theme the entry according
    * to the combobox_entry, the desired results arent achieved.
    * 1) we STILL get focus glow, although we removed it
    * 2) we lose the cursor
    */
   //combobox_entry->entry = etk_widget_new(ETK_ENTRY_TYPE, "focusable", ETK_TRUE, "focus-on-click", ETK_TRUE, "internal", ETK_TRUE, "theme-parent", combobox_entry, NULL);
   combobox_entry->entry = etk_entry_new();
   etk_widget_internal_set(combobox_entry->entry, ETK_TRUE);
   etk_widget_show(combobox_entry->entry);
   etk_box_append(ETK_BOX(combobox_entry->hbox), combobox_entry->entry, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_box_append(ETK_BOX(combobox_entry->hbox), combobox_entry->button, ETK_BOX_START, ETK_BOX_NONE, 0);

   combobox_entry->window = ETK_POPUP_WINDOW(etk_widget_new(ETK_POPUP_WINDOW_TYPE,
         "theme-group", "window", "theme-parent", combobox_entry, NULL));
   etk_object_data_set(ETK_OBJECT(combobox_entry->window), "_Etk_Combobox_Entry_Window::Combobox_Entry", combobox_entry);
   ETK_WIDGET(combobox_entry->window)->size_request = _etk_combobox_entry_window_size_request;
   ETK_WIDGET(combobox_entry->window)->size_allocate = _etk_combobox_entry_window_size_allocate;

   combobox_entry->popup_offset_x = 0;
   combobox_entry->popup_offset_y = 0;
   combobox_entry->popup_extra_w = 0;
   combobox_entry->num_cols = 0;
   combobox_entry->cols = NULL;
   combobox_entry->first_item = NULL;
   combobox_entry->last_item = NULL;
   combobox_entry->selected_item = NULL;
   combobox_entry->active_item = NULL;
   combobox_entry->items_height = DEFAULT_ITEM_HEIGHT;
   combobox_entry->built = ETK_FALSE;
   combobox_entry->autosearch_column = 0;
   combobox_entry->autosearch_enable = 0;

   ETK_WIDGET(combobox_entry)->size_request = _etk_combobox_entry_size_request;
   ETK_WIDGET(combobox_entry)->size_allocate = _etk_combobox_entry_size_allocate;

   etk_signal_connect_by_code(ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL, ETK_OBJECT(combobox_entry->button), ETK_CALLBACK(_etk_combobox_entry_button_toggled_cb), combobox_entry);
   etk_signal_connect_by_code(ETK_BUTTON_PRESSED_SIGNAL, ETK_OBJECT(combobox_entry->button), ETK_CALLBACK(_etk_combobox_entry_button_pressed_cb), combobox_entry);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(combobox_entry->hbox), ETK_CALLBACK(_etk_combobox_entry_hbox_mouse_up_cb), combobox_entry);
   etk_signal_connect_by_code(ETK_POPUP_WINDOW_POPPED_DOWN_SIGNAL, ETK_OBJECT(combobox_entry->window), ETK_CALLBACK(_etk_combobox_entry_window_popped_down_cb), combobox_entry);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(combobox_entry->window), ETK_CALLBACK(_etk_combobox_entry_window_key_down_cb), combobox_entry);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(combobox_entry->entry), ETK_CALLBACK(_etk_combobox_entry_entry_key_down_cb), combobox_entry);

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(combobox_entry), ETK_CALLBACK(_etk_combobox_entry_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_FOCUSED_SIGNAL, ETK_OBJECT(combobox_entry), ETK_CALLBACK(_etk_combobox_entry_focused_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNFOCUSED_SIGNAL, ETK_OBJECT(combobox_entry), ETK_CALLBACK(_etk_combobox_entry_unfocused_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_ENABLED_SIGNAL, ETK_OBJECT(combobox_entry), ETK_CALLBACK(_etk_combobox_entry_enabled_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_DISABLED_SIGNAL, ETK_OBJECT(combobox_entry), ETK_CALLBACK(_etk_combobox_entry_disabled_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(combobox_entry), ETK_CALLBACK(_etk_combobox_entry_key_down_cb), NULL);
}

/* Destroys the combobox_entry */
static void _etk_combobox_entry_destructor(Etk_Combobox_Entry *combobox_entry)
{
   int i;

   if (!combobox_entry)
      return;

   combobox_entry->selected_item = NULL;
   combobox_entry->active_item = NULL;
   etk_combobox_entry_clear(combobox_entry);

   for (i = 0; i < combobox_entry->num_cols; i++)
      free(combobox_entry->cols[i]);
   free(combobox_entry->cols);

   etk_object_destroy(ETK_OBJECT(combobox_entry->window));
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_combobox_entry_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_COMBOBOX_ENTRY_ITEMS_HEIGHT_PROPERTY:
         etk_combobox_entry_items_height_set(combobox_entry, etk_property_value_int_get(value));
         break;
      case ETK_COMBOBOX_ENTRY_ACTIVE_ITEM_PROPERTY:
         etk_combobox_entry_active_item_set(combobox_entry, ETK_COMBOBOX_ENTRY_ITEM(etk_property_value_object_get(value)));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_combobox_entry_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_COMBOBOX_ENTRY_ITEMS_HEIGHT_PROPERTY:
         etk_property_value_int_set(value, combobox_entry->items_height);
         break;
      case ETK_COMBOBOX_ENTRY_ACTIVE_ITEM_PROPERTY:
         etk_property_value_object_set(value, ETK_OBJECT(combobox_entry->active_item));
         break;
      default:
         break;
   }
}

/* Initializes the combobox_entry item */
static void _etk_combobox_entry_item_constructor(Etk_Combobox_Entry_Item *item)
{
   if (!item)
      return;

   item->combobox_entry = NULL;
   item->prev = NULL;
   item->next = NULL;
   item->widgets = NULL;
   item->data = NULL;
   item->data_free_cb = NULL;

   etk_signal_connect_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_entry_item_destroyed_cb), NULL);
}

/* Destroys the combobox_entry item */
static void _etk_combobox_entry_item_destructor(Etk_Combobox_Entry_Item *item)
{
   if (!item)
      return;

   if (item->data && item->data_free_cb)
      item->data_free_cb(item->data);
   free(item->widgets);
}

/* Called when a combobox_entry-item is destroyed. We use this function and not the destructor because
 * the destructor is not called immediately after the etk_object_destroy() call, but we want to remove the item
 * immediately from the combobox_entry (the destructor is still used the free the memory though) */
static Etk_Bool _etk_combobox_entry_item_destroyed_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Entry_Item *item;
   Etk_Combobox_Entry *combobox_entry;

   if (!(item = ETK_COMBOBOX_ENTRY_ITEM(object)) || !(combobox_entry = item->combobox_entry))
      return ETK_TRUE;

   if (item->combobox_entry->selected_item == item)
      _etk_combobox_entry_selected_item_set(item->combobox_entry, NULL);
   /* FIXME: Do we need to set an active item? Might result in recursive loops */
#if 0
   if (item->combobox_entry->active_item == item)
      etk_combobox_entry_active_item_set(item->combobox_entry, NULL);
#endif

   if (item->prev)
      item->prev->next = item->next;
   if (item->next)
      item->next->prev = item->prev;
   if (combobox_entry->first_item == item)
      combobox_entry->first_item = item->next;
   if (combobox_entry->last_item == item)
      combobox_entry->last_item = item->prev;

   return ETK_TRUE;
}

/* Calculates the ideal size of the combobox_entry */
static void _etk_combobox_entry_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(widget)) || !size)
      return;
   etk_widget_size_request(combobox_entry->hbox, size);
}

/* Resizes the combobox_entry to the allocated geometry */
static void _etk_combobox_entry_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(widget)))
      return;
   etk_widget_size_allocate(combobox_entry->hbox, geometry);
}

/* Calculates the ideal size of the combobox_entry window */
static void _etk_combobox_entry_window_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Combobox_Entry *combobox_entry;
   Etk_Combobox_Entry_Item *item;
   int i, num_visible_items;

   if (!widget || !size)
      return;
   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Entry_Window::Combobox_Entry"))))
      return;

   size->w = 0;
   for (i = 0; i < combobox_entry->num_cols; i++)
      size->w += combobox_entry->cols[i]->width;

   num_visible_items = 0;
   for (item = combobox_entry->first_item; item; item = item->next)
   {
      if (etk_widget_is_visible(ETK_WIDGET(item)))
         num_visible_items++;
   }

   size->h = num_visible_items * combobox_entry->items_height;
}

/* Resizes the combobox_entry window to the allocated geometry */
static void _etk_combobox_entry_window_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox_Entry *combobox_entry;
   Etk_Combobox_Entry_Item *item;

   if (!widget || !(combobox_entry = ETK_COMBOBOX_ENTRY(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Entry_Window::Combobox_Entry"))))
      return;

   geometry.h = combobox_entry->items_height;
   for (item = combobox_entry->first_item; item; item = item->next)
   {
      if (etk_widget_is_visible(ETK_WIDGET(item)))
      {
         etk_widget_size_allocate(ETK_WIDGET(item), geometry);
         geometry.y += combobox_entry->items_height;
      }
   }
}

/* Resizes the combobox_entry item to the allocated geometry */
static void _etk_combobox_entry_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox_Entry_Item *item;
   Etk_Combobox_Entry *combobox_entry;

   if (!(item = ETK_COMBOBOX_ENTRY_ITEM(widget)) || !(combobox_entry = item->combobox_entry))
      return;

   _etk_combobox_entry_item_cells_render(combobox_entry, item->widgets, geometry, ETK_FALSE);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the combobox_entry is realized */
static Etk_Bool _etk_combobox_entry_realized_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(object)))
      return ETK_TRUE;

   /* Read the data defined in the theme */
   if (etk_widget_theme_data_get(ETK_WIDGET(combobox_entry), "popup_offset_x", "%d", &combobox_entry->popup_offset_x) != 1)
      combobox_entry->popup_offset_x = 0;
   if (etk_widget_theme_data_get(ETK_WIDGET(combobox_entry), "popup_offset_y", "%d", &combobox_entry->popup_offset_y) != 1)
      combobox_entry->popup_offset_y = 0;
   if (etk_widget_theme_data_get(ETK_WIDGET(combobox_entry), "popup_extra_width", "%d", &combobox_entry->popup_extra_w) != 1)
      combobox_entry->popup_extra_w = 0;

   return ETK_TRUE;
}

/* Called when the combobox_entry is focused */
static Etk_Bool _etk_combobox_entry_focused_cb(Etk_Widget *widget, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(widget)))
      return ETK_TRUE;
   etk_widget_theme_signal_emit(combobox_entry->button, "etk,state,focused", ETK_FALSE);
   return ETK_TRUE;
}

/* Called when the combobox_entry is unfocused */
static Etk_Bool _etk_combobox_entry_unfocused_cb(Etk_Widget *widget, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(widget)))
      return ETK_TRUE;
   etk_widget_theme_signal_emit(combobox_entry->button, "etk,state,unfocused", ETK_FALSE);
   return ETK_TRUE;
}

/* Called when the combobox_entry is enabled */
static Etk_Bool _etk_combobox_entry_enabled_cb(Etk_Widget *widget, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(widget)))
      return ETK_TRUE;
   etk_widget_disabled_set(combobox_entry->button, ETK_FALSE);
   return ETK_TRUE;
}

/* Called when the combobox_entry is disabled */
static Etk_Bool _etk_combobox_entry_disabled_cb(Etk_Widget *widget, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(widget)))
      return ETK_TRUE;
   etk_widget_disabled_set(combobox_entry->button, ETK_TRUE);
   return ETK_TRUE;
}

/* Called when a key is pressed while the combobox_entry is focused*/
static Etk_Bool _etk_combobox_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(object)))
      return ETK_TRUE;

   if (strcmp(event->keyname, "space") == 0)
   {
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(combobox_entry->button), ETK_TRUE);
      return ETK_FALSE;
   }

   return ETK_TRUE;
}

/* Called when the hbox gets a click */
static Etk_Bool _etk_combobox_entry_hbox_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(data)))
      return ETK_TRUE;
   // dont pop up on whole widget, let toggle button (and on cursor up/down inside entry part) handle this
   //etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(combobox_entry->button));
   
   /* NOTE:
    * somehow this callback gets fired many times when popping up the popup window
    */
   return ETK_TRUE;
}

/* Called when the combobox_entry button is toggled */
static Etk_Bool _etk_combobox_entry_button_toggled_cb(Etk_Object *object, void *data)
{
   int ex, ey, sx, sy;
   int bx, by, bw, bh;
   Etk_Combobox_Entry *combobox_entry;
   Etk_Toplevel *toplevel;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(data)) || !(toplevel = etk_widget_toplevel_parent_get(combobox_entry->button)))
      return ETK_TRUE;

   if (etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(combobox_entry->button)))
   {
      etk_widget_geometry_get(combobox_entry->hbox, &bx, &by, &bw, &bh);
      etk_toplevel_evas_position_get(toplevel, &ex, &ey);
      etk_toplevel_screen_position_get(toplevel, &sx, &sy);
      etk_popup_window_popup_at_xy(combobox_entry->window,
            sx + (bx - ex) + combobox_entry->popup_offset_x,
            sy + (by - ey) + bh + combobox_entry->popup_offset_y);
      etk_window_resize(ETK_WINDOW(combobox_entry->window), bw + combobox_entry->popup_extra_w, 0);
   }

   return ETK_TRUE;
}

/* Called when the combobox_entry button is pressed */
static Etk_Bool _etk_combobox_entry_button_pressed_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Entry *combobox_entry;
   Etk_Combobox_Entry_Item *item;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(data)))
      return ETK_TRUE;

   if (!combobox_entry->autosearch_enable)
      return ETK_TRUE;
   
   for (item = combobox_entry->first_item; item; item = item->next)
      etk_widget_show(ETK_WIDGET(item));

   return ETK_TRUE;
}

/* Called when a key is pressed on the combobox_entry's entry widget */
static Etk_Bool _etk_combobox_entry_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(data)))
      return ETK_TRUE;

   if (strcmp(event->keyname, "Down") == 0)
   {
      if (!combobox_entry->first_item)
         return ETK_TRUE;

      if (!etk_popup_window_is_popped_up(ETK_POPUP_WINDOW(combobox_entry->window)))
	etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(combobox_entry->button));

      if (!combobox_entry->selected_item || !combobox_entry->selected_item->next)
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->first_item);
      else
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->selected_item->next);

      while (combobox_entry->selected_item &&
             !etk_widget_is_visible(ETK_WIDGET(combobox_entry->selected_item)))
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->selected_item->next);
   }
   else if (strcmp(event->keyname, "Up") == 0)
   {
      if (!combobox_entry->first_item)
         return ETK_TRUE;

      if (!etk_popup_window_is_popped_up(ETK_POPUP_WINDOW(combobox_entry->window)))
	etk_toggle_button_toggle(ETK_TOGGLE_BUTTON(combobox_entry->button));

      if (!combobox_entry->selected_item || !combobox_entry->selected_item->next)
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->last_item);
      else
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->selected_item->prev);

      while (combobox_entry->selected_item &&
             !etk_widget_is_visible(ETK_WIDGET(combobox_entry->selected_item)))
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->selected_item->prev);
   }
   else if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "KP_Enter") == 0)
   {
      if (combobox_entry->selected_item)
      {
         etk_combobox_entry_active_item_set(combobox_entry, combobox_entry->selected_item);
         if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "KP_Enter") == 0)
            etk_popup_window_popdown(combobox_entry->window);
      }
   }
   else if (strcmp(event->keyname, "Escape") == 0)
      etk_popup_window_popdown(combobox_entry->window);

   return ETK_TRUE;
}


/* Called when the combobox_entry window is popped down */
static Etk_Bool _etk_combobox_entry_window_popped_down_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(data)))
      return ETK_TRUE;
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(combobox_entry->button), ETK_FALSE);
   return ETK_TRUE;
}

/* Called when a key is pressed on the combobox_entry window */
static Etk_Bool _etk_combobox_entry_window_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Combobox_Entry *combobox_entry;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(data)))
      return ETK_TRUE;

   if (strcmp(event->keyname, "Down") == 0)
   {
      if (!combobox_entry->first_item)
         return ETK_TRUE;

      if (!combobox_entry->selected_item || !combobox_entry->selected_item->next)
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->first_item);
      else
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->selected_item->next);

      while (combobox_entry->selected_item &&
             !etk_widget_is_visible(ETK_WIDGET(combobox_entry->selected_item)))
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->selected_item->next);
   }
   else if (strcmp(event->keyname, "Up") == 0)
   {
      if (!combobox_entry->first_item)
         return ETK_TRUE;

      if (!combobox_entry->selected_item || !combobox_entry->selected_item->prev)
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->last_item);
      else
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->selected_item->prev);

      while (combobox_entry->selected_item &&
             !etk_widget_is_visible(ETK_WIDGET(combobox_entry->selected_item)))
         _etk_combobox_entry_selected_item_set(combobox_entry, combobox_entry->selected_item->prev);
   }
   else if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "space") == 0
         || strcmp(event->keyname, "KP_Enter") == 0)
   {
      if (combobox_entry->selected_item)
      {
         etk_combobox_entry_active_item_set(combobox_entry, combobox_entry->selected_item);
         if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "KP_Enter") == 0)
            etk_popup_window_popdown(combobox_entry->window);
      }
   }
   else if (strcmp(event->keyname, "Escape") == 0)
      etk_popup_window_popdown(combobox_entry->window);
   else
      etk_signal_emit(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(combobox_entry->entry), event, data);
   
   return ETK_TRUE;
}

/* Called when the mouse enters the item */
static Etk_Bool _etk_combobox_entry_item_entered_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Entry_Item *item;

   if (!(item = ETK_COMBOBOX_ENTRY_ITEM(object)))
      return ETK_TRUE;
   _etk_combobox_entry_selected_item_set(item->combobox_entry, item);
   return ETK_TRUE;
}

/* Called when the mouse leaves the item */
static Etk_Bool _etk_combobox_entry_item_left_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Entry_Item *item;

   if (!(item = ETK_COMBOBOX_ENTRY_ITEM(object)))
      return ETK_TRUE;

   if (item->combobox_entry->selected_item == item)
      _etk_combobox_entry_selected_item_set(item->combobox_entry, NULL);

   return ETK_TRUE;
}

/* Called when the mouse releases the item */
static Etk_Bool _etk_combobox_entry_item_mouse_up_cb(Etk_Object *object, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Combobox_Entry_Item *item;

   if (!(item = ETK_COMBOBOX_ENTRY_ITEM(object)))
      return ETK_TRUE;
   if (event->button != 1 && event->button != 3)
      return ETK_TRUE;

   etk_combobox_entry_active_item_set(item->combobox_entry, item);
   if (event->button == 1)
      etk_popup_window_popdown(item->combobox_entry->window);

   return ETK_TRUE;
}

/* Called when the text in the entry change (used for autosearch) */
static Etk_Bool _etk_combobox_entry_entry_text_changed_cb(Etk_Object *object, void *data)
{
   Etk_Entry *entry;
   Etk_Combobox_Entry *combobox_entry;
   Etk_Combobox_Entry_Item *item;
   const char *entry_text = NULL;

   if (!(entry = ETK_ENTRY(object)))
      return ETK_TRUE;

   if (!(combobox_entry = ETK_COMBOBOX_ENTRY(data)))
      return ETK_TRUE;

   if (!etk_combobox_entry_is_popped_up(combobox_entry))
      etk_combobox_entry_pop_up(combobox_entry);

   entry_text = etk_entry_text_get(entry);
   if (!entry_text)
     return ETK_TRUE;

   /* TODO FIXME We pop_down / pop_up the window to make it's size
      recalculate. We MUST find a better way*/
   etk_combobox_entry_pop_down(combobox_entry);
   
   for (item = combobox_entry->first_item; item; item = item->next)
   {
      const char* item_field;

      item_field = etk_combobox_entry_item_field_get(item, combobox_entry->autosearch_column);
      if (strcasestr(item_field, entry_text))
         etk_widget_show(ETK_WIDGET(item));
      else
         etk_widget_hide(ETK_WIDGET(item));
   }

   etk_combobox_entry_pop_up(combobox_entry);
   return ETK_TRUE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Sets the selected item of the the combobox_entry */
static void _etk_combobox_entry_selected_item_set(Etk_Combobox_Entry *combobox_entry, Etk_Combobox_Entry_Item *item)
{
   if (!combobox_entry)
      return;

   if (combobox_entry->selected_item)
   {
      etk_widget_theme_signal_emit(ETK_WIDGET(combobox_entry->selected_item), "etk,state,unselected", ETK_FALSE);
      combobox_entry->selected_item = NULL;
   }
   if (item)
   {
      etk_widget_theme_signal_emit(ETK_WIDGET(item), "etk,state,selected", ETK_FALSE);
      combobox_entry->selected_item = item;
   }
}

/* Renders the different cells of a combobox_entry item */
static void _etk_combobox_entry_item_cells_render(Etk_Combobox_Entry *combobox_entry, Etk_Widget **cells, Etk_Geometry geometry, Etk_Bool ignore_other)
{
   Etk_Geometry col_geometry;
   Etk_Geometry child_geometry;
   int num_expandable_cols = 0;
   int expandable_width = 0;
   int total_width = 0;
   int last_col = 0;
   int i;

   for (i = 0; i < combobox_entry->num_cols; i++)
   {
      if (ignore_other && (combobox_entry->cols[i]->type == ETK_COMBOBOX_ENTRY_OTHER))
         continue;

      if (combobox_entry->cols[i]->fill_policy & ETK_COMBOBOX_ENTRY_EXPAND)
      {
         num_expandable_cols++;
         expandable_width += combobox_entry->cols[i]->width;
      }
      total_width += combobox_entry->cols[i]->width;
      last_col = i;
   }

   col_geometry.x = geometry.x;
   col_geometry.y = geometry.y;
   col_geometry.h = geometry.h;
   for (i = 0; i < combobox_entry->num_cols; i++)
   {
      if (ignore_other && (combobox_entry->cols[i]->type == ETK_COMBOBOX_ENTRY_OTHER))
         continue;

      if (num_expandable_cols == 0)
      {
         if (i == last_col)
            col_geometry.w = geometry.w - (col_geometry.x - geometry.x);
         else
            col_geometry.w = combobox_entry->cols[i]->width;
      }
      else
      {
         if (combobox_entry->cols[i]->fill_policy & ETK_COMBOBOX_ENTRY_EXPAND)
         {
            col_geometry.w = (combobox_entry->cols[i]->width / (float)expandable_width) *
                  (geometry.w - (total_width - expandable_width));
         }
         else
            col_geometry.w = combobox_entry->cols[i]->width;
      }

      if (cells[i])
      {
         child_geometry = col_geometry;
         etk_container_child_space_fill(cells[i], &child_geometry,
               combobox_entry->cols[i]->fill_policy & ETK_COMBOBOX_ENTRY_FILL, ETK_TRUE, combobox_entry->cols[i]->align, 0.5);
         etk_widget_size_allocate(cells[i], child_geometry);
      }
      col_geometry.x += col_geometry.w;
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Combobox_Entry
 *
 * @image html widgets/combobox_entry.png
 * Once the combobox_entry is created, at least one column must be added to it with etk_combobox_entry_column_add().
 * Each column can contain a specific widget type. @n
 * Once all the column are created, the combobox_entry has to be built with etk_combobox_entry_build(),
 * in order to be able to add items to it. No item can be added to a combobox_entry if it is not built,
 * and no columns can be added anymore to the combobox_entry once it is built. @n
 * Items can be added to the combobox_entry with etk_combobox_entry_item_prepend() and etk_combobox_entry_item_append().
 * The parameters of those functions depend on the type of widget of the column. @n @n
 * Here is a simple example, where we create a combobox_entry with three columns (one for the icon, one for the label and
 * one with a checkbox), and then, we add two items to it:
 * @code
 * Etk_Combobox_Entry *combobox_entry;
 *
 * combobox_entry = ETK_COMBOBOX_ENTRY(etk_combobox_entry_new());
 * etk_combobox_entry_column_add(combobox_entry, ETK_COMBOBOX_ENTRY_IMAGE, 24, ETK_COMBOBOX_ENTRY_NONE, 0.0);
 * etk_combobox_entry_column_add(combobox_entry, ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
 * etk_combobox_entry_column_add(combobox_entry, ETK_COMBOBOX_ENTRY_OTHER, 32, ETK_COMBOBOX_ENTRY_NONE, 1.0);
 * etk_combobox_entry_build(combobox_entry);
 *
 * etk_combobox_entry_item_append(combobox_entry, image1, "item 1", checkbox1);
 * etk_combobox_entry_item_append(combobox_entry, image2, "item 2", checkbox1);
 * @endcode @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Combobox_Entry
 *
 * \par Signals:
 * @signal_name "active-item-changed": Emitted when the active item is changed (when the user selects another items)
 * @signal_cb Etk_Bool callback(Etk_Combobox_Entry *combobox_entry, void *data)
 * @signal_arg combobox_entry: the combobox_entry whose active item has been changed
 * @signal_data
 *
 * \par Properties:
 * @prop_name "items-height": The height of an item of the combobox_entry (should be > 0)
 * @prop_type Integer
 * @prop_rw
 * @prop_val 24
 * \par
 * @prop_name "active-item": The active item of the combobox_entry (the item displayed in the combobox_entry button)
 * @prop_type Pointer (Etk_Combobox_Entry_Item *)
 * @prop_rw
 * @prop_val NULL
 */
