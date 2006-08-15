/** @file etk_toolbar.c */

#include "etk_toolbar.h"
#include <stdlib.h>

#include "etk_widget.h"
#include "etk_signal.h"

/**
 * @addtogroup Etk_Toolbar
 * @{
 */

enum Etk_Window_Property_Id
{
   ETK_TOOLBAR_ORIENTATION_PROPERTY
};     

static void _etk_toolbar_constructor(Etk_Toolbar *toolbar);
static void _etk_toolbar_destructor(Etk_Toolbar *toolbar);
static void _etk_toolbar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toolbar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toolbar_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_toolbar_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Toolbar
 * @return Returns the type of an Etk_Toolbar
 */
Etk_Type *etk_toolbar_type_get()
{
   static Etk_Type *toolbar_type = NULL;

   if (!toolbar_type)
   {
      toolbar_type = etk_type_new("Etk_Toolbar", ETK_WIDGET_TYPE, sizeof(Etk_Toolbar),
         ETK_CONSTRUCTOR(_etk_toolbar_constructor), ETK_DESTRUCTOR(_etk_toolbar_destructor));

      etk_type_property_add(toolbar_type, "orientation", ETK_TOOLBAR_ORIENTATION_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_TOOLBAR_ORIENTATION_HORIZONTAL));
      
      toolbar_type->property_set = _etk_toolbar_property_set;
      toolbar_type->property_get = _etk_toolbar_property_get;
   }

   return toolbar_type;
}

/**
 * @brief Creates a new toolbar
 */
Etk_Widget *etk_toolbar_new()
{
   return etk_widget_new(ETK_TOOLBAR_TYPE, "theme_group", "toolbar","orientation", ETK_TOOLBAR_ORIENTATION_HORIZONTAL, NULL);
}

/**
 * @brief Appends an item to the start of the toolbar
 * @param toolbar the toolbar
 * @param widget the item to append
 */
void etk_toolbar_append(Etk_Toolbar *toolbar, Etk_Widget *widget)
{
   if (!toolbar || !widget)
      return;
   
   etk_box_append(ETK_BOX(toolbar->box), widget, ETK_BOX_START, ETK_BOX_NONE, 0);
}

/**
 * @brief Prepends an item to the end of the toolbar
 * @param toolbar the toolbar
 * @param widget the item to append
 */
void etk_toolbar_prepend(Etk_Toolbar *toolbar, Etk_Widget *widget)
{
   if (!toolbar || !widget)
      return;
   
   etk_box_prepend(ETK_BOX(toolbar->box), widget, ETK_BOX_START, ETK_BOX_NONE, 0);
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
   
   if (toolbar->orientation == ETK_TOOLBAR_ORIENTATION_VERTICAL)
      toolbar->box = etk_vbox_new(ETK_FALSE, 0);
   else
      toolbar->box = etk_hbox_new(ETK_FALSE, 0);
   
   ETK_WIDGET(toolbar)->size_request = _etk_toolbar_size_request;
   ETK_WIDGET(toolbar)->size_allocate = _etk_toolbar_size_allocate;
   
   etk_widget_parent_set(toolbar->box, ETK_WIDGET(toolbar));
   etk_widget_visibility_locked_set(ETK_WIDGET(toolbar->box), ETK_TRUE);
   etk_widget_show(toolbar->box);
}

/* Destroys the toolbar */
static void _etk_toolbar_destructor(Etk_Toolbar *toolbar)
{
   if (!toolbar)
     return;
   
   etk_object_destroy(ETK_OBJECT(toolbar->box));
   toolbar->box = NULL;
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
         //etk_window_title_set(window, etk_property_value_string_get(value));
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
         //etk_property_value_string_set(value, etk_window_title_get(window));
         break;
      default:
         break;
   }
}

/* Resizes the toolbar  */
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
   
   if (!(toolbar = ETK_TOOLBAR(widget)))
      return;   
      
   etk_widget_size_allocate(ETK_WIDGET(toolbar->box), geometry);
}

/** @} */
