/** @file etk_toplevel_widget.c */
#include "etk_toplevel_widget.h"
#include <stdlib.h>
#include "etk_main.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Toplevel_Widget
 * @{
 */

enum _Etk_Toplevel_Widget_Property_Id
{
   ETK_TOPLEVEL_WIDGET_EVAS_PROPERTY,
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
      toplevel_widget_type = etk_type_new("Etk_Toplevel_Widget", ETK_BIN_TYPE, sizeof(Etk_Toplevel_Widget), ETK_CONSTRUCTOR(_etk_toplevel_widget_constructor), ETK_DESTRUCTOR(_etk_toplevel_widget_destructor));
      
      etk_type_property_add(toplevel_widget_type, "evas", ETK_TOPLEVEL_WIDGET_EVAS_PROPERTY, ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE, etk_property_value_pointer(NULL));
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
 * @brief Gets the position of the toplevel widget, relative to the top left corner of the evas where it is drawn
 * @param toplevel_widget a toplevel widget
 * @param x the location where to store the x evas position of the toplevel widget
 * @param y the location where to store the y evas position of the toplevel widget
 */
void etk_toplevel_widget_evas_position_get(Etk_Toplevel_Widget *toplevel_widget, int *x, int *y)
{
   if (!toplevel_widget || !toplevel_widget->evas_position_get)
   {
      if (x)  *x = 0;
      if (y)  *y = 0;
      return;
   }
   else
      toplevel_widget->evas_position_get(toplevel_widget, x, y);
}

/**
 * @brief Gets the position of the toplevel widget, relative to the top left corner of the screen
 * @param toplevel_widget a toplevel widget
 * @param x the location where to store the x screen position of the toplevel widget
 * @param y the location where to store the y screen position of the toplevel widget
 */
void etk_toplevel_widget_screen_position_get(Etk_Toplevel_Widget *toplevel_widget, int *x, int *y)
{
   if (!toplevel_widget || !toplevel_widget->screen_position_get)
   {
      if (x)  *x = 0;
      if (y)  *y = 0;
      return;
   }
   else
      toplevel_widget->screen_position_get(toplevel_widget, x, y);
}

/**
 * @brief Gets the size of the toplevel widget
 * @param toplevel_widget a toplevel widget
 * @param w the location where to store the width of the toplevel widget
 * @param h the location where to store the height of the toplevel widget
 */
void etk_toplevel_widget_size_get(Etk_Toplevel_Widget *toplevel_widget, int *w, int *h)
{
   if (!toplevel_widget || !toplevel_widget->size_get)
   {
      if (w)  *w = 0;
      if (h)  *h = 0;
      return;
   }
   else
      toplevel_widget->size_get(toplevel_widget, w, h);
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
 * @brief Gets the previous widget to focus.  Mainly for widget implementations
 * @param toplevel_widget a toplevel widget
 * @return Returns the previous widget to focus
 */
Etk_Widget *etk_toplevel_widget_focused_widget_prev_get(Etk_Toplevel_Widget *toplevel_widget)
{
   Etk_Widget *focused_parent;

   if (!toplevel_widget)
      return NULL;

   if (toplevel_widget->focused_widget && (focused_parent = toplevel_widget->focused_widget->parent))
      return _etk_toplevel_widget_prev_to_focus_get(focused_parent, toplevel_widget->focused_widget);
   else
      return _etk_toplevel_widget_prev_to_focus_get(ETK_WIDGET(toplevel_widget), NULL);
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

   if (toplevel_widget->focused_widget && (focused_parent = toplevel_widget->focused_widget->parent))
      return _etk_toplevel_widget_next_to_focus_get(focused_parent, toplevel_widget->focused_widget);
   else
      return _etk_toplevel_widget_next_to_focus_get(ETK_WIDGET(toplevel_widget), NULL);
}

/**
 * @brief Pushs a pointer type on the pointer stack. It will change the pointer shape
 * @param toplevel_widget a toplevel widget
 * @param pointer_type the type of pointer to push on the stack
 */ 
void etk_toplevel_widget_pointer_push(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
{
   Etk_Pointer_Type *prev_pointer_type;
   Etk_Pointer_Type *new_pointer_type;

   if (!toplevel_widget)
      return;

   new_pointer_type = malloc(sizeof(Etk_Pointer_Type));
   *new_pointer_type = pointer_type;
   prev_pointer_type = evas_list_data(evas_list_last(toplevel_widget->pointer_stack));
   toplevel_widget->pointer_stack = evas_list_append(toplevel_widget->pointer_stack, new_pointer_type);

   if (toplevel_widget->pointer_set && (!prev_pointer_type || (*prev_pointer_type != pointer_type)))
      toplevel_widget->pointer_set(toplevel_widget, pointer_type);
}

/**
 * @brief Pops a pointer type out of the pointer stack. It may change the pointer shape if the popped pointer type was on the top of the stack
 * @param toplevel_widget a toplevel widget
 * @param pointer_type the type of pointer to pop out of the stack. -1 to pop the last pointer on the stack
 */ 
void etk_toplevel_widget_pointer_pop(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
{
   Evas_List *l;
   Etk_Pointer_Type prev_pointer_type;
   Etk_Pointer_Type *prev_pointer_type_ptr;
   Etk_Pointer_Type *current_pointer_ptr;
   Etk_Pointer_Type *p;
   
   if (!toplevel_widget || !(prev_pointer_type_ptr = evas_list_data(evas_list_last(toplevel_widget->pointer_stack))))
      return;
   
   prev_pointer_type = *prev_pointer_type_ptr;
   
   if (pointer_type < 0)
   {
      toplevel_widget->pointer_stack = evas_list_remove_list(toplevel_widget->pointer_stack, evas_list_last(toplevel_widget->pointer_stack));
      free(prev_pointer_type_ptr);
   }
   else
   {
      for (l = evas_list_last(toplevel_widget->pointer_stack); l; l = l->prev)
      {
         p = l->data;
         if (*p == pointer_type)
         {
            toplevel_widget->pointer_stack = evas_list_remove_list(toplevel_widget->pointer_stack, l);
            free(p);
            break;
         }
      }
   }

   if (toplevel_widget->pointer_set)
   {
      if ((current_pointer_ptr = evas_list_data(evas_list_last(toplevel_widget->pointer_stack))))
      {
         if (*current_pointer_ptr != prev_pointer_type)
            toplevel_widget->pointer_set(toplevel_widget, *current_pointer_ptr);
      }
      else
         toplevel_widget->pointer_set(toplevel_widget, ETK_POINTER_DEFAULT);
   }
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
   toplevel_widget->focused_widget = NULL;
   toplevel_widget->pointer_stack = NULL;
   toplevel_widget->pointer_set = NULL;
   toplevel_widget->evas_position_get = NULL;
   toplevel_widget->screen_position_get = NULL;
   toplevel_widget->size_get = NULL;
   ETK_WIDGET(toplevel_widget)->toplevel_parent = toplevel_widget;

   etk_signal_connect("realize", ETK_OBJECT(toplevel_widget), ETK_CALLBACK(_etk_toplevel_widget_realize_cb), NULL);

   etk_main_toplevel_widget_add(toplevel_widget);
}

/* Destroys the toplevel widget */
static void _etk_toplevel_widget_destructor(Etk_Toplevel_Widget *toplevel_widget)
{
   Evas_List *l;

   if (!toplevel_widget)
      return;

   for (l = toplevel_widget->pointer_stack; l; l = l->next)
      free(l->data);
   toplevel_widget->pointer_stack = evas_list_free(toplevel_widget->pointer_stack);
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
      case ETK_TOPLEVEL_WIDGET_EVAS_PROPERTY:
         etk_property_value_pointer_set(value, toplevel_widget->evas);
         break;
      case ETK_TOPLEVEL_WIDGET_FOCUSED_WIDGET_PROPERTY:
         etk_property_value_pointer_set(value, toplevel_widget->focused_widget);
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

/* Called when the toplevel widget is realized */
static void _etk_toplevel_widget_realize_cb(Etk_Object *object, void *data)
{
   Etk_Toplevel_Widget *toplevel_widget;

   if (!(toplevel_widget = ETK_TOPLEVEL_WIDGET(object)))
      return;

   if (!toplevel_widget->focused_widget && ETK_WIDGET(toplevel_widget)->smart_object)
      evas_object_focus_set(ETK_WIDGET(toplevel_widget)->smart_object, 1);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Gets recursively the next widget to focus */
static Etk_Widget *_etk_toplevel_widget_next_to_focus_get(Etk_Widget *node, Etk_Widget *from)
{
   Evas_List *focus_order;
   Evas_List *l;

   if (!node)
      return NULL;
   if (node->focusable)
      return node;
   
   focus_order = node->use_focus_order ? node->focus_order : node->children;
   if (!focus_order)
      return _etk_toplevel_widget_next_to_focus_get(node->parent, node);

   if (from && (l = evas_list_find_list(focus_order, from)))
   {
      if (l->next)
         return _etk_toplevel_widget_next_to_focus_get(ETK_WIDGET(l->next->data), NULL);
      else
         return _etk_toplevel_widget_next_to_focus_get(node->parent, node);
   }
   else 
      return _etk_toplevel_widget_next_to_focus_get(ETK_WIDGET(focus_order->data), NULL);
}

/* Gets recursively the previous widget to focus */
static Etk_Widget *_etk_toplevel_widget_prev_to_focus_get(Etk_Widget *node, Etk_Widget *from)
{
   Evas_List *focus_order;
   Evas_List *l;

   if (!node)
      return NULL;
   if (node->focusable)
      return node;
   
   focus_order = node->use_focus_order ? node->focus_order : node->children;
   if (!focus_order)
      return _etk_toplevel_widget_prev_to_focus_get(node->parent, node);

   if (from && (l = evas_list_find_list(focus_order, from)))
   {
      if (l->prev)
         return _etk_toplevel_widget_prev_to_focus_get(ETK_WIDGET(l->prev->data), NULL);
      else
         return _etk_toplevel_widget_prev_to_focus_get(node->parent, node);
   }
   else 
      return _etk_toplevel_widget_prev_to_focus_get(ETK_WIDGET(evas_list_data(evas_list_last(focus_order))), NULL);
}

/** @} */
