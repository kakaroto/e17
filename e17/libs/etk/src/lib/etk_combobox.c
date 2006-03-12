/** @file etk_combobox_bar.c */
#include "etk_combobox.h"
#include <stdlib.h>
#include "etk_menu_window.h"
#include "etk_toggle_button.h"
#include "etk_label.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Combobox
* @{
 */

static void _etk_combobox_constructor(Etk_Combobox *combobox);
static void _etk_combobox_destructor(Etk_Combobox *combobox);
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
static void _etk_combobox_button_toggled_cb(Etk_Object *object, void *data);
static void _etk_combobox_window_popped_down_cb(Etk_Object *object, void *data);
static void _etk_combobox_item_enter_cb(Etk_Object *object, void *data);
static void _etk_combobox_item_leave_cb(Etk_Object *object, void *data);

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
      combobox_type = etk_type_new("Etk_Combobox", ETK_WIDGET_TYPE, sizeof(Etk_Combobox),
         ETK_CONSTRUCTOR(_etk_combobox_constructor), ETK_DESTRUCTOR(_etk_combobox_destructor));
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
   {
      combobox_item_type = etk_type_new("Etk_Combobox_Item", ETK_WIDGET_TYPE, sizeof(Etk_Combobox_Item),
         NULL, ETK_DESTRUCTOR(_etk_combobox_item_destructor));
   }

   return combobox_item_type;
}

/**
 * @brief Creates a new combobox
 * @return Returns the new combobox widget
 */
Etk_Widget *etk_combobox_new()
{
   return etk_widget_new(ETK_COMBOBOX_TYPE, "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new combobox, adds a unique column that can contain a label, and build the combobox
 * @return Returns the new combobox widget
 */
Etk_Widget *etk_combobox_new_default()
{
   Etk_Widget *combobox;
   
   combobox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_LABEL, 100, ETK_FALSE);
   etk_combobox_build(ETK_COMBOBOX(combobox));
   
   return combobox;
}

/**
 * @brief Adds a column to the combobox. The combobox shouldn't be already be built
 * @param combobox a combobox
 * @param col_type the type of widget that will be packed in the column
 * @param size TODO
 * @param expand TODO
 */
void etk_combobox_column_add(Etk_Combobox *combobox, Etk_Combobox_Column_Type col_type, int size, Etk_Bool expand)
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
   combobox->num_cols++;
}

/**
 * @brief Builds the combobox: to create a combobox with items, first create the columns, then build it, @n
 * and then start to add the items. Items can't be added if the combobox is not built
 * @param combobox the combobox to build
 */
void etk_combobox_build(Etk_Combobox *combobox)
{
   if (!combobox)
      return;
   
   combobox->active_item_widget = etk_widget_new(ETK_WIDGET_TYPE, "pass_events", ETK_TRUE, "visible", ETK_TRUE, NULL);
   combobox->active_item_widget->size_request = _etk_combobox_active_item_size_request;
   combobox->active_item_widget->size_allocate = _etk_combobox_active_item_size_allocate;
   etk_object_data_set(ETK_OBJECT(combobox->active_item_widget), "_Etk_Combobox_Window::Combobox", combobox);
   etk_container_add(ETK_CONTAINER(combobox->button), combobox->active_item_widget);
   
   combobox->built = ETK_TRUE;
}

/**
 * @brief Appends a new item to the combobox
 * @param combobox a combobox
 * @param data the data associated to the item
 * @param ... TODO
 * @return Returns the new item
 * @note Unlike other widgets, the new item will be automatically shown, so you won't have to call etk_widget_show() on it. 
 */
