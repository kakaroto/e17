/** @file etk_toplevel_widget.c */
#include "etk_toplevel_widget.h"
#include <stdlib.h>
#include <Evas.h>
#include <Ecore_Data.h>
#include "etk_main.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_container.h"

/**
 * @addtogroup Etk_Toplevel_Widget
* @{
 */

enum _Etk_Toplevel_Widget_Property_Id
{
   ETK_TOPLEVEL_WIDGET_FOCUSED_WIDGET_PROPERTY
};

static void _etk_toplevel_widget_constructor(Etk_Toplevel_Widget *toplevel_widget);
static void _etk_toplevel_widget_destructor(Etk_Toplevel_Widget *toplevel_widget);
static void _etk_toplevel_widget_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toplevel_widget_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toplevel_widget_realize_cb(Etk_Object *object, void *data);
static Etk_Widget *_etk_toplevel_widget_next_to_focus_get(Etk_Widget *widget, Etk_Widget *after);
static Etk_Widget *_etk_toplevel_widget_prev_to_focus_get(Etk_Widget *widget, Etk_Widget *before);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Toplevel_Widget
 * @return Returns the type on an Etk_Toplevel_Widget
 */
Etk_Type *etk_toplevel_widget_type_get()
{
   static Etk_Type *toplevel_widget_type = NULL;

   if (!toplevel_widget_type)
   {
      toplevel_widget_type = etk_type_new("Etk_Toplevel_Widget", ETK_BIN_TYPE, sizeof(Etk_Toplevel_Widget), ETK_CONSTRUCTOR(_etk_toplevel_widget_constructor), ETK_DESTRUCTOR(_etk_toplevel_widget_destructor), NULL);
      
      etk_type_property_add(toplevel_widget_type, "focused_widget", ETK_TOPLEVEL_WIDGET_FOCUSED_WIDGET_PROPERTY, ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_pointer(NULL));
   
      toplevel_widget_type->property_set = _etk_toplevel_widget_property_set;
      toplevel_widget_type->property_get = _etk_toplevel_widget_property_get;
   }

   return toplevel_widget_type;
}

/**
 * @brief Gets the evas of the toplevel widget
 * @param toplevel_widget a toplevel widget
 * @return Returns the evas of the toplevel widget
 */
Evas *etk_toplevel_widget_evas_get(Etk_Toplevel_Widget *toplevel_widget)
{
   if (!toplevel_widget)
      return NULL;
   return toplevel_widget->evas;
}

/**
 * @brief Gets the size of the toplevel widget
 * @param toplevel_widget a toplevel widget
 * @param width the location where to set the width of the toplevel widget
 * @param height the location where to set the height of the toplevel widget
 */
void etk_toplevel_widget_size_get(Etk_Toplevel_Widget *toplevel_widget, int *width, int *height)
{
   if (!toplevel_widget)
      return;

   if (width)
      *width = toplevel_widget->width;
   if (height)
      *height = toplevel_widget->height;
}

/**
 * @brief Sets the focused widget of the toplevel widget. Only for widget implementations. If you want to focus a widget, use etk_widget_focus()
 * @param toplevel_widget a toplevel widget
 * @param widget the widget to set as focused
 */
void  etk_toplevel_widget_focused_widget_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Widget *widget)
{
   if (!toplevel_widget || (widget == toplevel_widget->focused_widget))
      return;

   toplevel_widget->focused_widget = widget;
   etk_object_notify(ETK_OBJECT(toplevel_widget), "focused_widget");
}

/**
 * @brief Gets the focused widget of the toplevel widget
 * @param toplevel_widget a toplevel widget
 * @return Returns the focused widget of the toplevel widget
 */
Etk_Widget *etk_toplevel_widget_focused_widget_get(Etk_Toplevel_Widget *toplevel_widget)
{
   if (!toplevel_widget)
      return NULL;
   return toplevel_widget->focused_widget;
}

/**
 * @brief Gets the next widget to focus. Mainly for widget implementations
 * @param toplevel_widget a toplevel widget
 * @return Returns the next widget to focus
 */
Etk_Widget *etk_toplevel_widget_focused_widget_next_get(Etk_Toplevel_Widget *toplevel_widget)
{
   Etk_Widget *focused_parent;

   if (!toplevel_widget)
      return NULL;

   if (toplevel_widget->focused_widget && (focused_parent = ETK_WIDGET(toplevel_widget->focused_widget->parent)))
      return _etk_toplevel_widget_next_to_focus_get(focused_parent, toplevel_widget->focused_widget);
   else
      return _etk_toplevel_widget_next_to_focus_get(ETK_WIDGET(toplevel_widget), NULL);
}

/**
 * @brief Gets the previous widget to focus.  Mainly for widget implementations
 * @param toplevel_widget a toplevel widget
 * @return Returns the previous widget to focus
 */
