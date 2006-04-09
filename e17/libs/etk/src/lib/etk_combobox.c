/** @file etk_combobox_bar.c */
#include "etk_combobox.h"
#include <stdlib.h>
#include <string.h>
#include "etk_container.h"
#include "etk_popup_window.h"
#include "etk_toggle_button.h"
#include "etk_label.h"
#include "etk_image.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Combobox
 * @{
 */

enum _Etk_Combobox_Signal_Id
{
   ETK_COMBOBOX_ACTIVE_ITEM_CHANGED_SIGNAL,
   ETK_COMBOBOX_NUM_SIGNALS
};

enum _Etk_Combobox_Property_Id
{
   ETK_COMBOBOX_ACTIVE_ITEM_PROPERTY
};

static void _etk_combobox_constructor(Etk_Combobox *combobox);
static void _etk_combobox_destructor(Etk_Combobox *combobox);
static void _etk_combobox_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_combobox_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_combobox_item_destructor(Etk_Combobox_Item *item);
static void _etk_combobox_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_combobox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_window_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_combobox_window_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_active_item_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_combobox_active_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_focus_handler(Etk_Widget *widget);
static void _etk_combobox_unfocus_handler(Etk_Widget *widget);
static void _etk_combobox_key_down_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_combobox_button_toggled_cb(Etk_Object *object, void *data);
static void _etk_combobox_window_popped_down_cb(Etk_Object *object, void *data);
static void _etk_combobox_window_key_down_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_combobox_item_enter_cb(Etk_Object *object, void *data);
static void _etk_combobox_item_leave_cb(Etk_Object *object, void *data);
static void _etk_combobox_item_mouse_up_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_combobox_selected_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item);
static Etk_Combobox_Item *_etk_combobox_item_new_valist(Etk_Combobox *combobox, va_list args);

