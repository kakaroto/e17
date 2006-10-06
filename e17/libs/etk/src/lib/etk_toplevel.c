/** @file etk_toplevel.c */
#include "etk_toplevel.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Toplevel
 * @{
 */

enum _Etk_Toplevel_Property_Id
{
   ETK_TOPLEVEL_EVAS_PROPERTY,
   ETK_TOPLEVEL_FOCUSED_WIDGET_PROPERTY
};

static void _etk_toplevel_constructor(Etk_Toplevel *toplevel);
static void _etk_toplevel_destructor(Etk_Toplevel *toplevel);
static void _etk_toplevel_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toplevel_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toplevel_realize_cb(Etk_Object *object, void *data);
static Etk_Widget *_etk_toplevel_next_to_focus_get(Etk_Widget *widget, Etk_Widget *after);
static Etk_Widget *_etk_toplevel_prev_to_focus_get(Etk_Widget *widget, Etk_Widget *before);

static Evas_List *_etk_toplevel_widgets = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Toplevel
 * @return Returns the type on an Etk_Toplevel
 */
Etk_Type *etk_toplevel_type_get()
{
   static Etk_Type *toplevel_type = NULL;

   if (!toplevel_type)
   {
      toplevel_type = etk_type_new("Etk_Toplevel", ETK_BIN_TYPE, sizeof(Etk_Toplevel), ETK_CONSTRUCTOR(_etk_toplevel_constructor), ETK_DESTRUCTOR(_etk_toplevel_destructor));
      
      etk_type_property_add(toplevel_type, "evas", ETK_TOPLEVEL_EVAS_PROPERTY, ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE, etk_property_value_pointer(NULL));
      etk_type_property_add(toplevel_type, "focused_widget", ETK_TOPLEVEL_FOCUSED_WIDGET_PROPERTY, ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_pointer(NULL));
   
      toplevel_type->property_set = _etk_toplevel_property_set;
      toplevel_type->property_get = _etk_toplevel_property_get;
   }

   return toplevel_type;
}

/**
 * @brief Gets the evas of the toplevel widget
 * @param toplevel a toplevel widget
 * @return Returns the evas of the toplevel widget
 */
Evas *etk_toplevel_evas_get(Etk_Toplevel *toplevel)
{
   if (!toplevel)
      return NULL;
   return toplevel->evas;
}

/**
 * @brief Gets the position of the toplevel widget, relative to the top left corner of the evas where it is drawn
 * @param toplevel a toplevel widget
 * @param x the location where to store the x evas position of the toplevel widget
 * @param y the location where to store the y evas position of the toplevel widget
 */
void etk_toplevel_evas_position_get(Etk_Toplevel *toplevel, int *x, int *y)
{
   if (!toplevel || !toplevel->evas_position_get)
   {
      if (x)  *x = 0;
      if (y)  *y = 0;
      return;
   }
   else
      toplevel->evas_position_get(toplevel, x, y);
}

/**
 * @brief Gets the position of the toplevel widget, relative to the top left corner of the screen
 * @param toplevel a toplevel widget
 * @param x the location where to store the x screen position of the toplevel widget
 * @param y the location where to store the y screen position of the toplevel widget
 */
void etk_toplevel_screen_position_get(Etk_Toplevel *toplevel, int *x, int *y)
{
   if (!toplevel || !toplevel->screen_position_get)
   {
      if (x)  *x = 0;
      if (y)  *y = 0;
      return;
   }
   else
      toplevel->screen_position_get(toplevel, x, y);
}

/**
 * @brief Gets the size of the toplevel widget
 * @param toplevel a toplevel widget
 * @param w the location where to store the width of the toplevel widget
 * @param h the location where to store the height of the toplevel widget
 */
void etk_toplevel_size_get(Etk_Toplevel *toplevel, int *w, int *h)
{
   if (!toplevel || !toplevel->size_get)
   {
      if (w)  *w = 0;
      if (h)  *h = 0;
      return;
   }
   else
      toplevel->size_get(toplevel, w, h);
}

/**
 * @brief Sets the focused widget of the toplevel widget. Only for widget implementations. If you want to focus a widget, use etk_widget_focus()
 * @param toplevel a toplevel widget
 * @param widget the widget to set as focused
 */
void  etk_toplevel_focused_widget_set(Etk_Toplevel *toplevel, Etk_Widget *widget)
{
   if (!toplevel || (widget == toplevel->focused_widget))
      return;

   toplevel->focused_widget = widget;
   etk_object_notify(ETK_OBJECT(toplevel), "focused_widget");
}

/**
 * @brief Gets the focused widget of the toplevel widget
 * @param toplevel a toplevel widget
 * @return Returns the focused widget of the toplevel widget
 */
Etk_Widget *etk_toplevel_focused_widget_get(Etk_Toplevel *toplevel)
{
   if (!toplevel)
      return NULL;
   return toplevel->focused_widget;
}

/**
 * @brief Gets the previous widget to focus.  Mainly for widget implementations
 * @param toplevel a toplevel widget
 * @return Returns the previous widget to focus
 */
Etk_Widget *etk_toplevel_focused_widget_prev_get(Etk_Toplevel *toplevel)
{
   Etk_Widget *focused_parent;

   if (!toplevel)
      return NULL;

   if (toplevel->focused_widget && (focused_parent = toplevel->focused_widget->parent))
      return _etk_toplevel_prev_to_focus_get(focused_parent, toplevel->focused_widget);
   else
      return _etk_toplevel_prev_to_focus_get(ETK_WIDGET(toplevel), NULL);
}

/**
 * @brief Gets the next widget to focus. Mainly for widget implementations
 * @param toplevel a toplevel widget
 * @return Returns the next widget to focus
 */
Etk_Widget *etk_toplevel_focused_widget_next_get(Etk_Toplevel *toplevel)
{
   Etk_Widget *focused_parent;

   if (!toplevel)
      return NULL;

   if (toplevel->focused_widget && (focused_parent = toplevel->focused_widget->parent))
      return _etk_toplevel_next_to_focus_get(focused_parent, toplevel->focused_widget);
   else
      return _etk_toplevel_next_to_focus_get(ETK_WIDGET(toplevel), NULL);
}

/**
 * @brief Pushs a pointer type on the pointer stack. It will change the pointer shape
 * @param toplevel a toplevel widget
 * @param pointer_type the type of pointer to push on the stack
 */ 
void etk_toplevel_pointer_push(Etk_Toplevel *toplevel, Etk_Pointer_Type pointer_type)
{
   Etk_Pointer_Type *prev_pointer_type;
   Etk_Pointer_Type *new_pointer_type;

   if (!toplevel)
      return;

   new_pointer_type = malloc(sizeof(Etk_Pointer_Type));
   *new_pointer_type = pointer_type;
   prev_pointer_type = evas_list_data(evas_list_last(toplevel->pointer_stack));
   toplevel->pointer_stack = evas_list_append(toplevel->pointer_stack, new_pointer_type);

   if (toplevel->pointer_set && (!prev_pointer_type || (*prev_pointer_type != pointer_type)))
      toplevel->pointer_set(toplevel, pointer_type);
}

/**
 * @brief Pops a pointer type out of the pointer stack. It may change the pointer shape if the popped pointer type was on the top of the stack
 * @param toplevel a toplevel widget
 * @param pointer_type the type of pointer to pop out of the stack. -1 to pop the last pointer on the stack
 */ 
void etk_toplevel_pointer_pop(Etk_Toplevel *toplevel, Etk_Pointer_Type pointer_type)
{
   Evas_List *l;
   Etk_Pointer_Type prev_pointer_type;
   Etk_Pointer_Type *prev_pointer_type_ptr;
   Etk_Pointer_Type *current_pointer_ptr;
   Etk_Pointer_Type *p;
   
   if (!toplevel || !(prev_pointer_type_ptr = evas_list_data(evas_list_last(toplevel->pointer_stack))))
      return;
   
   prev_pointer_type = *prev_pointer_type_ptr;
   
   if (pointer_type < 0)
   {
      toplevel->pointer_stack = evas_list_remove_list(toplevel->pointer_stack, evas_list_last(toplevel->pointer_stack));
      free(prev_pointer_type_ptr);
   }
   else
   {
      for (l = evas_list_last(toplevel->pointer_stack); l; l = l->prev)
      {
         p = l->data;
         if (*p == pointer_type)
         {
            toplevel->pointer_stack = evas_list_remove_list(toplevel->pointer_stack, l);
            free(p);
            break;
         }
      }
   }

   if (toplevel->pointer_set)
   {
      if ((current_pointer_ptr = evas_list_data(evas_list_last(toplevel->pointer_stack))))
      {
         if (*current_pointer_ptr != prev_pointer_type)
            toplevel->pointer_set(toplevel, *current_pointer_ptr);
      }
      else
         toplevel->pointer_set(toplevel, ETK_POINTER_DEFAULT);
   }
}

/**
 * @brief Gets a list of all the created toplevel widgets
 * @return Returns a list of all the created toplevel widgets
 */
Evas_List *etk_toplevel_widgets_get(void)
{
   return _etk_toplevel_widgets;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members of the toplevel widget */
static void _etk_toplevel_constructor(Etk_Toplevel *toplevel)
{
   if (!toplevel)
      return;

   toplevel->evas = NULL;
   toplevel->focused_widget = NULL;
   toplevel->pointer_stack = NULL;
   toplevel->pointer_set = NULL;
   toplevel->evas_position_get = NULL;
   toplevel->screen_position_get = NULL;
   toplevel->size_get = NULL;
   toplevel->need_update = ETK_FALSE;
   ETK_WIDGET(toplevel)->toplevel_parent = toplevel;
   etk_signal_connect("realize", ETK_OBJECT(toplevel), ETK_CALLBACK(_etk_toplevel_realize_cb), NULL);

   _etk_toplevel_widgets = evas_list_append(_etk_toplevel_widgets, toplevel);
}

/* Destroys the toplevel widget */
static void _etk_toplevel_destructor(Etk_Toplevel *toplevel)
{
   Evas_List *l;

   if (!toplevel)
      return;

   for (l = toplevel->pointer_stack; l; l = l->next)
      free(l->data);
   toplevel->pointer_stack = evas_list_free(toplevel->pointer_stack);
   _etk_toplevel_widgets = evas_list_remove(_etk_toplevel_widgets, toplevel);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_toplevel_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Toplevel *toplevel;

   if (!(toplevel = ETK_TOPLEVEL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TOPLEVEL_FOCUSED_WIDGET_PROPERTY:
         etk_toplevel_focused_widget_set(toplevel, ETK_WIDGET(etk_property_value_pointer_get(value)));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_toplevel_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Toplevel *toplevel;

   if (!(toplevel = ETK_TOPLEVEL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TOPLEVEL_EVAS_PROPERTY:
         etk_property_value_pointer_set(value, toplevel->evas);
         break;
      case ETK_TOPLEVEL_FOCUSED_WIDGET_PROPERTY:
         etk_property_value_pointer_set(value, toplevel->focused_widget);
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
static void _etk_toplevel_realize_cb(Etk_Object *object, void *data)
{
   Etk_Toplevel *toplevel;

   if (!(toplevel = ETK_TOPLEVEL(object)))
      return;

   if (!toplevel->focused_widget && ETK_WIDGET(toplevel)->smart_object)
      evas_object_focus_set(ETK_WIDGET(toplevel)->smart_object, 1);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Gets recursively the next widget to focus */
static Etk_Widget *_etk_toplevel_next_to_focus_get(Etk_Widget *node, Etk_Widget *from)
{
   Evas_List *focus_order;
   Evas_List *l;

   if (!node)
      return NULL;
   if (node->focusable)
      return node;
   
   focus_order = node->use_focus_order ? node->focus_order : node->children;
   if (!focus_order)
      return _etk_toplevel_next_to_focus_get(node->parent, node);

   if (from && (l = evas_list_find_list(focus_order, from)))
   {
      if (l->next)
         return _etk_toplevel_next_to_focus_get(ETK_WIDGET(l->next->data), NULL);
      else
         return _etk_toplevel_next_to_focus_get(node->parent, node);
   }
   else 
      return _etk_toplevel_next_to_focus_get(ETK_WIDGET(focus_order->data), NULL);
}

/* Gets recursively the previous widget to focus */
static Etk_Widget *_etk_toplevel_prev_to_focus_get(Etk_Widget *node, Etk_Widget *from)
{
   Evas_List *focus_order;
   Evas_List *l;

   if (!node)
      return NULL;
   if (node->focusable)
      return node;
   
   focus_order = node->use_focus_order ? node->focus_order : node->children;
   if (!focus_order)
      return _etk_toplevel_prev_to_focus_get(node->parent, node);

   if (from && (l = evas_list_find_list(focus_order, from)))
   {
      if (l->prev)
         return _etk_toplevel_prev_to_focus_get(ETK_WIDGET(l->prev->data), NULL);
      else
         return _etk_toplevel_prev_to_focus_get(node->parent, node);
   }
   else 
      return _etk_toplevel_prev_to_focus_get(ETK_WIDGET(evas_list_data(evas_list_last(focus_order))), NULL);
}

/** @} */