Etk_Widget *etk_toplevel_widget_focused_widget_prev_get(Etk_Toplevel_Widget *toplevel_widget)
{
   Etk_Widget *focused_parent;

   if (!toplevel_widget)
      return NULL;

   if (toplevel_widget->focused_widget && (focused_parent = ETK_WIDGET(toplevel_widget->focused_widget->parent)))
      return _etk_toplevel_widget_prev_to_focus_get(focused_parent, toplevel_widget->focused_widget);
   else
      return _etk_toplevel_widget_prev_to_focus_get(ETK_WIDGET(toplevel_widget), NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members of the toplevel widget */
static void _etk_toplevel_widget_constructor(Etk_Toplevel_Widget *toplevel_widget)
{
   if (!toplevel_widget)
      return;

   toplevel_widget->evas = NULL;
   toplevel_widget->width = 0;
   toplevel_widget->height = 0;
   toplevel_widget->focused_widget = NULL;
   ETK_WIDGET(toplevel_widget)->toplevel_parent = toplevel_widget;

   etk_signal_connect_after("realize", ETK_OBJECT(toplevel_widget), ETK_CALLBACK(_etk_toplevel_widget_realize_cb), NULL);

   etk_main_toplevel_widget_add(toplevel_widget);
}

/* Destroys the toplevel widget */
static void _etk_toplevel_widget_destructor(Etk_Toplevel_Widget *toplevel_widget)
{
   if (!toplevel_widget)
      return;

   etk_main_toplevel_widget_remove(toplevel_widget);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_toplevel_widget_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Toplevel_Widget *toplevel_widget;

   if (!(toplevel_widget = ETK_TOPLEVEL_WIDGET(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TOPLEVEL_WIDGET_FOCUSED_WIDGET_PROPERTY:
         etk_toplevel_widget_focused_widget_set(toplevel_widget, ETK_WIDGET(etk_property_value_pointer_get(value)));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_toplevel_widget_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Toplevel_Widget *toplevel_widget;

   if (!(toplevel_widget = ETK_TOPLEVEL_WIDGET(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TOPLEVEL_WIDGET_FOCUSED_WIDGET_PROPERTY:
         etk_property_value_pointer_set(value, ETK_OBJECT(toplevel_widget->focused_widget));
         break;
      default:
         break;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

static void _etk_toplevel_widget_realize_cb(Etk_Object *object, void *data)
{
   Etk_Toplevel_Widget *toplevel_widget;
   Etk_Widget *widget;

   if (!(toplevel_widget = ETK_TOPLEVEL_WIDGET(object)))
      return;

   widget = ETK_WIDGET(toplevel_widget);
   if (toplevel_widget->focused_widget)
   {
      if (toplevel_widget->focused_widget->smart_object)
         evas_object_focus_set(toplevel_widget->focused_widget->smart_object, 1);
   }
   else if (widget->smart_object)
      evas_object_focus_set(widget->smart_object, 1);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Gets recursively the next widget to focus */
static Etk_Widget *_etk_toplevel_widget_next_to_focus_get(Etk_Widget *widget, Etk_Widget *after)
{
   Ecore_DList *children;
   Etk_Widget *child;
   Etk_Widget *parent;

   if (!widget)
      return NULL;

   if (widget->focusable)
      return widget;

   if (!ETK_IS_CONTAINER(widget))
      return NULL;

   if (!(children = etk_container_children_get(ETK_CONTAINER(widget))))
      return NULL;

   if (after && ecore_dlist_goto(children, after))
      ecore_dlist_next(children);
   else
      ecore_dlist_goto_first(children);

   while ((child = ETK_WIDGET(ecore_dlist_next(children))))
   {
      Etk_Widget *next;

      if ((next = _etk_toplevel_widget_next_to_focus_get(child, NULL)))
         return next;
   }

   if ((parent = ETK_WIDGET(widget->parent)))
      return _etk_toplevel_widget_next_to_focus_get(parent, widget);
   else
      return NULL;
}

/* Gets recursively the previous widget to focus */
static Etk_Widget *_etk_toplevel_widget_prev_to_focus_get(Etk_Widget *widget, Etk_Widget *before)
{
   Ecore_DList *children;
   Etk_Widget *child;
   Etk_Widget *parent;

   if (!widget)
      return NULL;

   if (widget->focusable)
      return widget;

   if (!ETK_IS_CONTAINER(widget))
      return NULL;

   if (!(children = etk_container_children_get(ETK_CONTAINER(widget))))
      return NULL;

   if (before && ecore_dlist_goto(children, before))
      ecore_dlist_previous(children);
   else
      ecore_dlist_goto_last(children);

   while ((child = ETK_WIDGET(ecore_dlist_previous(children))))
   {
      Etk_Widget *next;

      if ((next = _etk_toplevel_widget_prev_to_focus_get(child, NULL)))
         return next;
   }

   if ((parent = ETK_WIDGET(widget->parent)))
      return _etk_toplevel_widget_prev_to_focus_get(parent, widget);
   else
      return NULL;
}

/** @} */