Etk_Combobox_Item *etk_combobox_item_append(Etk_Combobox *combobox, void *data, ...)
{
   Etk_Combobox_Item *item;
   va_list args;
   int i;
   
   if (!combobox)
      return NULL;
   if (!combobox->built)
   {
      ETK_WARNING("The combobox should be built when you add a new item to it");
      return NULL;
   }
   
   item = ETK_COMBOBOX_ITEM(etk_widget_new(ETK_COMBOBOX_ITEM_TYPE, "theme_group", "combobox_item", "visible", ETK_TRUE, NULL));
   item->combobox = combobox;
   item->data = data;
   item->widgets = malloc(sizeof(Etk_Widget *) * combobox->num_cols);
   ETK_WIDGET(item)->size_allocate = _etk_combobox_item_size_allocate;
   
   va_start(args, data);
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
      etk_widget_show(item->widgets[i]);
      /* TODO: repeat/pass events?? */
   }
   va_end(args);
   
   etk_signal_connect("enter", ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_item_enter_cb), NULL);
   etk_signal_connect("leave", ETK_OBJECT(item), ETK_CALLBACK(_etk_combobox_item_leave_cb), NULL);
   
   etk_widget_parent_set(ETK_WIDGET(item), ETK_WIDGET(combobox->window));
   combobox->items = evas_list_append(combobox->items, item);
   
   return item;
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
   
   combobox->button = etk_widget_new(ETK_TOGGLE_BUTTON_TYPE, "theme_group", "combobox", "visible", ETK_TRUE,
      "repeat_events", ETK_TRUE, "visibility_locked", ETK_TRUE, NULL);
   etk_widget_parent_set(combobox->button, ETK_WIDGET(combobox));
   etk_signal_connect("toggled", ETK_OBJECT(combobox->button), ETK_CALLBACK(_etk_combobox_button_toggled_cb), combobox);
   
   combobox->window = ETK_MENU_WINDOW(etk_widget_new(ETK_MENU_WINDOW_TYPE, "theme_group", "combobox_window", NULL));
   etk_object_data_set(ETK_OBJECT(combobox->window), "_Etk_Combobox_Window::Combobox", combobox);
   etk_signal_connect("popped_down", ETK_OBJECT(combobox->window), ETK_CALLBACK(_etk_combobox_window_popped_down_cb), combobox);
   ETK_WIDGET(combobox->window)->size_request = _etk_combobox_window_size_request;
   ETK_WIDGET(combobox->window)->size_allocate = _etk_combobox_window_size_allocate;
   
   combobox->active_item_widget = NULL;
   combobox->num_cols = 0;
   combobox->cols = NULL;
   combobox->items = NULL;
   combobox->item_height = 24;
   combobox->built = ETK_FALSE;
   
   ETK_WIDGET(combobox)->focus = _etk_combobox_focus_handler;
   ETK_WIDGET(combobox)->unfocus = _etk_combobox_unfocus_handler;
   ETK_WIDGET(combobox)->size_request = _etk_combobox_size_request;
   ETK_WIDGET(combobox)->size_allocate = _etk_combobox_size_allocate;
}

/* Destroys the combobox */
static void _etk_combobox_destructor(Etk_Combobox *combobox)
{
   int i;
   
   if (!combobox)
      return;
   
   for (i = 0; i < combobox->num_cols; i++)
      free(combobox->cols[i]);
   free(combobox->cols);
   
   etk_object_destroy(ETK_OBJECT(combobox->window));
   /* TODO */
}

/* Destroys the combobox item */
static void _etk_combobox_item_destructor(Etk_Combobox_Item *item)
{
   if (!item)
      return;
   
   /* TODO */
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
   Etk_Geometry child_geometry;
   int num_expandable_cols = 0;
   int expandable_width = 0;
   int total_width = 0;
   int i;
   
   if (!(item = ETK_COMBOBOX_ITEM(widget)) || !(combobox = item->combobox))
      return;
   
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (!combobox->cols[i]->expand)
      {
         num_expandable_cols++;
         expandable_width += combobox->cols[i]->size;
      }
      total_width += combobox->cols[i]->size;
   }
   
   child_geometry.x = geometry.x;
   child_geometry.y = geometry.y;
   child_geometry.h = geometry.h;
   for (i = 0; i < combobox->num_cols; i++)
   {
      if (num_expandable_cols == 0)
      {
         if (i != combobox->num_cols - 1)
            child_geometry.w = combobox->cols[i]->size;
         else
            child_geometry.w = geometry.w - (child_geometry.x - geometry.x);
      }
      else
         child_geometry.w = (combobox->cols[i]->size / expandable_width) * (geometry.w - (total_width - expandable_width));
      
      if (item->widgets[i])
         etk_widget_size_allocate(item->widgets[i], child_geometry);
      child_geometry.x += child_geometry.w;
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

/* TODO: doc */
static void _etk_combobox_active_item_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
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
      etk_menu_window_popup_at_xy(combobox->window, tx + bx, ty + by + bh);
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

/* Called when the mouse enters the item */
static void _etk_combobox_item_enter_cb(Etk_Object *object, void *data)
{
   etk_widget_theme_object_signal_emit(ETK_WIDGET(object), "select");
}

/* Called when the mouse leaves the item */
static void _etk_combobox_item_leave_cb(Etk_Object *object, void *data)
{
   etk_widget_theme_object_signal_emit(ETK_WIDGET(object), "unselect");
}

/**************************
 *
 * Private functions
 *
 **************************/


/** @} */