static Etk_Signal *_etk_combobox_signals[ETK_COMBOBOX_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Combobox
 * @return Returns the type on an Etk_Combobox
 */
Etk_Type *etk_combobox_type_get()
{
   static Etk_Type *combobox_type = NULL;

   if (!combobox_type)
   {
      combobox_type = etk_type_new("Etk_Combobox", ETK_WIDGET_TYPE, sizeof(Etk_Combobox), ETK_CONSTRUCTOR(_etk_combobox_constructor), ETK_DESTRUCTOR(_etk_combobox_destructor));
      
      _etk_combobox_signals[ETK_COMBOBOX_ACTIVE_ITEM_CHANGED_SIGNAL] = etk_signal_new("active_item_changed", combobox_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      
      etk_type_property_add(combobox_type, "active_item", ETK_COMBOBOX_ACTIVE_ITEM_PROPERTY, ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_pointer(NULL));
      
      combobox_type->property_set = _etk_combobox_property_set;
      combobox_type->property_get = _etk_combobox_property_get;
   }

   return combobox_type;
}

/**
 * @brief Gets the type of an Etk_Combobox_Item
 * @return Returns the type on an Etk_Combobox_Item
 */
Etk_Type *etk_combobox_item_type_get()
{
   static Etk_Type *combobox_item_type = NULL;

   if (!combobox_item_type)
      combobox_item_type = etk_type_new("Etk_Combobox_Item", ETK_WIDGET_TYPE, sizeof(Etk_Combobox_Item), NULL, ETK_DESTRUCTOR(_etk_combobox_item_destructor));

   return combobox_item_type;
}

/**
 * @brief Creates a new combobox
 * @return Returns the new combobox widget
 */
Etk_Widget *etk_combobox_new()
{
   return etk_widget_new(ETK_COMBOBOX_TYPE, "theme_group", "combobox", "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new combobox, adds a unique column that can contain a label, and build the combobox
 * @return Returns the new combobox widget
 */
Etk_Widget *etk_combobox_new_default()
{
   Etk_Widget *combobox;
   
   combobox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_LABEL, 100, ETK_TRUE, ETK_FALSE, ETK_FALSE, 0.0, 0.5);
   etk_combobox_build(ETK_COMBOBOX(combobox));
   
   return combobox;
}

/**
 * @brief Adds a column to the combobox. The combobox shouldn't be already be built
 * @param combobox a combobox
 * @param col_type the type of widget that will be packed in the column
 * @param size the minimum width of the column. If @a expand is ETK_FALSE, the column will have this width
 * @param expand if @a expand is ETK_TRUE, the column will try to take as space as possible
 * @param hfill if @a hfill is ETK_TRUE, the child widget of the column will fill all the width of the column
 * @param vfill if @a vfill is ETK_TRUE, the child widget of the column will fill all the height of the row
 * @param xalign the horizontal alignment of the child widget of the column. If @a hfill is ETK_FALSE, it will have no effect
 * @param yalign the vertical alignment of the child widget of the column. If @a vfill is ETK_FALSE, it will have no effect
 */
void etk_combobox_column_add(Etk_Combobox *combobox, Etk_Combobox_Column_Type col_type, int size, Etk_Bool expand, Etk_Bool hfill, Etk_Bool vfill, float xalign, float yalign)
{
   if (!combobox)
      return;
   if (combobox->built)
   {
      ETK_WARNING("The combobox shouldn't be already built when you add a new column");
      return;
   }
   
   combobox->cols = realloc(combobox->cols, (combobox->num_cols + 1) * sizeof(Etk_Combobox_Column *));
   combobox->cols[combobox->num_cols] = malloc(sizeof(Etk_Combobox_Column));
   combobox->cols[combobox->num_cols]->type = col_type;
   combobox->cols[combobox->num_cols]->size = size;
   combobox->cols[combobox->num_cols]->expand = expand;
   combobox->cols[combobox->num_cols]->hfill = hfill;
   combobox->cols[combobox->num_cols]->vfill = vfill;
   combobox->cols[combobox->num_cols]->xalign = ETK_CLAMP(xalign, 0.0, 1.0);
   combobox->cols[combobox->num_cols]->yalign = ETK_CLAMP(yalign, 0.0, 1.0);
   combobox->num_cols++;
}

/**
 * @brief Builds the combobox: to create a combobox with items, first create the columns, then build it, @n
 * and then start to add the items. Items can't be added if the combobox is not built
 * @param combobox the combobox to build
 */
void etk_combobox_build(Etk_Combobox *combobox)
{
   int i, j;
   int num_children = 0;
   
   if (!combobox || combobox->built)
      return;
   
   combobox->active_item_widget = etk_widget_new(ETK_WIDGET_TYPE, "pass_mouse_events", ETK_TRUE,
      "visible", ETK_TRUE, "visibility_locked", ETK_TRUE, NULL);
   combobox->active_item_widget->size_request = _etk_combobox_active_item_size_request;
   combobox->active_item_widget->size_allocate = _etk_combobox_active_item_size_allocate;
   etk_object_data_set(ETK_OBJECT(combobox->active_item_widget), "_Etk_Combobox_Window::Combobox", combobox);
   etk_container_add(ETK_CONTAINER(combobox->button), combobox->active_item_widget);
   
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (combobox->cols[i]->type != ETK_COMBOBOX_OTHER)
         num_children++;
   }
   combobox->active_item_children = malloc(num_children * sizeof(Etk_Widget *));
   for (i = 0, j = 0; i < combobox->num_cols; i++)
   {
      if (combobox->cols[i]->type == ETK_COMBOBOX_OTHER)
         continue;
      
      switch (combobox->cols[i]->type)
      {
         case ETK_COMBOBOX_LABEL:
            combobox->active_item_children[j] = etk_label_new(NULL);
            break;
         case ETK_COMBOBOX_IMAGE:
            combobox->active_item_children[j] = etk_image_new();
            break;
         default:
            break;
      }
      etk_widget_visibility_locked_set(combobox->active_item_children[j], ETK_TRUE);
      etk_widget_show(combobox->active_item_children[j]);
      etk_widget_parent_set(combobox->active_item_children[j], combobox->active_item_widget);
      j++;
   }
   
   if (combobox->active_item)
      etk_combobox_active_item_set(combobox, combobox->active_item);
   
   combobox->built = ETK_TRUE;
}

/**
 * @brief Sets the active item of the combobox
 * @param combobox a combobox
 * @param item the item to set as active
 */
void etk_combobox_active_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item)
{
   if (!combobox || (item && item->combobox != combobox))
      return;
   
   if (combobox->built)
   {
      int i, j;
      
      for (i = 0, j = 0; i < combobox->num_cols; i++)
      {
         if (combobox->cols[i]->type == ETK_COMBOBOX_OTHER)
            continue;
         
         switch (combobox->cols[i]->type)
         {
            case ETK_COMBOBOX_LABEL:
               etk_label_set(ETK_LABEL(combobox->active_item_children[j]), item ? etk_label_get(ETK_LABEL(item->widgets[i])) : NULL);
               break;
            case ETK_COMBOBOX_IMAGE:
               if (item)
                  etk_image_copy(ETK_IMAGE(combobox->active_item_children[j]), ETK_IMAGE(item->widgets[i]));
               else
                  etk_image_set_from_file(ETK_IMAGE(combobox->active_item_children[j]), NULL);
               break;
            default:
               break;
         }
         j++;
      }
   }
   
   if (combobox->active_item != item)
   {
      combobox->active_item = item;
      etk_signal_emit(_etk_combobox_signals[ETK_COMBOBOX_ACTIVE_ITEM_CHANGED_SIGNAL], ETK_OBJECT(combobox), NULL);
      etk_object_notify(ETK_OBJECT(combobox), "active_item");
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
 * @brief Prepends a new item to the combobox
 * @param combobox a combobox
 * @param ... the different widgets to attach to the columns of the item: there must be as many arguments as the number of columns in the combobox @n
 * If the type of a column is ETK_COMBOBOX_LABEL, the argument must be a "const char *" @n
 * If the type of a column is ETK_COMBOBOX_IMAGE, the argument must be an "Etk_Image *" @n
 * If the type of a column is ETK_COMBOBOX_OTHER, the argument must be an "Etk_Widget *"
 * @return Returns the new item
 * @note Unlike other widgets, the new item will be automatically shown, so you won't have to call etk_widget_show() on it. 
 */
Etk_Combobox_Item *etk_combobox_item_prepend(Etk_Combobox *combobox, ...)
{
   Etk_Combobox_Item *item;
   va_list args;
   
   if (!combobox)
      return NULL;
   
   va_start(args, combobox);
   item = etk_combobox_item_prepend_valist(combobox, args);
   va_end(args);
   
   return item;
}

/**
 * @brief Prepends a new item to the combobox
 * @param combobox a combobox
 * @param args the different widgets to attach to the columns of the item. See: etk_combobox_item_prepend()'s ... argument for more informations
 * @return Returns the new item
 * @note Unlike other widgets, the new item will be automatically shown, so you won't have to call etk_widget_show() on it.
 * @see etk_combobox_item_prepend
 */
Etk_Combobox_Item *etk_combobox_item_prepend_valist(Etk_Combobox *combobox, va_list args)
{
   Etk_Combobox_Item *item;
   va_list args2;
   
   if (!combobox)
      return NULL;
   
   va_copy(args2, args);
   item = _etk_combobox_item_new_valist(combobox, args2);
   va_end(args2);
   
   if (item)
      combobox->items = evas_list_prepend(combobox->items, item);
   
   return item;
}

/**
 * @brief Appends a new item to the combobox
 * @param combobox a combobox
 * @param args the different widgets to attach to the columns of the item. See: etk_combobox_item_prepend()'s ... argument for more informations
 * @return Returns the new item
 * @note Unlike other widgets, the new item will be automatically shown, so you won't have to call etk_widget_show() on it.
 * @see etk_combobox_item_prepend
 */
Etk_Combobox_Item *etk_combobox_item_append(Etk_Combobox *combobox, ...)
{
   Etk_Combobox_Item *item;
   va_list args;
   
   if (!combobox)
      return NULL;
   
   va_start(args, combobox);
   item = etk_combobox_item_append_valist(combobox, args);
   va_end(args);
   
   return item;
}

/**
 * @brief Appends a new item to the combobox
 * @param combobox a combobox
 * @param args the different widgets to attach to the columns of the item. See: etk_combobox_item_prepend()'s ... argument for more informations
 * @return Returns the new item
 * @note Unlike other widgets, the new item will be automatically shown, so you won't have to call etk_widget_show() on it.
 * @see etk_combobox_item_prepend
 */
Etk_Combobox_Item *etk_combobox_item_append_valist(Etk_Combobox *combobox, va_list args)
{
   Etk_Combobox_Item *item;
   va_list args2;
   
   if (!combobox)
      return NULL;
   
   va_copy(args2, args);
   item = _etk_combobox_item_new_valist(combobox, args2);
   va_end(args2);
   
   if (item)
      combobox->items = evas_list_append(combobox->items, item);
   
   return item;
}

/**
 * @brief Returns an item from the combobox
 * @param combobox a combobox
 * @param index the number of the item to get
 */
Etk_Combobox_Item *etk_combobox_nth_item_get(Etk_Combobox *combobox, int index)
{
   Etk_Combobox_Item *item;

   if (!combobox)
      return NULL;

   /* Evas lists check the validity of the index */
   item = evas_list_nth(combobox->items, index);
   return item;
}

/**
 * @brief Removes an item from the combobox
 * @param combobox a combobox
 * @param item the item to remove
 * @see etk_combobox_nth_item_get
 */
void etk_combobox_item_remove(Etk_Combobox *combobox, Etk_Combobox_Item *item)
{
   if (!combobox || !item)
      return;

   combobox->items = evas_list_remove(combobox->items, item);
   if (item == combobox->active_item)
   {
     if (combobox->items)
       etk_combobox_active_item_set(combobox, combobox->items->data);
     else
       etk_combobox_active_item_set(combobox, NULL);
   }
   etk_object_destroy (ETK_OBJECT(item));
}

/**
 * @brief Removes all items from the combobox
 * @param combobox a combobox
 */
void etk_combobox_clear(Etk_Combobox *combobox)
{
   if (!combobox)
      return;

   while (combobox->items)
   {
      etk_object_destroy (combobox->items->data);
      combobox->items = evas_list_remove_list(combobox->items, combobox->items);
   }
   etk_combobox_active_item_set(combobox, NULL);
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
 * @brief Activates the combobox item
 * @param item the item to activate
 */
void etk_combobox_item_activate(Etk_Combobox_Item *item)
{
   if (!item || !item->combobox)
      return;
   etk_combobox_active_item_set(item->combobox, item);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_combobox_constructor(Etk_Combobox *combobox)
{
   if (!combobox)
      return;
   
   combobox->button = etk_widget_new(ETK_TOGGLE_BUTTON_TYPE, "theme_group", "button", "visible", ETK_TRUE,
      "repeat_mouse_events", ETK_TRUE, "visibility_locked", ETK_TRUE, NULL);
   etk_widget_parent_set(combobox->button, ETK_WIDGET(combobox));
   etk_signal_connect("toggled", ETK_OBJECT(combobox->button), ETK_CALLBACK(_etk_combobox_button_toggled_cb), combobox);
   
   combobox->window = ETK_POPUP_WINDOW(etk_widget_new(ETK_POPUP_WINDOW_TYPE, "theme_group", "window", NULL));
   etk_widget_theme_parent_set(ETK_WIDGET(combobox->window), ETK_WIDGET(combobox));
   etk_object_data_set(ETK_OBJECT(combobox->window), "_Etk_Combobox_Window::Combobox", combobox);
   etk_signal_connect("popped_down", ETK_OBJECT(combobox->window), ETK_CALLBACK(_etk_combobox_window_popped_down_cb), combobox);
   etk_signal_connect("key_down", ETK_OBJECT(combobox->window), ETK_CALLBACK(_etk_combobox_window_key_down_cb), combobox);
   ETK_WIDGET(combobox->window)->size_request = _etk_combobox_window_size_request;
   ETK_WIDGET(combobox->window)->size_allocate = _etk_combobox_window_size_allocate;
   
   combobox->selected_item = NULL;
   combobox->active_item = NULL;
   combobox->active_item_widget = NULL;
   combobox->active_item_children = NULL;
   
   combobox->num_cols = 0;
   combobox->cols = NULL;
   combobox->items = NULL;
   combobox->item_height = 24;
   combobox->built = ETK_FALSE;
   
   ETK_WIDGET(combobox)->focus = _etk_combobox_focus_handler;
   ETK_WIDGET(combobox)->unfocus = _etk_combobox_unfocus_handler;
   ETK_WIDGET(combobox)->size_request = _etk_combobox_size_request;
   ETK_WIDGET(combobox)->size_allocate = _etk_combobox_size_allocate;
   
   etk_signal_connect("key_down", ETK_OBJECT(combobox), ETK_CALLBACK(_etk_combobox_key_down_cb), NULL);
}

/* Destroys the combobox */
static void _etk_combobox_destructor(Etk_Combobox *combobox)
{
   int i;
   
   if (!combobox)
      return;
   
   combobox->selected_item = NULL;
   combobox->active_item = NULL;
   while (combobox->items)
      etk_object_destroy(ETK_OBJECT(combobox->items->data));
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
      case ETK_COMBOBOX_ACTIVE_ITEM_PROPERTY:
         etk_combobox_active_item_set(combobox, ETK_COMBOBOX_ITEM(etk_property_value_pointer_get(value)));
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
      case ETK_COMBOBOX_ACTIVE_ITEM_PROPERTY:
         etk_property_value_pointer_set(value, combobox->active_item);
         break;
      default:
         break;
   }
}

/* Destroys the combobox item */
static void _etk_combobox_item_destructor(Etk_Combobox_Item *item)
{
   if (!item)
      return;
   
   if (item->combobox)
   {
      if (item->combobox->selected_item == item || item->combobox->active_item == item)
      {
         Etk_Combobox_Item *next_item = NULL;
         Evas_List *l;
         
         if ((l = evas_list_find_list(item->combobox->items, item)))
         {
            if (l->next)
               next_item = ETK_COMBOBOX_ITEM(l->next->data);
            else if (l->prev)
               next_item = ETK_COMBOBOX_ITEM(l->prev->data);
         }
         
         if (item->combobox->selected_item == item)
            _etk_combobox_selected_item_set(item->combobox, next_item);
         if (item->combobox->active_item == item)
            etk_combobox_active_item_set(item->combobox, next_item);
      }
      item->combobox->items = evas_list_remove(item->combobox->items, item);
   }
   
   if (item->data && item->data_free_cb)
      item->data_free_cb(item->data);
   free(item->widgets);
}

/* Calculates the ideal size of the combobox */
static void _etk_combobox_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Combobox *combobox;
   
   if (!(combobox = ETK_COMBOBOX(widget)) || !size_requisition)
      return;
   etk_widget_size_request(combobox->button, size_requisition);
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
static void _etk_combobox_window_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Combobox *combobox;
   int i;
   
   if (!widget || !size_requisition)
      return;
   if (!(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Window::Combobox"))))
      return;
   
   size_requisition->w = 0;
   for (i = 0; i < combobox->num_cols; i++)
      size_requisition->w += combobox->cols[i]->size;
   
   size_requisition->h = evas_list_count(combobox->items) * combobox->item_height;
}

/* Resizes the combobox window to the allocated geometry */
static void _etk_combobox_window_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox *combobox;
   Evas_List *l;
   
   if (!widget || !(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Window::Combobox"))))
      return;
   
   geometry.h = combobox->item_height;
   for (l = combobox->items; l; l = l->next)
   {
      etk_widget_size_allocate(ETK_WIDGET(l->data), geometry);
      geometry.y += combobox->item_height;
   }
}

/* Resizes the combobox item to the allocated geometry */
static void _etk_combobox_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox_Item *item;
   Etk_Combobox *combobox;
   Etk_Geometry col_geometry;
   Etk_Geometry child_geometry;
   int num_expandable_cols = 0;
   int expandable_width = 0;
   int total_width = 0;
   int i;
   
   if (!(item = ETK_COMBOBOX_ITEM(widget)) || !(combobox = item->combobox))
      return;
   
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (combobox->cols[i]->expand)
      {
         num_expandable_cols++;
         expandable_width += combobox->cols[i]->size;
      }
      total_width += combobox->cols[i]->size;
   }
   
   col_geometry.x = geometry.x;
   col_geometry.y = geometry.y;
   col_geometry.h = geometry.h;
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (num_expandable_cols == 0)
      {
         if (i != combobox->num_cols - 1)
            col_geometry.w = combobox->cols[i]->size;
         else
            col_geometry.w = geometry.w - (col_geometry.x - geometry.x);
      }
      else
      {
         if (combobox->cols[i]->expand)
            col_geometry.w = (combobox->cols[i]->size / (float)expandable_width) * (geometry.w - (total_width - expandable_width));
         else
            col_geometry.w = combobox->cols[i]->size;
      }
      
      if (item->widgets[i])
      {
         memcpy(&child_geometry, &col_geometry, sizeof(Etk_Geometry));
         etk_container_child_space_fill(item->widgets[i], &child_geometry, combobox->cols[i]->hfill,
            combobox->cols[i]->vfill, combobox->cols[i]->xalign, combobox->cols[i]->yalign);
         etk_widget_size_allocate(item->widgets[i], child_geometry);
      }
      col_geometry.x += col_geometry.w;
   }
}

/* Calculates the ideal size of the active item of the combobox (the item in the combobox button */
static void _etk_combobox_active_item_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Combobox *combobox;
   int i;
   
   if (!widget || !size_requisition)
      return;
   if (!(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Window::Combobox"))))
      return;
   
   size_requisition->w = 0;
   size_requisition->h = 0;
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (combobox->cols[i]->type != ETK_COMBOBOX_OTHER)
         size_requisition->w += combobox->cols[i]->size;
   }
}

/* Resizes the active item of the combobox to the allocated geometry */
static void _etk_combobox_active_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox *combobox;
   Etk_Geometry col_geometry;
   Etk_Geometry child_geometry;
   int num_expandable_cols = 0;
   int num_children = 0;
   int expandable_width = 0;
   int total_width = 0;
   int i, j;
   
   if (!widget)
      return;
   if (!(combobox = ETK_COMBOBOX(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Combobox_Window::Combobox"))))
      return;
   
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (combobox->cols[i]->type == ETK_COMBOBOX_OTHER)
         continue;
      
      num_children++;
      if (combobox->cols[i]->expand)
      {
         num_expandable_cols++;
         expandable_width += combobox->cols[i]->size;
      }
      total_width += combobox->cols[i]->size;
   }
   
   col_geometry.x = geometry.x;
   col_geometry.y = geometry.y;
   col_geometry.h = geometry.h;
   for (i = 0, j = 0; i < combobox->num_cols; i++)
   {
      if (combobox->cols[i]->type == ETK_COMBOBOX_OTHER)
         continue;
      
      if (num_expandable_cols == 0)
      {
         if (j != num_children - 1)
            col_geometry.w = combobox->cols[i]->size;
         else
            col_geometry.w = geometry.w - (col_geometry.x - geometry.x);
      }
      else
      {
         if (combobox->cols[i]->expand)
            col_geometry.w = (combobox->cols[i]->size / (float)expandable_width) * (geometry.w - (total_width - expandable_width));
         else
            col_geometry.w = combobox->cols[i]->size;
      }
      
      if (combobox->active_item_children[j])
      {
         memcpy(&child_geometry, &col_geometry, sizeof(Etk_Geometry));
         etk_container_child_space_fill(combobox->active_item_children[j], &child_geometry,
            combobox->cols[i]->hfill,combobox->cols[i]->vfill, combobox->cols[i]->xalign, combobox->cols[i]->yalign);
         etk_widget_size_allocate(combobox->active_item_children[j], child_geometry);
      }
      
      col_geometry.x += col_geometry.w;
      j++;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Default handler for the "focus" handler */
static void _etk_combobox_focus_handler(Etk_Widget *widget)
{
   Etk_Combobox *combobox;
   
   if (!(combobox = ETK_COMBOBOX(widget)))
      return;
   etk_widget_theme_object_signal_emit(combobox->button, "focus");
}

/* Default handler for the "unfocus" handler */
static void _etk_combobox_unfocus_handler(Etk_Widget *widget)
{
   Etk_Combobox *combobox;
   
   if (!(combobox = ETK_COMBOBOX(widget)))
      return;
   etk_widget_theme_object_signal_emit(combobox->button, "unfocus");
}

/* Called when a key is pressed on the combobox */
static void _etk_combobox_key_down_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Combobox *combobox;
   Evas_Event_Key_Down *event;
   
   if (!(combobox = ETK_COMBOBOX(object)) || !(event = event_info))
      return;
   
   if (strcmp(event->keyname, "space") == 0)
   {
      etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(combobox->button), ETK_TRUE);
      etk_widget_key_event_propagation_stop();
   }
}

/* Called when the combobox button is toggled */
static void _etk_combobox_button_toggled_cb(Etk_Object *object, void *data)
{
   int tx, ty;
   int bx, by, bw, bh;
   Etk_Combobox *combobox;
   Etk_Toplevel_Widget *toplevel;
   
   if (!(combobox = ETK_COMBOBOX(data)) || !(toplevel = etk_widget_toplevel_parent_get(combobox->button)))
      return;
   
   if (etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(combobox->button)))
   {
      etk_toplevel_widget_geometry_get(toplevel, &tx, &ty, NULL, NULL);
      etk_widget_geometry_get(combobox->button, &bx, &by, &bw, &bh);
      etk_popup_window_popup_at_xy(combobox->window, tx + bx, ty + by + bh);
      etk_window_resize(ETK_WINDOW(combobox->window), bw, 0);
   }
}

/* Called when the combobox window is popped down */
static void _etk_combobox_window_popped_down_cb(Etk_Object *object, void *data)
{
   Etk_Combobox *combobox;
   
   if (!(combobox = ETK_COMBOBOX(data)))
      return;
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(combobox->button), ETK_FALSE);
}

/* Called when a key is pressed on the combobox window */
static void _etk_combobox_window_key_down_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Combobox *combobox;
   Evas_Event_Key_Down *event;
   Evas_List *l;
   
   if (!(combobox = ETK_COMBOBOX(data)) || !(event = event_info))
      return;
   
   if (strcmp(event->keyname, "Down") == 0)
   {
      if (!combobox->items)
         return;
      if (!combobox->selected_item || !(l = evas_list_find_list(combobox->items, combobox->selected_item)) || !l->next)
         _etk_combobox_selected_item_set(combobox, ETK_COMBOBOX_ITEM(combobox->items->data));
      else
         _etk_combobox_selected_item_set(combobox, ETK_COMBOBOX_ITEM(l->next->data));
   }
   else if (strcmp(event->keyname, "Up") == 0)
   {
      if (!combobox->items)
         return;
      if (!combobox->selected_item || !(l = evas_list_find_list(combobox->items, combobox->selected_item)) || !l->prev)
         _etk_combobox_selected_item_set(combobox, ETK_COMBOBOX_ITEM(evas_list_last(combobox->items)->data));
      else
         _etk_combobox_selected_item_set(combobox, ETK_COMBOBOX_ITEM(l->prev->data));
   }
   else if (strcmp(event->keyname, "Return") == 0 || strcmp(event->keyname, "space") == 0 || strcmp(event->keyname, "KP_Enter") == 0)
   {
      if (combobox->selected_item)
         etk_combobox_active_item_set(combobox, combobox->selected_item);
   }
   else if (strcmp(event->keyname, "Escape") == 0)
      etk_popup_window_popdown(combobox->window);
}

/* Called when the mouse enters the item */
static void _etk_combobox_item_enter_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Item *item;
   
   if (!(item = ETK_COMBOBOX_ITEM(object)))
      return;
   _etk_combobox_selected_item_set(item->combobox, item);
}

