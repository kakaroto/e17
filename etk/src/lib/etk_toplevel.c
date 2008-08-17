/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_toplevel.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_toplevel.h"

#include <stdlib.h>

#include "etk_event.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Toplevel
 * @{
 */

#define ETK_TOPLEVEL_FOCUS_ORDER(widget)    ((widget)->focus_order ? (widget)->focus_order : (widget)->children)

enum Etk_Toplevel_Property_Id
{
   ETK_TOPLEVEL_EVAS_PROPERTY,
   ETK_TOPLEVEL_FOCUSED_WIDGET_PROPERTY
};

static void _etk_toplevel_constructor(Etk_Toplevel *toplevel);
static void _etk_toplevel_destructor(Etk_Toplevel *toplevel);
static void _etk_toplevel_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_toplevel_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_toplevel_realized_cb(Etk_Object *object, void *data);
static void _etk_toplevel_key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_toplevel_key_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static Etk_Widget *_etk_toplevel_prev_to_focus_get(Etk_Toplevel *toplevel, Etk_Widget *widget);
static Etk_Widget *_etk_toplevel_next_to_focus_get(Etk_Toplevel *toplevel, Etk_Widget *widget);

static Evas_List *_etk_toplevel_widgets = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Toplevel
 * @return Returns the type of an Etk_Toplevel
 */
Etk_Type *etk_toplevel_type_get(void)
{
   static Etk_Type *toplevel_type = NULL;

   if (!toplevel_type)
   {
      toplevel_type = etk_type_new("Etk_Toplevel", ETK_BIN_TYPE,
         sizeof(Etk_Toplevel),
         ETK_CONSTRUCTOR(_etk_toplevel_constructor),
         ETK_DESTRUCTOR(_etk_toplevel_destructor), NULL);

      etk_type_property_add(toplevel_type, "evas", ETK_TOPLEVEL_EVAS_PROPERTY,
         ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE, etk_property_value_pointer(NULL));
      etk_type_property_add(toplevel_type, "focused-widget", ETK_TOPLEVEL_FOCUSED_WIDGET_PROPERTY,
         ETK_PROPERTY_OBJECT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_object(NULL));

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
 * @brief Sets the focused widget of the toplevel widget. If you want to focus a widget, use etk_widget_focus() rather
 * @param toplevel a toplevel widget
 * @param widget the widget to set as focused
 * @widget_implementation
 */
void  etk_toplevel_focused_widget_set(Etk_Toplevel *toplevel, Etk_Widget *widget)
{
   if (!toplevel || (widget == toplevel->focused_widget))
      return;

   toplevel->focused_widget = widget;
   etk_object_notify(ETK_OBJECT(toplevel), "focused-widget");
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
 * @brief Gets the previous widget to focus
 * @param toplevel a toplevel widget
 * @return Returns the previous widget to focus
 * @widget_implementation
 */
Etk_Widget *etk_toplevel_focused_widget_prev_get(Etk_Toplevel *toplevel)
{
   Etk_Widget *prev;

   if (!toplevel)
      return NULL;

   prev = toplevel->focused_widget;
   while ((prev = _etk_toplevel_prev_to_focus_get(toplevel, prev))
         && (!prev->focusable || etk_widget_disabled_get(prev)));
   return prev;
}

/**
 * @brief Gets the next widget to focus
 * @param toplevel a toplevel widget
 * @return Returns the next widget to focus
 * @widget_implementation
 */
Etk_Widget *etk_toplevel_focused_widget_next_get(Etk_Toplevel *toplevel)
{
   Etk_Widget *next;

   if (!toplevel)
      return NULL;

   next = toplevel->focused_widget;
   while ((next = _etk_toplevel_next_to_focus_get(toplevel, next))
         && (!next->focusable || etk_widget_disabled_get(next)));
   return next;
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
 * @brief Pops out of the pointer stack the first pointer of the stack whose type corresponds to @a pointer_type.
 * It will change the pointer shape if the pointer to pop out is the one currently on the top of the stack
 * @param toplevel a toplevel widget
 * @param pointer_type the type of pointer to pop out of the stack. -1 to pop the top pointer of the stack
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
         toplevel->pointer_set(toplevel, ETK_POINTER_NONE);
   }
}

/**
 * @brief Gets a list of all the created toplevel widgets
 * @return Returns a list of all the created toplevel widgets
 * @note This list should not be modified or freed
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
   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(toplevel), ETK_CALLBACK(_etk_toplevel_realized_cb), NULL);

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
         etk_toplevel_focused_widget_set(toplevel, ETK_WIDGET(etk_property_value_object_get(value)));
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
         etk_property_value_object_set(value, ETK_OBJECT(toplevel->focused_widget));
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
static Etk_Bool _etk_toplevel_realized_cb(Etk_Object *object, void *data)
{
   Etk_Widget *widget;
   Evas_Object *obj;

   if (!(widget = ETK_WIDGET(object)) || !(obj = widget->smart_object))
      return ETK_TRUE;

   evas_object_focus_set(obj, 1);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_KEY_DOWN, _etk_toplevel_key_down_cb, widget);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_KEY_UP, _etk_toplevel_key_up_cb, widget);
   return ETK_TRUE;
}

/* Called when a key of the keyboard is pressed (if the toplevel widget is focused) */
static void _etk_toplevel_key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Toplevel *toplevel;
   Etk_Widget *focused, *widget;
   Etk_Event_Key_Down event;
   Etk_Bool propagate = ETK_TRUE;

   if (!(toplevel = ETK_TOPLEVEL(data)))
      return;

   focused = toplevel->focused_widget ? toplevel->focused_widget : ETK_WIDGET(toplevel);
   for (widget = focused; widget && propagate; widget = widget->parent)
   {
      etk_event_key_down_wrap(widget, event_info, &event);
      propagate = etk_signal_emit(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(widget), &event);
   }
}

/* Called when a key of the keyboard is released (if the toplevel widget is focused) */
static void _etk_toplevel_key_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Toplevel *toplevel;
   Etk_Widget *focused, *widget;
   Etk_Event_Key_Up event;
   Etk_Bool propagate = ETK_TRUE;

   if (!(toplevel = ETK_TOPLEVEL(data)))
      return;

   focused = toplevel->focused_widget ? toplevel->focused_widget : ETK_WIDGET(toplevel);
   for (widget = focused; widget && propagate; widget = widget->parent)
   {
      etk_event_key_up_wrap(widget, event_info, &event);
      propagate = etk_signal_emit(ETK_WIDGET_KEY_UP_SIGNAL, ETK_OBJECT(widget), &event);
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Gets the previous widget to focus */
static Etk_Widget *_etk_toplevel_prev_to_focus_get(Etk_Toplevel *toplevel, Etk_Widget *widget)
{
   Etk_Widget *prev;
   Evas_List *focus_order, *l;

   if (!toplevel)
      return NULL;
   if (!widget)
   {
      widget = ETK_WIDGET(toplevel);
      while (!widget->focusable && (focus_order = ETK_TOPLEVEL_FOCUS_ORDER(widget)))
         widget = ETK_WIDGET(evas_list_last(focus_order)->data);
      if (widget->focusable)
         return widget;
   }

   prev = NULL;
   for ( ; widget->parent; widget = widget->parent)
   {
      focus_order = ETK_TOPLEVEL_FOCUS_ORDER(widget->parent);
      if ((l = evas_list_find_list(focus_order, widget)))
      {
         if (l->prev)
         {
            prev = ETK_WIDGET(l->prev->data);
            while (!prev->focusable)
            {
               if (!(focus_order = ETK_TOPLEVEL_FOCUS_ORDER(prev)))
                  break;
               prev = ETK_WIDGET(evas_list_last(focus_order)->data);
            }
            break;
         }
      }
      else
         break;
   }

   return prev;
}

/* Gets the next widget to focus */
static Etk_Widget *_etk_toplevel_next_to_focus_get(Etk_Toplevel *toplevel, Etk_Widget *widget)
{
   Etk_Widget *next;
   Evas_List *focus_order, *l;

   if (!toplevel)
      return NULL;
   if (!widget)
   {
      widget = ETK_WIDGET(toplevel);
      while (!widget->focusable && (focus_order = ETK_TOPLEVEL_FOCUS_ORDER(widget)))
         widget = ETK_WIDGET(focus_order->data);
      if (widget->focusable)
         return widget;
   }

   next = NULL;
   for ( ; widget->parent; widget = widget->parent)
   {
      focus_order = ETK_TOPLEVEL_FOCUS_ORDER(widget->parent);
      if ((l = evas_list_find_list(focus_order, widget)))
      {
         if (l->next)
         {
            next = ETK_WIDGET(l->next->data);
            while (!next->focusable)
            {
               if (!(focus_order = ETK_TOPLEVEL_FOCUS_ORDER(next)))
                  break;
               next = ETK_WIDGET(focus_order->data);
            }
            break;
         }
      }
      else
         break;
   }

   return next;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Toplevel
 *
 * A toplevel widget has also to provide the Evas that will be used by all its children. @n
 * You can use the toplevel parent of a widget to change the shape of the mouse pointer with
 * etk_toplevel_pointer_push() and etk_toplevel_pointer_pop(). @n
 * For example, to change the mouse pointer when the mouse is above a button:
 * @code
 * //Called when the button is entered: change the mouse pointer to ETK_POINTER_MOVE
 * void _button_entered_cb(Etk_Widget *widget, void *data)
 * {
 *    Etk_Toplevel *toplevel;
 *
 *    if (!(toplevel = etk_widget_toplevel_parent_get(widget)))
 *       return;
 *    etk_toplevel_pointer_push(toplevel, ETK_POINTER_MOVE);
 * }
 *
 * //Called when the button is left: restore the mouse pointer
 * void _button_left_cb(Etk_Widget *widget, void *data)
 * {
 *    Etk_Toplevel *toplevel;
 *
 *    if (!(toplevel = etk_widget_toplevel_parent_get(widget)))
 *       return;
 *    etk_toplevel_pointer_pop(toplevel, ETK_POINTER_MOVE);
 * }
 *
 * etk_signal_connect("entered", ETK_OBJECT(button), ETK_CALLBACK(_button_entered_cb), NULL);
 * etk_signal_connect("left", ETK_OBJECT(button), ETK_CALLBACK(_button_left_cb), NULL);
 * @endcode @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Bin
 *         - Etk_Toplevel
 *
 * \par Properties:
 * @prop_name "evas": The Evas of the toplevel widget
 * @prop_type Pointer (Evas *)
 * @prop_ro
 * @prop_val NULL
 * \par
 * @prop_name "focused-widget": The focused widget of the toplevel widget
 * @prop_type Pointer (Etk_Widget *)
 * @prop_rw
 * @prop_val NULL
 */
