/** @file etk_combobox_bar.c */
#include "etk_combobox.h"
#include <stdlib.h>
#include "etk_menu_window.h"
#include "etk_toggle_button.h"
#include "etk_utils.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Combobox
* @{
 */

static void _etk_combobox_constructor(Etk_Combobox *combobox);
static void _etk_combobox_destructor(Etk_Combobox *combobox);
static void _etk_combobox_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_combobox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_combobox_button_clicked_cb(Etk_Object *object, void *data);

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
      combobox_type = etk_type_new("Etk_Combobox", ETK_WIDGET_TYPE, sizeof(Etk_Combobox), ETK_CONSTRUCTOR(_etk_combobox_constructor), ETK_DESTRUCTOR(_etk_combobox_destructor));

   return combobox_type;
}

/**
 * @brief Creates a new combobox
 * @return Returns the new combobox widget
 */
Etk_Widget *etk_combobox_new()
{
   return etk_widget_new(ETK_COMBOBOX_TYPE, "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
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
   
   combobox->button = etk_widget_new(ETK_TOGGLE_BUTTON_TYPE, "theme_group", "combobox", "visible", ETK_TRUE, "visibility_locked", ETK_TRUE, NULL);
   etk_widget_parent_set(combobox->button, ETK_WIDGET(combobox));
   etk_signal_connect("clicked", ETK_OBJECT(combobox->button), ETK_CALLBACK(_etk_combobox_button_clicked_cb), combobox);
   
   
   combobox->window = ETK_MENU_WINDOW(etk_widget_new(ETK_MENU_WINDOW_TYPE, "theme_group", "combobox_window", NULL));
   /* TODO: connect */
   //etk_signal_connect("popped_up", ETK_OBJECT(menu->window), ETK_CALLBACK(_etk_menu_window_popped_up_cb), menu);
   //etk_signal_connect("popped_down", ETK_OBJECT(menu->window), ETK_CALLBACK(_etk_menu_window_popped_down_cb), menu);
   
   ETK_WIDGET(combobox)->size_request = _etk_combobox_size_request;
   ETK_WIDGET(combobox)->size_allocate = _etk_combobox_size_allocate;
}

/* Destroys the combobox */
static void _etk_combobox_destructor(Etk_Combobox *combobox)
{
   if (!combobox)
      return;
   etk_object_destroy(ETK_OBJECT(combobox->window));
}

/* Calculates the ideal size of the combobox */
static void _etk_combobox_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Combobox *combobox;
   
   if (!(combobox = ETK_COMBOBOX(widget)) || !size_requisition)
      return;
   etk_widget_size_request(combobox->button, size_requisition);
}

/* Resizes the combobox to the size allocation */
static void _etk_combobox_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Combobox *combobox;
   
   if (!(combobox = ETK_COMBOBOX(widget)))
      return;
   etk_widget_size_allocate(combobox->button, geometry);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the combobox button is clicked */
static void _etk_combobox_button_clicked_cb(Etk_Object *object, void *data)
{
   int tx, ty;
   int bx, by, bw, bh;
   Etk_Combobox *combobox;
   Etk_Toplevel_Widget *toplevel;
   
   if (!(combobox = ETK_COMBOBOX(data)) || !(toplevel = etk_widget_toplevel_parent_get(combobox->button)))
      return;
   
   etk_toplevel_widget_geometry_get(toplevel, &tx, &ty, NULL, NULL);
   etk_widget_geometry_get(combobox->button, &bx, &by, &bw, &bh);
   etk_menu_window_popup_at_xy(combobox->window, tx + bx, ty + by + bh);
}

/**************************
 *
 * Private functions
 *
 **************************/


/** @} */
