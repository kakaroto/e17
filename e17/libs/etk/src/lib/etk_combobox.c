/** @file etk_combobox_bar.c */
#include "etk_combobox.h"
#include <stdlib.h>
#include "etk_menu_window.h"
#include "etk_button.h"
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
   
   combobox->button = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", "combobox", "visible", ETK_TRUE, "visibility_locked", ETK_TRUE, NULL);
   etk_widget_parent_set(combobox->button, ETK_WIDGET(combobox));
   //combobox->window = etk_menu_window();
   ETK_WIDGET(combobox)->size_request = _etk_combobox_size_request;
   ETK_WIDGET(combobox)->size_allocate = _etk_combobox_size_allocate;
}

/* Destroys the combobox */
static void _etk_combobox_destructor(Etk_Combobox *combobox)
{
   if (!combobox)
      return;
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


/**************************
 *
 * Private functions
 *
 **************************/


/** @} */