/* Called when the mouse leaves the item */
static void _etk_combobox_item_leave_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Item *item;
   
   if (!(item = ETK_COMBOBOX_ITEM(object)))
      return;
   
   if (item->combobox->selected_item == item)
      _etk_combobox_selected_item_set(item->combobox, NULL);
}

/* Called when the mouse releases the item */
static void _etk_combobox_item_mouse_up_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Combobox_Item *item;
   
   if (!(item = ETK_COMBOBOX_ITEM(object)))
      return;
   etk_combobox_item_activate(item);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates a new item for the combobox */
static Etk_Combobox_Item *_etk_combobox_item_new_valist(Etk_Combobox *combobox, va_list args)
{
   Etk_Combobox_Item *item;
   int i;
   
   if (!combobox)
      return NULL;
   if (!combobox->built)
   {
      ETK_WARNING("The combobox should be already built when you add a new item to it");
      return NULL;
   }
   
   item = ETK_COMBOBOX_ITEM(etk_widget_new(ETK_COMBOBOX_ITEM_TYPE, "theme_group", "combobox_item", "visible", ETK_TRUE, NULL));
   etk_widget_theme_parent_set(ETK_WIDGET(item), ETK_WIDGET(combobox));
   item->combobox = combobox;
   item->data = NULL;
   item->data_free_cb = NULL;
   item->widgets = malloc(sizeof(Etk_Widget *) * combobox->num_cols);
   ETK_WIDGET(item)->size_allocate = _etk_combobox_item_size_allocate;
   
   for (i = 0; i < combobox->num_cols; i++)
   {
      switch (combobox->cols[i]->type)
      {
         case ETK_COMBOBOX_LABEL:
            item->widgets[i] = etk_label_new(va_arg(args, char *));
            break;
         case ETK_COMBOBOX_IMAGE:
         case ETK_COMBOBOX_OTHER:
            item->widgets[i] = va_arg(args, Etk_Widget *);
         default:
            break;
      }
      etk_widget_parent_set(item->widgets[i], ETK_WIDGET(item));
      etk_widget_pass_mouse_events_set(item->widgets[i], ETK_TRUE);
      etk_widget_show(item->widgets[i]);
   }
   
   etk_signal_connect("enter", ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_item_enter_cb), NULL);
   etk_signal_connect("leave", ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_item_leave_cb), NULL);
   etk_signal_connect("mouse_up", ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_item_mouse_up_cb), NULL);
   
   etk_widget_parent_set(ETK_WIDGET(item), ETK_WIDGET(combobox->window));
   
   if (!combobox->active_item)
      etk_combobox_active_item_set(combobox, item);
   
   return item;
}

/* Sets the selected item of the the combobox */
static void _etk_combobox_selected_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item)
{
   if (!combobox)
      return;
   
   if (combobox->selected_item)
   {
      etk_widget_theme_object_signal_emit(ETK_WIDGET(combobox->selected_item), "unselect");
      combobox->selected_item = NULL;
   }
   if (item)
   {
      etk_widget_theme_object_signal_emit(ETK_WIDGET(item), "select");
      combobox->selected_item = item;
   }
}

/** @} */
