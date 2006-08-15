/** @file etk_toolbar.c */

#include "etk_toolbar.h"
#include <stdlib.h>

#include "etk_widget.h"
#include "etk_signal.h"
#include "etk_button.h"
#include "etk_separator.h"

/**
 * @addtogroup Etk_Toolbar
 * @{
 */

enum Etk_Window_Property_Id
{
   ETK_TOOLBAR_ORIENTATION_PROPERTY,
   ETK_TOOLBAR_STYLE_PROPERTY     
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

      etk_type_property_add(toolbar_type, "orientation", ETK_TOOLBAR_ORIENTATION_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_TOOLBAR_HORIZ));
      etk_type_property_add(toolbar_type, "style", ETK_TOOLBAR_STYLE_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_TOOLBAR_BOTH_VERT));
      
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
   return etk_widget_new(ETK_TOOLBAR_TYPE, "theme_group", "toolbar", NULL);
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
   
   if (ETK_IS_BUTTON(widget))
   {
      Etk_Button_Style button_style;
           
      switch(toolbar->style)
      {
         case ETK_TOOLBAR_ICONS:
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
            button_style = ETK_BUTTON_ICON;
            break;      
      }      
      etk_button_style_set(ETK_BUTTON(widget), button_style);
   }
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
   
   if (ETK_IS_BUTTON(widget))
   {
      Etk_Button_Style button_style;
           
      switch(toolbar->style)
      {
         case ETK_TOOLBAR_ICONS:
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
            button_style = ETK_BUTTON_ICON;
            break;      
      }      
      etk_button_style_set(ETK_BUTTON(widget), button_style);
   }   
   etk_box_prepend(ETK_BOX(toolbar->box), widget, ETK_BOX_START, ETK_BOX_NONE, 0);
}

/**
 * @brief Sets the toolbar's orientation (horizontal or vertical)
 * @param toolbar the toolbar
 * @param orientation the orientation
 */
void etk_toolbar_orientation_set(Etk_Toolbar *toolbar, Etk_Toolbar_Orientation orientation)
{
   Evas_List *children;
   Evas_List *l;
   
   if (!toolbar || toolbar->orientation == orientation)
      return;
   
   children = etk_container_children_get(ETK_CONTAINER(toolbar->box));
      
   for (l = children; l; l = l->next)
      etk_container_remove(ETK_CONTAINER(toolbar->box), ETK_WIDGET(l->data));
   
   etk_object_destroy(ETK_OBJECT(toolbar->box));
   
   toolbar->orientation = orientation;
   if (toolbar->orientation == ETK_TOOLBAR_VERT)
      toolbar->box = etk_vbox_new(ETK_FALSE, 0);
   else
      toolbar->box = etk_hbox_new(ETK_FALSE, 0);           
   
   etk_widget_parent_set(toolbar->box, ETK_WIDGET(toolbar));
   etk_widget_visibility_locked_set(ETK_WIDGET(toolbar->box), ETK_TRUE);
   etk_widget_show(toolbar->box);
   
   for (l = children; l; l = l->next)
   {
      if (ETK_IS_VSEPARATOR(l->data))
      {
	 etk_object_destroy(ETK_OBJECT(l->data));
	 l->data = etk_hseparator_new();
      } 
      else if (ETK_IS_HSEPARATOR(l->data))
      {
	 etk_object_destroy(ETK_OBJECT(l->data));
	 l->data = etk_vseparator_new();
      }
      etk_box_append(ETK_BOX(toolbar->box), ETK_WIDGET(l->data), ETK_BOX_START, ETK_BOX_NONE, 0);
   }
}

/**
 * @brief Gets the toolbar's orientation (horizontal or vertical)
 * @param toolbar the toolbar
 * @return the orientation
 */
Etk_Toolbar_Orientation etk_toolbar_orientation_get(Etk_Toolbar *toolbar)
{
   if (!toolbar)
      return ETK_FALSE;
   
   return toolbar->orientation;
}

/**
 * @brief Sets the toolbar's style (icons, text, both, both vertical)
 * @param toolbar the toolbar
 * @param style the style
 */
void etk_toolbar_style_set(Etk_Toolbar *toolbar, Etk_Toolbar_Style style)
{
   Evas_List *children;
   Evas_List *l;
   Etk_Button_Style button_style;
   
   if (!toolbar || toolbar->style == style)
      return;
   
   children = etk_container_children_get(ETK_CONTAINER(toolbar->box));
      
   toolbar->style = style;
   switch(style)
   {
      case ETK_TOOLBAR_ICONS:
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
         button_style = ETK_BUTTON_ICON;
         break;      
   }
   for (l = children; l; l = l->next)
   {
      if (ETK_IS_BUTTON(l->data))
	 etk_button_style_set(ETK_BUTTON(l->data), button_style);
   }
}

/**
 * @brief Gets the toolbar's style (icons, text, both, both vertical)
 * @param toolbar the toolbar
 * @return the style
 */
Etk_Toolbar_Style etk_toolbar_style_get(Etk_Toolbar *toolbar)
{
   if (!toolbar)
      return ETK_TOOLBAR_DEFAULT;
   
   return toolbar->style;
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
      
   ETK_WIDGET(toolbar)->size_request = _etk_toolbar_size_request;
   ETK_WIDGET(toolbar)->size_allocate = _etk_toolbar_size_allocate;

   toolbar->box = etk_hbox_new(ETK_FALSE, 0);
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
         etk_toolbar_orientation_set(toolbar, etk_property_value_int_get(value));
         break;
      case ETK_TOOLBAR_STYLE_PROPERTY:
         etk_toolbar_style_set(toolbar, etk_property_value_int_get(value));
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
   Etk_Size size;
   
   if (!(toolbar = ETK_TOOLBAR(widget)))
      return;   
   
   etk_widget_size_request(toolbar->box, &size);
   geometry.w = ETK_MAX(geometry.w, size.w);
   geometry.h = ETK_MAX(geometry.h, size.h);
   etk_widget_size_allocate(ETK_WIDGET(toolbar->box), geometry);
}

/** @} */
