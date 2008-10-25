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

/** @file etk_widget.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_widget.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_event.h"
#include "etk_marshallers.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_theme.h"
#include "etk_toplevel.h"
#include "etk_utils.h"

#define _XOPEN_SOURCE 600

/**
 * @addtogroup Etk_Widget
 * @{
 */

/* TODO: fixme: color_set() when adding a new child, when adding a swallowed object? */

#define ETK_WIDGET_CONTENT_PART      "etk.swallow.content"

typedef struct Etk_Widget_Member_Object
{
   Evas_Object *object;
   Etk_Bool visible:1;
} Etk_Widget_Member_Object;

typedef struct Etk_Widget_Swallowed_Object
{
   char *part;
   Evas_Object *object;
   Etk_Widget *widget;
} Etk_Widget_Swallowed_Object;

enum Etk_Widget_Property_Id
{
   ETK_WIDGET_PARENT_PROPERTY,
   ETK_WIDGET_THEME_FILE_PROPERTY,
   ETK_WIDGET_THEME_GROUP_PROPERTY,
   ETK_WIDGET_THEME_PARENT_PROPERTY,
   ETK_WIDGET_PADDING_PROPERTY,
   ETK_WIDGET_GEOMETRY_PROPERTY,
   ETK_WIDGET_REQUESTED_WIDTH_PROPERTY,
   ETK_WIDGET_REQUESTED_HEIGHT_PROPERTY,
   ETK_WIDGET_VISIBLE_PROPERTY,
   ETK_WIDGET_COLOR_PROPERTY,
   ETK_WIDGET_PROPAGATE_COLOR_PROPERTY,
   ETK_WIDGET_DISABLED_PROPERTY,
   ETK_WIDGET_INTERNAL_PROPERTY,
   ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY,
   ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY,
   ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY,
   ETK_WIDGET_FOCUSABLE_PROPERTY,
   ETK_WIDGET_FOCUS_ON_CLICK_PROPERTY
};

int ETK_WIDGET_SHOWN_SIGNAL;
int ETK_WIDGET_HIDDEN_SIGNAL;
int ETK_WIDGET_REALIZED_SIGNAL;
int ETK_WIDGET_UNREALIZED_SIGNAL;
int ETK_WIDGET_SIZE_REQUESTED_SIGNAL;
int ETK_WIDGET_MOUSE_IN_SIGNAL;
int ETK_WIDGET_MOUSE_OUT_SIGNAL;
int ETK_WIDGET_MOUSE_MOVE_SIGNAL;
int ETK_WIDGET_MOUSE_DOWN_SIGNAL;
int ETK_WIDGET_MOUSE_UP_SIGNAL;
int ETK_WIDGET_MOUSE_CLICK_SIGNAL;
int ETK_WIDGET_MOUSE_WHEEL_SIGNAL;
int ETK_WIDGET_KEY_DOWN_SIGNAL;
int ETK_WIDGET_KEY_UP_SIGNAL;
int ETK_WIDGET_ENTERED_SIGNAL;
int ETK_WIDGET_LEFT_SIGNAL;
int ETK_WIDGET_FOCUSED_SIGNAL;
int ETK_WIDGET_UNFOCUSED_SIGNAL;
int ETK_WIDGET_ENABLED_SIGNAL;
int ETK_WIDGET_DISABLED_SIGNAL;
int ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL;
int ETK_WIDGET_SELECTION_RECEIVED_SIGNAL;

static void _etk_widget_constructor(Etk_Widget *widget);
static void _etk_widget_destructor(Etk_Widget *widget);
static Etk_Bool _etk_widget_destroyed_cb(Etk_Object *object, void *data);
static void _etk_widget_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_widget_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static Etk_Bool _etk_widget_shown_handler(Etk_Widget *widget);
static Etk_Bool _etk_widget_entered_handler(Etk_Widget *widget);
static Etk_Bool _etk_widget_left_handler(Etk_Widget *widget);
static Etk_Bool _etk_widget_focused_handler(Etk_Widget *widget);
static Etk_Bool _etk_widget_unfocused_handler(Etk_Widget *widget);
static Etk_Bool _etk_widget_enabled_handler(Etk_Widget *widget);
static Etk_Bool _etk_widget_disabled_handler(Etk_Widget *widget);

static void _etk_widget_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_mouse_wheel_cb(void *data, Evas *evas, Evas_Object *object, void *event_info);
static void _etk_widget_focus_out_cb(void *data, Evas *evas,
                                     Evas_Object *object, void *event_info);
static Etk_Bool _etk_widget_signal_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data);
static Etk_Bool _etk_widget_signal_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);

static void _etk_widget_toplevel_evas_changed_cb(Etk_Object *object, const char *property_name, void *data);

static void _etk_widget_realize(Etk_Widget *widget);
static void _etk_widget_unrealize(Etk_Widget *widget);
static void _etk_widget_theme_group_full_update(Etk_Widget *widget);

static void _etk_widget_toplevel_parent_set(Etk_Widget *widget, Etk_Toplevel *toplevel_parent);
static void _etk_widget_realize_children(Etk_Widget *widget, Etk_Bool realize, Etk_Bool evas_changed);
static void _etk_widget_realize_theme_children(Etk_Widget *widget, Etk_Bool realize, Etk_Bool theme_group_changed);
static void _etk_widget_unrealize_all(Etk_Widget *widget);

static void _etk_widget_theme_min_size_calc(Etk_Widget *widget, int *w, int *h, Etk_Bool hidden_has_no_size);
static void _etk_widget_redraw_queue_recursive(Etk_Widget *widget);

static void _etk_widget_swallow_full(Etk_Widget *swallower, const char *part, Evas_Object *object, Etk_Widget *widget);
static void _etk_widget_unswallow_full(Etk_Widget *swallower, Eina_List *swo_node);

static void _etk_widget_object_add_to_smart(Etk_Widget *widget, Evas_Object *object, Etk_Bool clip);
static void _etk_widget_add_to_clip(Etk_Widget *widget, Evas_Object *clip);
static void _etk_widget_remove_from_clip(Etk_Widget *widget, Evas_Object *clip);
static void _etk_widget_real_color_get(Etk_Widget *widget, int *r, int *g, int *b, int *a);

static Eina_List *_etk_widget_member_object_find(Etk_Widget *widget, Evas_Object *object);
static void _etk_widget_member_object_intercept_show_cb(void *data, Evas_Object *obj);
static void _etk_widget_member_object_intercept_hide_cb(void *data, Evas_Object *obj);

static void _etk_widget_member_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_widget_clip_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_widget_swallowed_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static Etk_Bool _etk_widget_swallowed_widget_realized_cb(Etk_Object *object, void *data);

static Evas_Object *_etk_widget_smart_object_add(Evas *evas, Etk_Widget *widget);
static void _etk_widget_smart_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_widget_smart_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_widget_smart_object_show_cb(Evas_Object *obj);
static void _etk_widget_smart_object_hide_cb(Evas_Object *obj);
static void _etk_widget_smart_object_color_set_cb(Evas_Object *obj, int r, int g, int b, int a);
static void _etk_widget_smart_object_clip_set_cb(Evas_Object *object, Evas_Object *clip);
static void _etk_widget_smart_object_clip_unset_cb(Evas_Object *object);

static Evas_Object *_etk_widget_content_object_add(Evas *evas, Etk_Widget *widget);
static void _etk_widget_content_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_widget_content_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_widget_content_object_clip_set_cb(Evas_Object *obj, Evas_Object *clip);
static void _etk_widget_content_object_clip_unset_cb(Evas_Object *obj);

static void _evas_object_smart_move(Evas_Object *object, Evas_Coord x, Evas_Coord y);
static void _evas_object_smart_resize(Evas_Object *object, Evas_Coord w, Evas_Coord h);


static Etk_Bool _etk_widget_intercept_show_hide = ETK_TRUE;
static Evas_Smart *_etk_widget_smart_object_smart = NULL;
static Evas_Smart *_etk_widget_content_object_smart = NULL;
static Etk_Widget_Swallow_Error _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NONE;


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Widget
 * @return Returns the type of an Etk_Widget
 */
Etk_Type *etk_widget_type_get(void)
{
   static Etk_Type *widget_type = NULL;

   if (!widget_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_HANDLER(ETK_WIDGET_SHOWN_SIGNAL,
            "shown", Etk_Widget, shown_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_HIDDEN_SIGNAL,
            "hidden", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_REALIZED_SIGNAL,
            "realized", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_UNREALIZED_SIGNAL,
            "unrealized", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_SIZE_REQUESTED_SIGNAL,
            "size-requested", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_MOUSE_IN_SIGNAL,
            "mouse-in", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_MOUSE_OUT_SIGNAL,
            "mouse-out", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_MOUSE_MOVE_SIGNAL,
            "mouse-move", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_MOUSE_DOWN_SIGNAL,
            "mouse-down", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_MOUSE_UP_SIGNAL,
            "mouse-up", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_MOUSE_CLICK_SIGNAL,
            "mouse-click", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_MOUSE_WHEEL_SIGNAL,
            "mouse-wheel", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_KEY_DOWN_SIGNAL,
            "key-down", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_KEY_UP_SIGNAL,
            "key-up", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_HANDLER(ETK_WIDGET_ENTERED_SIGNAL,
            "entered", Etk_Widget, entered_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_WIDGET_LEFT_SIGNAL,
            "left", Etk_Widget, left_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_WIDGET_FOCUSED_SIGNAL,
            "focused", Etk_Widget, focused_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_WIDGET_UNFOCUSED_SIGNAL,
            "unfocused", Etk_Widget, unfocused_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_WIDGET_ENABLED_SIGNAL,
            "enabled", Etk_Widget, enabled_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_HANDLER(ETK_WIDGET_DISABLED_SIGNAL,
            "disabled", Etk_Widget, disabled_handler, etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL,
            "scroll-size-changed", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_WIDGET_SELECTION_RECEIVED_SIGNAL,
            "selection-received", etk_marshaller_POINTER),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      widget_type = etk_type_new("Etk_Widget", ETK_OBJECT_TYPE,
         sizeof(Etk_Widget), ETK_CONSTRUCTOR(_etk_widget_constructor),
         ETK_DESTRUCTOR(_etk_widget_destructor), signals);

      etk_type_property_add(widget_type, "parent", ETK_WIDGET_PARENT_PROPERTY,
            ETK_PROPERTY_OBJECT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_object(NULL));
      etk_type_property_add(widget_type, "theme-file", ETK_WIDGET_THEME_FILE_PROPERTY,
            ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(widget_type, "theme-group", ETK_WIDGET_THEME_GROUP_PROPERTY,
            ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(widget_type, "theme-parent", ETK_WIDGET_THEME_PARENT_PROPERTY,
            ETK_PROPERTY_OBJECT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_object(NULL));
      etk_type_property_add(widget_type, "padding", ETK_WIDGET_PADDING_PROPERTY,
            ETK_PROPERTY_OTHER, ETK_PROPERTY_NO_ACCESS, NULL);
      etk_type_property_add(widget_type, "geometry", ETK_WIDGET_GEOMETRY_PROPERTY,
            ETK_PROPERTY_OTHER, ETK_PROPERTY_NO_ACCESS, NULL);
      etk_type_property_add(widget_type, "requested-width", ETK_WIDGET_REQUESTED_WIDTH_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(-1));
      etk_type_property_add(widget_type, "requested-height", ETK_WIDGET_REQUESTED_HEIGHT_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(-1));
      etk_type_property_add(widget_type, "visible", ETK_WIDGET_VISIBLE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "color", ETK_WIDGET_COLOR_PROPERTY,
            ETK_PROPERTY_OTHER, ETK_PROPERTY_NO_ACCESS, NULL);
      etk_type_property_add(widget_type, "propagate-color", ETK_WIDGET_PROPAGATE_COLOR_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(widget_type, "disabled", ETK_WIDGET_DISABLED_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "internal", ETK_WIDGET_INTERNAL_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "focusable", ETK_WIDGET_FOCUSABLE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "focus-on-click", ETK_WIDGET_FOCUS_ON_CLICK_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "repeat-mouse-events", ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "pass-mouse-events", ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(widget_type, "has-event-object", ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));

      widget_type->property_set = _etk_widget_property_set;
      widget_type->property_get = _etk_widget_property_get;
   }

   return widget_type;
}

/**
 * @brief Creates a new widget: it calls the corresponding constructors (from the constructor of the base class to the
 * constructor of the more derived class) and then sets the values of the given properties
 * @param widget_type the type of widget to create
 * @param first_property the name of the first property to set
 * @param ... the value of the first property, followed by any number of property-name/property-value pairs,
 * terminated with NULL
 * @return Returns the new Etk_Widget of type @a widget_type
 * @note This function just calls etk_object_new() and casts the result to an "Etk_Widget *"
 */
Etk_Widget *etk_widget_new(Etk_Type *widget_type, const char *first_property, ...)
{
   Etk_Widget *new_widget;
   va_list args;

   if (!widget_type)
      return NULL;

   va_start(args, first_property);
   new_widget = ETK_WIDGET(etk_object_new_valist(widget_type, first_property, args));
   va_end(args);

   return new_widget;
}

/**
 * @brief Shows the widget
 * @param widget a widget
 * @note The widget will effectively be shown if all its parents are also shown
 */
void etk_widget_show(Etk_Widget *widget)
{
   Etk_Widget *parent;

   if (!widget || widget->visible)
      return;

   parent = widget->parent;
   widget->visible = ETK_TRUE;
   if (widget->smart_object && (!parent || (parent->smart_object && evas_object_visible_get(parent->smart_object))))
      evas_object_show(widget->smart_object);
   etk_widget_size_recalc_queue(widget);

   etk_signal_emit(ETK_WIDGET_SHOWN_SIGNAL, ETK_OBJECT(widget));
   etk_object_notify(ETK_OBJECT(widget), "visible");
}

/**
 * @brief Recursively shows the widget and its children
 * @param widget a widget
 */
void etk_widget_show_all(Etk_Widget *widget)
{
   Eina_List *l;

   if (!widget)
      return;

   if (!widget->internal)
      etk_widget_show(widget);
   for (l = widget->children; l; l = l->next)
      etk_widget_show_all(ETK_WIDGET(l->data));
}

/**
 * @brief Hides the widget
 * @param widget a widget
 */
void etk_widget_hide(Etk_Widget *widget)
{
   if (!widget || !widget->visible)
      return;

   widget->visible = ETK_FALSE;
   if (widget->smart_object)
      evas_object_hide(widget->smart_object);
   etk_widget_size_recalc_queue(widget);

   etk_signal_emit(ETK_WIDGET_HIDDEN_SIGNAL, ETK_OBJECT(widget));
   etk_object_notify(ETK_OBJECT(widget), "visible");
}

/**
 * @brief Recursively hides the widget and its children
 * @param widget a widget
 */
void etk_widget_hide_all(Etk_Widget *widget)
{
   Eina_List *l;

   if (!widget)
      return;

   if (!widget->internal)
      etk_widget_hide(widget);
   for (l = widget->children; l; l = l->next)
      etk_widget_hide_all(ETK_WIDGET(l->data));
}

/**
 * @brief Checks if the object is visible
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is visible, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_is_visible(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->visible;
}

/**
 * @brief Sets whether or not the widget is an internal widget: an internal widget is not affected by
 * etk_widget_show_all() and etk_widget_hide_all(). It prevents visibility issues when one calls
 * etk_widget_hide_all(widget) and then etk_widget_show(widget). @n
 * For example, if the label of a button wasn't an internal widget, calling etk_widget_hide_all(button) would hide the
 * button and the label, and etk_widget_show(button) would only make the button reappear. The label would be invisible
 * while it should be visible
 * @param widget a widget
 * @param internal ETK_TRUE to prevent the widget to be affected by etk_widget_show_all() and etk_widget_hide_all()
 * @widget_implementation
 */
void etk_widget_internal_set(Etk_Widget *widget, Etk_Bool internal)
{
   if (!widget || widget->internal == internal)
      return;

   widget->internal = internal;
   etk_object_notify(ETK_OBJECT(widget), "internal");
}

/**
 * @brief Gets whether the widget is an internal widget. See etk_widget_internal_set() for more info
 * about internal widgets
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is an internal widget, ETK_FALSE otherwise
 * @see etk_widget_internal_set()
 */
Etk_Bool etk_widget_internal_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->internal;
}

/**
 * @brief Focuses the widget. The focused widget is the one which receives the keyboard events
 * @param widget a widget
 * @note The widget has to be attached to a toplevel widget, otherwise it will have no effect
 */
void etk_widget_focus(Etk_Widget *widget)
{
   Etk_Widget *focused;

   if (!widget || !widget->toplevel_parent || !widget->focusable || etk_widget_disabled_get(widget))
      return;

   /* Make sure to set the focus on the smart object of the toplevel parent
    * because if many Etk_Embed are on the same Evas, only one of them will
    * be able to receive keyboard events. */
   evas_object_focus_set(ETK_WIDGET(widget->toplevel_parent)->smart_object, 1);

   if ((focused = etk_toplevel_focused_widget_get(widget->toplevel_parent)) && (widget == focused))
      return;

   if (focused)
      etk_widget_unfocus(focused);

   /* TODO: etk_toplevel_focused_widget_set() should emit the "focused" signal no? */
   etk_toplevel_focused_widget_set(widget->toplevel_parent, widget);
   etk_signal_emit(ETK_WIDGET_FOCUSED_SIGNAL, ETK_OBJECT(widget));
}

/**
 * @brief Unfocuses the widget
 * @param widget a widget
 */
void etk_widget_unfocus(Etk_Widget *widget)
{
   if (!widget || !widget->toplevel_parent || !etk_widget_is_focused(widget))
      return;

   /* TODO: etk_toplevel_focused_widget_set() should emit the "unfocused" signal no? */
   etk_toplevel_focused_widget_set(widget->toplevel_parent, NULL);
   etk_signal_emit(ETK_WIDGET_UNFOCUSED_SIGNAL, ETK_OBJECT(widget));
}

/**
 * @brief Gets whether the widget is focused
 * @param widget a widget
 * @return Returns ETK_TRUE if @a widget is focused, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_is_focused(Etk_Widget *widget)
{
   if (!widget || !widget->toplevel_parent)
      return ETK_FALSE;
   return (etk_toplevel_focused_widget_get(widget->toplevel_parent) == widget);
}

/**
 * @brief Sets whether or not the widget can be focused
 * @param widget a widget
 * @param focusable ETK_TRUE to make @a widget focusable, ETK_FALSE otherwise
 */
void etk_widget_focusable_set(Etk_Widget *widget, Etk_Bool focusable)
{
   if (!widget || widget->focusable == focusable)
      return;

   if (!focusable)
      etk_widget_unfocus(widget);
   widget->focusable = focusable;
   etk_object_notify(ETK_OBJECT(widget), "focusable");
}

/**
 * @brief Gets whether the widget can be focused or not
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is focusable, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_focusable_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->focusable;
}

/**
 * @brief Sets whether or not the widget is disabled. When a widget is disabled, the user can't interact
 * with it anymore until it's enabled again. The widget's look will also change to make it look inactive
 * @param widget a widget
 * @param disabled ETK_TRUE to disable the widget, ETK_FALSE to enable it
 */
void etk_widget_disabled_set(Etk_Widget *widget, Etk_Bool disabled)
{
   if (!widget || widget->disabled == disabled)
      return;

   widget->disabled = disabled;
   if (disabled)
      etk_widget_unfocus(widget);

   if (widget->smart_object && !widget->pass_mouse_events)
      evas_object_pass_events_set(widget->smart_object, disabled);
   if (disabled)
      etk_signal_emit(ETK_WIDGET_DISABLED_SIGNAL, ETK_OBJECT(widget));
   else
      etk_signal_emit(ETK_WIDGET_ENABLED_SIGNAL, ETK_OBJECT(widget));
   etk_object_notify(ETK_OBJECT(widget), "disabled");
}

/**
 * @brief Recursively disables or enables the widget and its children
 * @param widget a widget
 * @param disabled ETK_TRUE to disable the widget and its children, ETK_FALSE to enable them
 */
void etk_widget_disabled_set_all(Etk_Widget *widget, Etk_Bool disabled)
{
   Eina_List *l;

   if (!widget)
      return;

   etk_widget_disabled_set(widget, disabled);
   for (l = widget->children; l; l = l->next)
      etk_widget_disabled_set_all(ETK_WIDGET(l->data), disabled);
}

/**
 * @brief Gets whether or not the widget is disabled
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is disabled, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_disabled_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->disabled;
}

/**
 * @brief Raises a widget: it will be moved above all the other widgets that have the same parent as @a widget
 * @param widget the widget to raise
 */
void etk_widget_raise(Etk_Widget *widget)
{
   Eina_List *l;

   if (!widget || !widget->parent)
      return;

   if ((l = eina_list_data_find_list(widget->parent->children, widget)))
   {
      widget->parent->children = eina_list_remove_list(widget->parent->children, l);
      widget->parent->children = eina_list_append(widget->parent->children, widget);
   }

   if (widget->smart_object)
      evas_object_raise(widget->smart_object);
}

/**
 * @brief Lowers a widget: it will be moved below all the other widgets that have the same parent as @a widget
 * @param widget the widget to lower
 */
void etk_widget_lower(Etk_Widget *widget)
{
   Eina_List *l;

   if (!widget || !widget->parent)
      return;

   if ((l = eina_list_data_find_list(widget->parent->children, widget)))
   {
      widget->parent->children = eina_list_remove_list(widget->parent->children, l);
      widget->parent->children = eina_list_prepend(widget->parent->children, widget);
   }

   /* Lower the smart-object */
   if (widget->smart_object)
   {
      if (widget->parent->content_object)
         evas_object_lower(widget->smart_object);
      else if (widget->parent->theme_object)
         evas_object_stack_above(widget->smart_object, widget->theme_object);
      else if (widget->parent->event_object)
         evas_object_stack_above(widget->smart_object, widget->event_object);
      else
         evas_object_lower(widget->smart_object);
   }
}

/**
 * @brief Emits the "entered" signal on the widget. The widget will look like if the mouse had entered the widget
 * @param widget a widget
 */
Etk_Bool etk_widget_enter(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_signal_emit(ETK_WIDGET_ENTERED_SIGNAL, ETK_OBJECT(widget));
   return ETK_TRUE;
}

/**
 * @brief Emits the "left" signal on the widget. The widget will look like if the mouse had left the widget
 * @param widget a widget
 */
Etk_Bool etk_widget_leave(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_signal_emit(ETK_WIDGET_LEFT_SIGNAL, ETK_OBJECT(widget));
   return ETK_TRUE;
}

/**
 * @brief Sets whether the mouse-events received by the widget should be propagated to its parent. By default,
 * mouse-events are not propagated (when you click a button, the window containing the button doesn't receive the
 * click event)
 * @param widget a widget
 * @param repeat_mouse_events if @a repeat_mouse_events is ETK_TRUE, the parent
 * widget will also receive the mouse-events
 */
void etk_widget_repeat_mouse_events_set(Etk_Widget *widget, Etk_Bool repeat_mouse_events)
{
   if (!widget || widget->repeat_mouse_events == repeat_mouse_events)
      return;

   widget->repeat_mouse_events = repeat_mouse_events;
   etk_object_notify(ETK_OBJECT(widget), "repeat-mouse-events");
}

/**
 * @brief Checks whether the mouse-events received by the widget are propagated to its parent
 * @param widget a widget
 * @return Returns ETK_TRUE if the mouse-events received by the widget are propagated to its parent
 */
Etk_Bool etk_widget_repeat_mouse_events_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->repeat_mouse_events;
}

/**
 * @brief Sets whether the widget should ignore the mouse-events it receives.
 * In this case, the mouse-events will be directly passed to its parent
 * @param widget a widget
 * @param pass_mouse_events if @a pass_mouse_events is ETK_TRUE, the mouse-events will be
 * directly passed to the parent. @a widget won't receive mouse-events anymore
 */
void etk_widget_pass_mouse_events_set(Etk_Widget *widget, Etk_Bool pass_mouse_events)
{
   if (!widget || widget->pass_mouse_events == pass_mouse_events)
      return;

   widget->pass_mouse_events = pass_mouse_events;
   if (widget->smart_object && !widget->disabled)
      evas_object_pass_events_set(widget->smart_object, pass_mouse_events);
   etk_object_notify(ETK_OBJECT(widget), "pass-mouse-events");
}

/**
 * @brief Checks whether the mouse-events received by the widget are ignored
 * by the widget and passed directly to its parent
 * @param widget a widget
 * @return Returns ETK_TRUE if the the mouse-events are ignored by the widget, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_pass_mouse_events_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->pass_mouse_events;
}

/**
 * @brief Sets whether or not the widget has an event-object. An event-object is an invisible rectangle that
 * is used to grab the mouse events on the widget. It can be useful for example if you want a container
 * with no theme-object (a table, a box, ...) to still receive the mouse events. @n
 * If @a widget already has a theme-object, this function has no effect (the theme-object is indeed already
 * used to grab the mouse events)
 * @param widget a widget
 * @param has_event_object ETK_TRUE to make the widget have an event-object, ETK_FALSE otherwise
 */
void etk_widget_has_event_object_set(Etk_Widget *widget, Etk_Bool has_event_object)
{
   Evas *evas;

   if (!widget || widget->has_event_object == has_event_object)
      return;

   widget->has_event_object = has_event_object;

   if (has_event_object)
   {
      if (!widget->theme_object && (evas = etk_widget_toplevel_evas_get(widget)))
      {
         widget->event_object = evas_object_rectangle_add(evas);
         evas_object_color_set(widget->event_object, 0, 0, 0, 0);
         evas_object_show(widget->event_object);
         _etk_widget_object_add_to_smart(widget, widget->event_object, ETK_TRUE);
         evas_object_lower(widget->event_object);
         etk_widget_redraw_queue(widget);
      }
   }
   else if (widget->event_object)
   {
      evas_object_del(widget->event_object);
      widget->event_object = NULL;
   }

   etk_object_notify(ETK_OBJECT(widget), "has-event-object");
}

/**
 * @brief Gets whether the widget has an event-object
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget has an event-object, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_has_event_object_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->has_event_object;
}

/**
 * @brief Sets the parent of the widget. If the widget has already a parent, it will be reparented
 * @param widget a widget
 * @param parent the new parent of @a widget
 * @widget_implementation
 * @note If you want to add a widget to a container, use etk_container_add() or the appropriate function of
 * the container's API. Otherwise the widget will not be packed correctly
 */
void etk_widget_parent_set(Etk_Widget *widget, Etk_Widget *parent)
{
   Etk_Widget *prev_parent;
   Etk_Toplevel *prev_toplevel, *new_toplevel;
   Evas *new_evas, *prev_evas;
   const char *prev_theme_file, *new_theme_file;

   if (!widget || ETK_IS_TOPLEVEL(widget))
      return;

   prev_parent = widget->parent;
   prev_toplevel = widget->toplevel_parent;
   prev_evas = etk_widget_toplevel_evas_get(widget);
   prev_theme_file = etk_widget_theme_file_get(widget);

   new_toplevel = etk_widget_toplevel_parent_get(parent);
   new_evas = etk_widget_toplevel_evas_get(parent);
   if (widget->theme_file)
      new_theme_file = widget->theme_file;
   else
      new_theme_file = etk_widget_theme_file_get(widget->theme_parent ? widget->theme_parent : parent);

   /* Remove the widget from its current parent */
   if (prev_parent)
   {
      Eina_List *l;

      if ((l = eina_list_data_find_list(prev_parent->children, widget)))
      {
         if (widget->swallowed)
            etk_widget_unswallow_widget(prev_parent, widget);
         if (widget->smart_object)
            evas_object_smart_member_del(widget->smart_object);
         if (widget->clip && prev_parent->clip == widget->clip)
            etk_widget_clip_unset(widget);

         prev_parent->children = eina_list_remove_list(prev_parent->children, l);
         etk_widget_size_recalc_queue(prev_parent);
      }
   }
   /* And set the new parent of the widget */
   if (parent)
      parent->children = eina_list_append(parent->children, widget);
   widget->parent = parent;

   if (new_toplevel != prev_toplevel)
      _etk_widget_toplevel_parent_set(widget, new_toplevel);

   /* Realize/unrealize the widget and its children */
   /* TODO: re-read this... */
   if (new_evas)
   {
      Etk_Bool same_theme_file;

      same_theme_file = (strcmp(prev_theme_file ? prev_theme_file : "", new_theme_file ? new_theme_file : "") == 0);
      if (new_evas != prev_evas)
         _etk_widget_realize_children(widget, ETK_TRUE, ETK_TRUE);
      if (!same_theme_file)
         _etk_widget_realize_theme_children(widget, (new_evas == prev_evas), ETK_FALSE);
      /* TODO: add the smart-object as a member of the parent's smart-object ? */
   }
   else if (widget->realized)
      _etk_widget_unrealize_all(widget);

   etk_object_notify(ETK_OBJECT(widget), "parent");
}

/**
 * @brief Gets the parent of the widget
 * @param widget a widget
 * @return Returns the parent of the widget
 */
Etk_Widget *etk_widget_parent_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->parent;
}

/**
 * @brief Gets the toplevel widget that contains @a widget
 * @param widget a widget
 * @return Returns the toplevel widget if @a widget is contained by a toplevel widget, or NULL otherwise
 */
Etk_Toplevel *etk_widget_toplevel_parent_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->toplevel_parent;
}

/**
 * @brief Gets the evas of the toplevel widget that contains @a widget
 * @param widget a widget
 * @return Returns the evas if @a widget is contained by a toplevel widget, or NULL otherwise
 */
Evas *etk_widget_toplevel_evas_get(Etk_Widget *widget)
{
   if (!widget || !widget->toplevel_parent)
      return NULL;
   return etk_toplevel_evas_get(widget->toplevel_parent);
}

/**
 * @brief Changes the theme of the widget: it sets both the theme-file and the theme-group in a more optimized way
 * than just calling etk_widget_theme_file_set() and then etk_widget_theme_group_set(). But if you only want to change
 * the theme-file OR the theme-group, you should better call directly the corresponding function
 * (etk_widget_theme_file_set() or etk_widget_theme_group_set())
 * @param widget a widget
 * @param theme_file the path of the theme-file. If NULL, it will use the first non-NULL theme-file of its parents.
 * If none of its parents has a non-NULL theme-file, the current theme-file of Etk. @n
 * The new theme-file will also be automatically used by all the children of @widget, as long as they have a
 * theme-file set to NULL
 * @param theme_group the theme-group to use
 */
void etk_widget_theme_set(Etk_Widget *widget, const char *theme_file, const char *theme_group)
{
   if (!widget)
      return;

   if (theme_file != widget->theme_file)
   {
      free(widget->theme_file);
      widget->theme_file = theme_file ? strdup(theme_file) : NULL;
   }
   if (theme_group != widget->theme_group)
   {
      free(widget->theme_group);
      widget->theme_group = theme_group ? strdup(theme_group) : NULL;
      _etk_widget_theme_group_full_update(widget);
   }

   _etk_widget_realize_children(widget, ETK_TRUE, ETK_FALSE);
   _etk_widget_realize_theme_children(widget, ETK_FALSE, ETK_TRUE);
   etk_object_notify(ETK_OBJECT(widget), "theme-file");
   etk_object_notify(ETK_OBJECT(widget), "theme-group");
}

/**
 * @brief Sets the theme-file used by the widget. The widget will be automatically re-realized
 * @param widget a widget
 * @param theme_file the path of the theme-file. If NULL, it will use the first non-NULL theme-file of its parents.
 * If none of its parents has a non-NULL theme-file, the current theme-file of Etk. @n
 * The new theme-file will also be automatically used by all the children of @widget, as long as they have a
 * theme-file set to NULL
 */
void etk_widget_theme_file_set(Etk_Widget *widget, const char *theme_file)
{
   if (!widget)
      return;

   if (theme_file != widget->theme_file)
   {
      free(widget->theme_file);
      widget->theme_file = theme_file ? strdup(theme_file) : NULL;

      _etk_widget_realize_children(widget, ETK_TRUE, ETK_FALSE);
      _etk_widget_realize_theme_children(widget, ETK_FALSE, ETK_FALSE);
      etk_object_notify(ETK_OBJECT(widget), "theme-file");
   }
}

/**
 * @brief Gets the theme-file used by the widget
 * @param widget a widget
 * @return Returns the path to the theme-file used by the widget (NULL if the widget uses the current theme of Etk)
 * @note The returned value may be different from the theme-file set with etk_widget_theme_file_set(): if
 * the theme-file has been set to NULL, etk_widget_theme_file_get() will look recursively for the first non-NULL
 * theme-file of its parents. If none of its parents have a non-NULL theme-file, etk_widget_theme_file_get() will
 * return NULL (meaning that @a widget uses the current theme of Etk). @n
 * To get the value set with etk_widget_theme_file_set(), you can use
 * etk_object_properties_get(ETK_OBJECT(widget), "theme-file", &theme_file, NULL);
 */
const char *etk_widget_theme_file_get(Etk_Widget *widget)
{
   Etk_Widget *parent;

   for (parent = widget; parent; parent = parent->theme_parent ? parent->theme_parent : parent->parent)
   {
      if (parent->theme_file)
         return parent->theme_file;
   }

   return NULL;
}

/**
 * @brief Sets the theme-group of the widget. The theme-group of a widget is also used by the theme-children of the
 * widget. For example, the theme-group of a tree is "tree", and the theme-group of its rows is "row". Since the rows
 * are theme-children of the tree, the edje-group really loaded for the rows is "etk/tree/rows". It also means
 * that if you change the theme-group of the tree later to "new_tree" in order to change its appearance, the appearance
 * of the rows will also be updated (the rows will use automatically the edje-group called "etk/new_tree/rows").
 * @param widget a widget
 * @param theme_group the theme-group to use
 */
void etk_widget_theme_group_set(Etk_Widget *widget, const char *theme_group)
{
   if (!widget)
      return;

   if (theme_group != widget->theme_group
         && !(widget->theme_group && theme_group && strcmp(widget->theme_group, theme_group) == 0))
         /* TODO: wtf? */
         //&& (strcmp(widget->theme_group ? widget->theme_group : "", theme_group ? theme_group : "") != 0))
   {
      free(widget->theme_group);
      widget->theme_group = theme_group ? strdup(theme_group) : NULL;

      _etk_widget_theme_group_full_update(widget);
      _etk_widget_realize_theme_children(widget, ETK_TRUE, ETK_TRUE);
      etk_object_notify(ETK_OBJECT(widget), "theme-group");
   }
}

/**
 * @brief Gets the full theme-group of the widget
 * @param widget a widget
 * @return Returns the full theme-group of the widget
 * @note The returned value may be different from the theme-group set with etk_widget_theme_group_set() because
 * if @a widget has a theme-parent, it will be taken into account. For example, if @a widget is a row, and
 * the theme-parent of the widget is a tree, the returned value will be "tree/row", and not just "row". @n
 * To get the value set with etk_widget_theme_group_set(), you can use
 * etk_object_properties_get(ETK_OBJECT(widget), "theme-group", &theme_group, NULL);
 */
const char *etk_widget_theme_group_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->theme_group_full;
}

/**
 * @brief Sets the theme-parent of the widget. The widget will automatically inherit the theme-group from the
 * theme-parent. See etk_widget_theme_group_set() for more info.
 * @param widget a widget
 * @param theme_parent The theme-parent to use
 * @widget_implementation
 * @see etk_widget_theme_group_set()
 */
void etk_widget_theme_parent_set(Etk_Widget *widget, Etk_Widget *theme_parent)
{
   if (!widget || widget->theme_parent == theme_parent)
      return;

   if (widget->theme_parent)
      widget->theme_parent->theme_children = eina_list_remove(widget->theme_parent->theme_children, widget);
   if (theme_parent)
      theme_parent->theme_children = eina_list_append(theme_parent->theme_children, widget);
   widget->theme_parent = theme_parent;

   _etk_widget_theme_group_full_update(widget);
   _etk_widget_realize_children(widget, ETK_TRUE, ETK_FALSE);
   _etk_widget_realize_theme_children(widget, ETK_FALSE, ETK_TRUE);
   etk_object_notify(ETK_OBJECT(widget), "theme-parent");
}

/**
 * @brief Gets the theme-parent of the widget
 * @param widget a widget
 * @return Returns the theme-parent of the widget
 * @widget_implementation
 */
Etk_Widget *etk_widget_theme_parent_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->theme_parent;
}

/**
 * @brief Sets the color of the widget. By default, the color change is also propagated automatically
 * to the children of the widget. To change this behavior, see @a etk_widget_propagate_color_set()
 * @param widget a widget
 * @param r the red component of the color to set
 * @param g the green component of the color to set
 * @param b the blue component of the color to set
 * @param a the alpha component of the color to set
 * @note The color should be alpha-premultiplied
 */
void etk_widget_color_set(Etk_Widget *widget, int r, int g, int b, int a)
{
   if (!widget)
      return;

   if (widget->smart_object)
      evas_object_color_set(widget->smart_object, r, g, b, a);
   else
   {
      widget->color.r = r;
      widget->color.g = g;
      widget->color.b = b;
      widget->color.a = a;
      etk_object_notify(ETK_OBJECT(widget), "color");
   }
}

/**
 * @brief Gets the current color of the widget
 * @param widget a widget
 * @param r the location where to store the red component of the color
 * @param g the location where to store the green component of the color
 * @param b the location where to store the blue component of the color
 * @param a the location where to store the alpha component of the color
 * @note The returned color is alpha-premultiplied
 */
void etk_widget_color_get(Etk_Widget *widget, int *r, int *g, int *b, int *a)
{
   if (r)   *r = widget ? widget->color.r : 255;
   if (g)   *g = widget ? widget->color.g : 255;
   if (b)   *b = widget ? widget->color.b : 255;
   if (a)   *a = widget ? widget->color.a : 255;
}

/**
 * @brief Sets whether or not a color change on the widget should be propagated to its children.
 * If the color change is propagated, the color of the widget's children will be multiplied by the color of the widget
 * @param widget a widget
 * @param propagate_color ETK_TRUE to "propagate" the color changes on the widget, ETK_FALSE otherwise
 */
void etk_widget_propagate_color_set(Etk_Widget *widget, Etk_Bool propagate_color)
{
   Eina_List *l;
   Etk_Widget *child;

   if (!widget || widget->propagate_color == propagate_color)
      return;

   widget->propagate_color = propagate_color;
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (child->smart_object)
         evas_object_color_set(child->smart_object, child->color.r, child->color.g, child->color.b, child->color.a);
   }

   etk_object_notify(ETK_OBJECT(widget), "propagate-color");
}

/**
 * @brief Gets whether or not a color change on the widget is propagated to its children
 * @param widget a widget
 * @return Returns ETK_TRUE if the color changes are propagated, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_propagate_color_get(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->propagate_color;
}

/**
 * @brief Sets the padding on the different sides of the widget.
 * Padding is a blank space on the sides of the widget
 * @param widget a widget
 * @param left the padding at the left of the widget
 * @param right the padding at the right of the widget
 * @param top the padding at the top of the widget
 * @param bottom the padding at the bottom of the widget
 */
void etk_widget_padding_set(Etk_Widget *widget, int left, int right, int top, int bottom)
{
   if (!widget)
      return;

   widget->padding.left = ETK_MAX(0, left);
   widget->padding.right = ETK_MAX(0, right);
   widget->padding.top = ETK_MAX(0, top);
   widget->padding.bottom = ETK_MAX(0, bottom);

   etk_object_notify(ETK_OBJECT(widget), "padding");
   etk_widget_size_recalc_queue(widget);
}

/**
 * @brief Gets the padding on the different sides of the widget
 * @param widget a widget
 * @param left the location where to store the padding at the left of the widget
 * @param right the location where to store the padding at the right of the widget
 * @param top the location where to store the padding at the top of the widget
 * @param bottom the location where to store the padding at the bottom of the widget
 */
void etk_widget_padding_get(Etk_Widget *widget, int *left, int *right, int *top, int *bottom)
{
   if (left)    *left = widget ? widget->padding.left : 0;
   if (right)   *right = widget ? widget->padding.right : 0;
   if (top)     *top = widget ? widget->padding.top : 0;
   if (bottom)  *bottom = widget ? widget->padding.bottom : 0;
}

/**
 * @brief Gets the geometry of the widget, relative to the top-left corner of the Evas where it is drawn
 * @param widget a widget
 * @param x the location where to store the x position of the widget
 * @param y the location where to store the y position of the widget
 * @param w the location where to store the width of the widget
 * @param h the location where to store the height of the widget
 */
void etk_widget_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h)
{
   if (x)   *x = widget ? widget->geometry.x : 0;
   if (y)   *y = widget ? widget->geometry.y : 0;
   if (w)   *w = widget ? widget->geometry.w : 0;
   if (h)   *h = widget ? widget->geometry.h : 0;
}

/**
 * @brief Gets the inner geometry of the widget, relative to the top-left corner of the Evas where it is drawn. @n
 * The inner geometry corresponds to the rectangle in which the widget's children are rendered. This rectangle may be
 * smaller than the geometry of the widget because inset values can be set by the theme
 * @param widget a widget
 * @param x the location where to store the inner x position of the widget
 * @param y the location where to store the inner y position of the widget
 * @param w the location where to store the inner width of the widget
 * @param h the location where to store the inner height of the widget
 */
void etk_widget_inner_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h)
{
   if (x)   *x = widget ? widget->inner_geometry.x : 0;
   if (y)   *y = widget ? widget->inner_geometry.y : 0;
   if (w)   *w = widget ? widget->inner_geometry.w : 0;
   if (h)   *h = widget ? widget->inner_geometry.h : 0;
}

/**
 * @brief Queues a size recalculation request: during the next main-loop iteration, the size of the widget will be
 * recalculated, and the widget will be redrawn
 * @param widget the widget to queue
 * @widget_implementation
 */
void etk_widget_size_recalc_queue(Etk_Widget *widget)
{
   Etk_Widget *w;

   if (!widget)
      return;

   for (w = widget; w; w = ETK_WIDGET(w->parent))
   {
      if ((w->swallowed || w->content_object) && w->parent)
         w->parent->need_theme_size_recalc = ETK_TRUE;
      w->need_size_recalc = ETK_TRUE;
      w->need_redraw = ETK_TRUE;
   }

   if (widget->toplevel_parent)
      widget->toplevel_parent->need_update = ETK_TRUE;
}

/**
 * @brief Queues a redraw request: during the next main-loop iteration, the widget and its children will be redrawn
 * (i.e. etk_widget_size_allocate() will be called on them)
 * @param widget the widget to queue
 * @widget_implementation
 */
void etk_widget_redraw_queue(Etk_Widget *widget)
{
   if (!widget)
      return;

   _etk_widget_redraw_queue_recursive(widget);
   if (widget->toplevel_parent)
      widget->toplevel_parent->need_update = ETK_TRUE;
}

/**
 * @brief Sets the size-request of the widget. Use this function to request Etk to resize the widget to the given size. @n
 * It does not mean the widget will be allocated this size: in some cases, the widget may be allocated less than its
 * size-request, and often the allocated size can be larger than the size-request (if the widget expands for example). @n
 * You can use this function to force a widget to be smaller or larger than its default size
 * @param widget a widget
 * @param w the width to request (-1 will make Etk calculate it automatically)
 * @param h the height to request (-1 will make Etk calculate it automatically)
 * @note The padding values will be added to the final size-request
 */
void etk_widget_size_request_set(Etk_Widget *widget, int w, int h)
{
   if (!widget)
      return;

   widget->requested_size.w = w;
   widget->requested_size.h = h;

   etk_widget_size_recalc_queue(widget);
   etk_object_notify(ETK_OBJECT(widget), "requested-width");
   etk_object_notify(ETK_OBJECT(widget), "requested-height");
}

/**
 * @brief Calculates the size-request of the widget
 * @param widget a widget
 * @param size_requisition the location where to store the requested size
 * @note etk_widget_size_request_full(widget, size_request) is equivalent to
 * etk_widget_size_request_full(widget, size_request, ETK_TRUE)
 * @widget_implementation
 */
void etk_widget_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   etk_widget_size_request_full(widget, size_requisition, ETK_TRUE);
}

/**
 * @brief Calculates the size-request of the widget
 * @param widget a widget
 * @param size_requisition the location where to store the ideal size
 * @param hidden_has_no_size if @a hidden_has_no_size is ETK_TRUE, then if the widget is hidden,
 * the returned size will be 0x0. If it is ETK_FALSE, the size-request will be calculated even if the widget is hidden
 * @widget_implementation
 */
void etk_widget_size_request_full(Etk_Widget *widget, Etk_Size *size_requisition, Etk_Bool hidden_has_no_size)
{
   if (!widget || !size_requisition)
      return;

   size_requisition->w = -1;
   size_requisition->h = -1;

   if (!widget->realized)
   {
      size_requisition->w = widget->padding.left + widget->padding.right;
      size_requisition->h = widget->padding.top + widget->padding.bottom;
      return;
   }

   if (!widget->visible && hidden_has_no_size)
      size_requisition->w = 0;
   else if (widget->requested_size.w >= 0)
      size_requisition->w = widget->requested_size.w;
   else if (widget->last_calced_size.w >= 0
         && !widget->need_size_recalc && (widget->visible || hidden_has_no_size))
   {
      size_requisition->w = widget->last_calced_size.w;
   }

   if (!widget->visible && hidden_has_no_size)
      size_requisition->h = 0;
   else if (widget->requested_size.h >= 0)
      size_requisition->h = widget->requested_size.h;
   else if (widget->last_calced_size.h >= 0
         && !widget->need_size_recalc && (widget->visible || hidden_has_no_size))
   {
      size_requisition->h = widget->last_calced_size.h;
   }

   /* We need to recalc it */
   if (size_requisition->w < 0 || size_requisition->h < 0)
   {
      int min_w, min_h;

      _etk_widget_theme_min_size_calc(widget, &min_w, &min_h, hidden_has_no_size);
      if (!widget->content_object && widget->size_request)
      {
         Etk_Size widget_requisition;

         widget->size_request(widget, &widget_requisition);
         if (size_requisition->w < 0)
            size_requisition->w = ETK_MAX(min_w, widget_requisition.w + widget->inset.left + widget->inset.right);
         if (size_requisition->h < 0)
            size_requisition->h = ETK_MAX(min_h, widget_requisition.h + widget->inset.top + widget->inset.bottom);
      }
      else
      {
         if (size_requisition->w < 0)
            size_requisition->w = min_w;
         if (size_requisition->h < 0)
            size_requisition->h = min_h;
      }
   }

   if (widget->visible || !hidden_has_no_size)
      widget->last_calced_size = *size_requisition;

   size_requisition->w += widget->padding.left + widget->padding.right;
   size_requisition->h += widget->padding.top + widget->padding.bottom;

   widget->need_size_recalc = ETK_FALSE;
   etk_signal_emit(ETK_WIDGET_SIZE_REQUESTED_SIGNAL, ETK_OBJECT(widget), size_requisition);
}

/**
 * @brief Moves and resizes the widget to the given geometry
 * @param widget a widget
 * @param geometry the geometry the widget should occupy.
 * The position should be relative to the Evas where the widget is drawn.
 * @widget_implementation
 */
void etk_widget_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   int x, y, w, h;

   if (!widget || widget->swallowed || !widget->smart_object)
      return;

   evas_object_geometry_get(widget->smart_object, &x, &y, &w, &h);
   if (geometry.x != x || geometry.y != y || widget->need_redraw)
      _evas_object_smart_move(widget->smart_object, geometry.x, geometry.y);
   if (geometry.w != w || geometry.h != h || widget->need_redraw)
      _evas_object_smart_resize(widget->smart_object, geometry.w, geometry.h);
}

/**
 * @brief Sends a signal to the theme-object of the widget
 * @param widget a widget
 * @param signal_name the signal to send
 * @param size_recalc if @a size_recalc is ETK_TRUE, etk_widget_size_recalc_queue() will be called on the widget.
 * So, if the emitted signal is likely to change the size of the widget, @a size_recalc has to be set to ETK_TRUE.
 * Otherwise it should be set to ETK_FALSE
 * @note The widget has to be realized, otherwise it will have no effect
 * @widget_implementation
 * @see edje_object_signal_emit()
 */
void etk_widget_theme_signal_emit(Etk_Widget *widget, const char *signal_name, Etk_Bool size_recalc)
{
   if (!widget)
      return;

   if (widget->theme_object)
      edje_object_signal_emit(widget->theme_object, signal_name, "etk");
   if (widget->theme_signal_emit)
      widget->theme_signal_emit(widget, signal_name, size_recalc);

   if (size_recalc)
   {
      widget->need_theme_size_recalc = ETK_TRUE;
      etk_widget_size_recalc_queue(widget);
   }
}

/**
 * @brief Sets the text of a text-part of the widget's theme-object.
 * @param widget a widget
 * @param part_name the name of the text-part
 * @param text the text to set
 * @note The widget has to be realized, otherwise it will have no effect
 * @note etk_widget_size_recalc_queue() is automatically called on the widget
 * @widget_implementation
 * @see edje_object_part_text_set()
 */
void etk_widget_theme_part_text_set(Etk_Widget *widget, const char *part_name, const char *text)
{
   if (!widget || !widget->theme_object)
      return;
   edje_object_part_text_set(widget->theme_object, part_name, text);
   widget->need_theme_size_recalc = ETK_TRUE;
   etk_widget_size_recalc_queue(widget);
}

/**
 * @brief Gets the data called @a data_name from the theme of the widget
 * @param widget a widget
 * @param data_name the name of the data you want to get
 * @param format the format of the data. Same format as the format of sscanf()
 * @param ... the location of the variables where to store the values
 * @return Returns the number of the input items successfully matched and assigned, same as sscanf
 * @note The widget has to be realized, otherwise it will have no effect
 * @widget_implementation
 * @see edje_object_data_get()
 */
int etk_widget_theme_data_get(Etk_Widget *widget, const char *data_name, const char *format, ...)
{
   const char *data_string;
   int result;
   va_list args;

   if (!widget || !data_name || !format || !widget->theme_object)
      return 0;
   if (!(data_string = edje_object_data_get(widget->theme_object, data_name)))
      return 0;

   va_start(args, format);
   result = vsscanf(data_string, format, args);
   va_end(args);

   return result;
}

/**
 * @brief Adds an evas object to the list of member-objects of the widget. @n
 * A member-object is an object which is automatically shown/hidden when the widget is shown/hidden,
 * which is clipped against the clip-object of the widget, and which is used by the widget to receive the mouse
 * events. A member-object is also stacked on the same level as the widget. @n
 * The object will be automatically deleted when the object is unrealized, unless you remove it from the list
 * with etk_widget_member_object_del() before the widget is unrealized.
 * @param widget a widget
 * @param object the evas object to add
 * @return Returns ETK_TRUE on success. ETK_FALSE on failure, probably because the widget and the object do not
 * belong to the same evas, or because the widget is not realized yet
 * @note The object has to belong to the same evas as the widget
 * @widget_implementation
 */
Etk_Bool etk_widget_member_object_add(Etk_Widget *widget, Evas_Object *object)
{
   Etk_Widget_Member_Object *member_object;
   Etk_Widget *prev_widget;
   int r, g, b, a;
   int r2, g2, b2, a2;

   if (!widget || !object || !widget->realized
         || (evas_object_evas_get(object) != etk_widget_toplevel_evas_get(widget)))
      return ETK_FALSE;
   if (_etk_widget_member_object_find(widget, object))
      return ETK_TRUE;

   if ((prev_widget = ETK_WIDGET(evas_object_data_get(object, "_Etk_Widget::Parent"))))
      etk_widget_member_object_del(prev_widget, object);
   else if ((prev_widget = ETK_WIDGET(evas_object_data_get(object, "_Etk_Widget::Swallower"))))
      etk_widget_unswallow_object(prev_widget, object);

   member_object = malloc(sizeof(Etk_Widget_Member_Object));
   member_object->object = object;
   member_object->visible = evas_object_visible_get(object);

   /* TODO: is that ok? should it be added to add_to_smart? */
   _etk_widget_real_color_get(widget, &r, &g, &b, &a);
   evas_object_color_get(object, &r2, &g2, &b2, &a2);
   evas_object_color_set(object, (r * r2) / 255, (g * g2) / 255, (b * b2) / 255, (a * a2) / 255);

   _etk_widget_object_add_to_smart(widget, object, (evas_object_clip_get(object) == NULL));
   evas_object_intercept_show_callback_add(object, _etk_widget_member_object_intercept_show_cb, widget);
   evas_object_intercept_hide_callback_add(object, _etk_widget_member_object_intercept_hide_cb, widget);
   evas_object_event_callback_add(object, EVAS_CALLBACK_FREE, _etk_widget_member_object_deleted_cb, widget);
   evas_object_data_set(object, "_Etk_Widget::Parent", widget);

   widget->member_objects = eina_list_append(widget->member_objects, member_object);

   return ETK_TRUE;
}

/**
 * @brief Removes an evas object from the list of member-objects of the widget. See etk_widget_member_object_add() for
 * more info about member-objects.
 * @param widget a widget
 * @param object the evas object to remove
 * @note The object will still be displayed on the Evas after this call
 * @widget_implementation
 * @see etk_widget_member_object_add()
 */
void etk_widget_member_object_del(Etk_Widget *widget, Evas_Object *object)
{
   Eina_List *l;
   Etk_Widget_Member_Object *member_object;

   if (!widget || !object)
      return;

   if ((l = _etk_widget_member_object_find(widget, object)))
   {
      member_object = l->data;

      evas_object_smart_member_del(object);
      if (widget->clip)
         evas_object_clip_unset(member_object->object);

      evas_object_intercept_show_callback_del(member_object->object, _etk_widget_member_object_intercept_show_cb);
      evas_object_intercept_hide_callback_del(member_object->object, _etk_widget_member_object_intercept_hide_cb);
      evas_object_event_callback_del(object, EVAS_CALLBACK_FREE, _etk_widget_member_object_deleted_cb);
      evas_object_data_del(object, "_Etk_Widget::Parent");

      free(member_object);
      widget->member_objects = eina_list_remove_list(widget->member_objects, l);
   }
}

/**
 * @brief Stacks @a object above all the other member-objects of the widget
 * @param widget a widget
 * @param object the object to raise
 * @widget_implementation
 */
void etk_widget_member_object_raise(Etk_Widget *widget, Evas_Object *object)
{
   if (!widget || !object)
      return;

   if (_etk_widget_member_object_find(widget, object))
      evas_object_raise(object);
}

/**
 * @brief Stacks @a object below all the other member-objects of the widget
 * @param widget a widget
 * @param object the object to lower
 * @widget_implementation
 */
void etk_widget_member_object_lower(Etk_Widget *widget, Evas_Object *object)
{
   if (!widget || !object)
      return;

   if (_etk_widget_member_object_find(widget, object))
   {
      if (widget->content_object)
         evas_object_lower(object);
      else if (widget->theme_object)
         evas_object_stack_above(object, widget->theme_object);
      else if (widget->event_object)
         evas_object_stack_above(object, widget->event_object);
      else
         evas_object_lower(object);
   }
}

/**
 * @brief Stacks the member-object @a object above the member-object @a above
 * @param widget a widget
 * @param object the object to restack
 * @param above the object above which @a object will be stacked
 * @widget_implementation
 */
void etk_widget_member_object_stack_above(Etk_Widget *widget, Evas_Object *object, Evas_Object *above)
{
   if (!widget || !above || (object == above))
      return;

   if (_etk_widget_member_object_find(widget, object) && _etk_widget_member_object_find(widget, above))
      evas_object_stack_above(object, above);
}

/**
 * @brief Stacks the member-object @a object below the member-object @a below
 * @param widget a widget
 * @param object the object to restack
 * @param below the object below which @a object will be stacked
 * @widget_implementation
 */
void etk_widget_member_object_stack_below(Etk_Widget *widget, Evas_Object *object, Evas_Object *below)
{
   if (!widget || !below || (object == below))
      return;

   if (_etk_widget_member_object_find(widget, object) && _etk_widget_member_object_find(widget, below))
      evas_object_stack_below(object, below);
}

/**
 * @brief Makes the widget swallow another widget in a part of its theme-object
 * @param swallower the widget that will swallow the widget @a to_swallow. @a swallower has to
 * be realized and its theme-object should have a part called @a part
 * @param part the name of the part of the theme-object in which @a to_swallow should be swallowed
 * @param to_swallow the widget to swallow. @a to_swallow has to be a child of @a swallower
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure (it may occur if @a swallower is not realized, if the part
 * doesn't exist or if @a to_swallow is not a child of @a swallower)
 * @widget_implementation
 */
Etk_Bool etk_widget_swallow_widget(Etk_Widget *swallower, const char *part, Etk_Widget *to_swallow)
{
   if (!swallower || !part || !to_swallow)
      return ETK_FALSE;
   if (!(swallower->theme_object))
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NO_THEME_OBJECT;
      return ETK_FALSE;
   }
   if (to_swallow->parent != swallower)
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_INCOMPATIBLE_PARENT;
      return ETK_FALSE;
   }
   if (!edje_object_part_exists(swallower->theme_object, part))
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NO_PART;
      return ETK_FALSE;
   }

   if (to_swallow->swallowed)
      etk_widget_unswallow_widget(to_swallow->parent, to_swallow);

   _etk_widget_swallow_full(swallower, part, to_swallow->smart_object, to_swallow);
   to_swallow->swallowed = ETK_TRUE;
   _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NONE;

   return ETK_TRUE;
}

/**
 * @brief Makes the widget @a swallower unswallow another widget
 * @param swallower the widget that is currently swallowing @a swallowed
 * @param swallowed the widget currently swallowed by @a swallower, and which should be unswallowed
 * @widget_implementation
 */
void etk_widget_unswallow_widget(Etk_Widget *swallower, Etk_Widget *swallowed)
{
   Eina_List *l;
   Etk_Widget_Swallowed_Object *swo;

   if (!swallower || !swallowed)
      return;

   for (l = swallower->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->widget == swallowed)
      {
         _etk_widget_unswallow_full(swallower, l);
         break;
      }
   }
}

/**
 * @brief Checks if the widget is swallowed by its parent
 * @return Returns ETK_TRUE if the widget is swallowed by its parent
 * @widget_implementation
 */
Etk_Bool etk_widget_is_swallowed(Etk_Widget *widget)
{
   if (!widget)
      return ETK_FALSE;
   return widget->swallowed;
}

/**
 * @brief Makes the widget swallow an Evas_Object in a part of its theme-object
 * @param swallower the widget that will swallow the object @a to_swallow. @a swallower has to
 * be realized and to have a theme-object
 * @param part the name of the part of the theme-object in which @a to_swallow should be swallowed
 * @param to_swallow the Evas_Object to swallow
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure (it may occur if @a swallower is not realized
 * or if the part doesn't exist)
 * @widget_implementation
 */
Etk_Bool etk_widget_swallow_object(Etk_Widget *swallower, const char *part, Evas_Object *to_swallow)
{
   if (!swallower || !part || !to_swallow)
      return ETK_FALSE;
   if (!swallower->theme_object)
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NO_THEME_OBJECT;
      return ETK_FALSE;
   }
   if (!edje_object_part_exists(swallower->theme_object, part))
   {
      _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NO_PART;
      return ETK_FALSE;
   }

   _etk_widget_swallow_full(swallower, part, to_swallow, NULL);
   _etk_widget_swallow_error = ETK_SWALLOW_ERROR_NONE;
   return ETK_TRUE;
}

/**
 * @brief Makes the widget unswallow an Evas_Object
 * @param swallower the widget that currently swallow @a object
 * @param object the Evas_Object currently swallowed by @a swallower, and that you want to be unswallowed
 * @widget_implementation
 */
void etk_widget_unswallow_object(Etk_Widget *swallower, Evas_Object *object)
{
   Eina_List *l;
   Etk_Widget_Swallowed_Object *swo;

   if (!swallower || !object)
      return;

   for (l = swallower->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->object == object)
      {
         _etk_widget_unswallow_full(swallower, l);
         break;
      }
   }
}

/**
 * @brief Gets the code corresponding to the error that occurs during the last call of
 * etk_widget_swallow_widget() or etk_widget_swallow_object()
 * @return Returns the error code
 * @widget_implementation
 */
Etk_Widget_Swallow_Error etk_widget_swallow_error_get(void)
{
   return _etk_widget_swallow_error;
}

/**
 * @brief Sets the clip-object of the widget. The theme-object and the member-objects of the widget will be
 * automatically clipped against @a clip
 * @param widget a widget
 * @param clip the clip-object to set
 * @widget_implementation
 */
void etk_widget_clip_set(Etk_Widget *widget, Evas_Object *clip)
{
   if (!widget)
      return;

   if (widget->clip)
      etk_widget_clip_unset(widget);

   if (clip)
   {
      if (widget->smart_object)
         evas_object_clip_set(widget->smart_object, clip);
      else
         _etk_widget_add_to_clip(widget, clip);
      widget->clip = clip;
   }
}

/**
 * @brief Unsets the clip-object of the widget. The theme-object and the member-objects won't be clipped anymore
 * @param widget a widget
 * @return Returns the clip-object of the widget
 * @widget_implementation
 */
void etk_widget_clip_unset(Etk_Widget *widget)
{
   if (!widget || !widget->clip)
      return;

   if (widget->smart_object)
      evas_object_clip_unset(widget->smart_object);
   else
      _etk_widget_remove_from_clip(widget, widget->clip);
   widget->clip = NULL;
}

/**
 * @brief Gets the clip-object of the widget
 * @param widget a widget
 * @return Returns the clip-object of the widget
 * @widget_implementation
 */
Evas_Object *etk_widget_clip_get(Etk_Widget *widget)
{
   if (!widget)
      return NULL;
   return widget->clip;
}

/**
 * @brief Sets whether the widget is dnd destination
 * @param widget a widget
 * @param on ETK_TRUE to enable this widget as a dnd destination, ETK_FALSE to disable it
 */
void etk_widget_dnd_dest_set(Etk_Widget *widget, Etk_Bool on)
{
   /* TODO: reimplement dnd */
}

/**
 * @brief Checks whether the widget is a dnd destination
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is a dnd destination, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_dnd_dest_get(Etk_Widget *widget)
{
   /* TODO: reimplement dnd */
   return ETK_FALSE;
}

/**
 * @brief Sets whether the widget is dnd source
 * @param widget a widget
 * @param on ETK_TRUE to enable this widget as a dnd source, ETK_FALSE to disable it
 */
void etk_widget_dnd_source_set(Etk_Widget *widget, Etk_Bool on)
{
   /* TODO: reimplement dnd */
   return;
}

/**
 * @brief Checks whether the widget is a dnd source
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget is a dnd source, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_dnd_source_get(Etk_Widget *widget)
{
   /* TODO: reimplement dnd */
   return ETK_FALSE;
}

/**
 * @brief Set whether the widget wants to handle its own dnd or not
 * @param widget a widget
 * @param on ETK_TRUE if the widget handles its own dnd, ETK_FALSE otherwise
 */
void etk_widget_dnd_internal_set(Etk_Widget *widget, Etk_Bool on)
{
   /* TODO: reimplement dnd */
}

/**
 * @brief Checks whether the widget wants to handle its own dnd or not
 * @param widget a widget
 * @return Returns ETK_TRUE if the widget handles its own dnd, ETK_FALSE otherwise
 */
Etk_Bool etk_widget_dnd_internal_get(Etk_Widget *widget)
{
   /* TODO: reimplement dnd */
   return ETK_FALSE;
}

/**
 * @brief Sets the visual data for the drag (the widget to be displayed)
 * @param widget a widget
 * @param drag_widget the widget that will appear in the drag window
 */
void etk_widget_dnd_drag_widget_set(Etk_Widget *widget, Etk_Widget *drag_widget)
{
   /* TODO: reimplement dnd */
}

/**
 * @brief Gets the visual data for the drag (the widget to be displayed)
 * @param widget a widget
 * @return Returns the widget that will appear in the drag window
 */
Etk_Widget *etk_widget_dnd_drag_widget_get(Etk_Widget *widget)
{
   /* TODO: reimplement dnd */
   return NULL;
}

/* TODO: doc */
void etk_widget_dnd_drag_data_set(Etk_Widget *widget, const char **types, int num_types, void *data, int data_size)
{
   /* TODO: reimplement dnd */
   return;
}

/**
 * @brief Sets the possible types for dnd
 * @param widget a widget
 * @param types list of acceptable types
 * @param num number of types
 */
void etk_widget_dnd_types_set(Etk_Widget *widget, const char **types, int num)
{
   /* TODO: reimplement dnd */
}

/**
 * @brief Gets the possible types for dnd
 * @param widget a widget
 * @param num number of types that the widget has
 * @return returns the dnd types this widget supports
 */
const char **etk_widget_dnd_types_get(Etk_Widget *widget, int *num)
{
   /* TODO: reimplement dnd */
   if (num)   *num = 0;
   return NULL;
}

/**
 * @brief Gets the list of the widgets that are dnd destinations
 * @return Returns the list of the dnd destination widgets
 */
Eina_List *etk_widget_dnd_dest_widgets_get(void)
{
   /* TODO: reimplement dnd */
   return NULL;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the widget */
static void _etk_widget_constructor(Etk_Widget *widget)
{
   if (!widget)
      return;

   widget->parent = NULL;
   widget->toplevel_parent = NULL;
   widget->children = NULL;
   widget->focus_order = NULL;

   widget->theme_object = NULL;
   widget->theme_min_size.w = 0;
   widget->theme_min_size.h = 0;
   widget->theme_file = NULL;
   widget->theme_group = NULL;
   widget->theme_group_full = NULL;
   widget->theme_parent = NULL;
   widget->theme_children = NULL;

   widget->smart_object = NULL;
   widget->event_object = NULL;
   widget->content_object = NULL;
   widget->clip = NULL;
   widget->swallowed_objects = NULL;
   widget->member_objects = NULL;

   widget->shown_handler = _etk_widget_shown_handler;
   widget->entered_handler = _etk_widget_entered_handler;
   widget->left_handler = _etk_widget_left_handler;
   widget->focused_handler = _etk_widget_focused_handler;
   widget->unfocused_handler = _etk_widget_unfocused_handler;
   widget->enabled_handler = _etk_widget_enabled_handler;
   widget->disabled_handler = _etk_widget_disabled_handler;

   widget->inset.left = 0;
   widget->inset.right = 0;
   widget->inset.top = 0;
   widget->inset.bottom = 0;
   widget->padding.left = 0;
   widget->padding.right = 0;
   widget->padding.top = 0;
   widget->padding.bottom = 0;

   widget->geometry.x = 0;
   widget->geometry.y = 0;
   widget->geometry.w = 0;
   widget->geometry.h = 0;
   widget->inner_geometry.x = 0;
   widget->inner_geometry.y = 0;
   widget->inner_geometry.w = 0;
   widget->inner_geometry.h = 0;
   widget->requested_size.w = -1;
   widget->requested_size.h = -1;
   widget->last_calced_size.w = 0;
   widget->last_calced_size.h = 0;

   widget->color.r = 255;
   widget->color.g = 255;
   widget->color.b = 255;
   widget->color.a = 255;
   widget->propagate_color = ETK_TRUE;

   widget->size_request = NULL;
   widget->size_allocate = NULL;
   widget->theme_signal_emit = NULL;
   widget->scroll_size_get = NULL;
   widget->scroll_margins_get = NULL;
   widget->scroll = NULL;

   widget->realized = ETK_FALSE;
   widget->swallowed = ETK_FALSE;
   widget->visible = ETK_FALSE;
   widget->internal = ETK_FALSE;
   widget->focusable = ETK_FALSE;
   widget->focus_on_click = ETK_FALSE;
   widget->disabled = ETK_FALSE;
   widget->has_event_object = ETK_FALSE;
   widget->repeat_mouse_events = ETK_FALSE;
   widget->pass_mouse_events = ETK_FALSE;
   widget->need_size_recalc = ETK_FALSE;
   widget->need_redraw = ETK_FALSE;
   widget->need_theme_size_recalc = ETK_FALSE;
   widget->swallowed = ETK_FALSE;

   etk_signal_connect_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_destroyed_cb), NULL);
   etk_signal_connect_swapped_by_code(ETK_WIDGET_MOUSE_IN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(etk_widget_enter), widget);
   etk_signal_connect_swapped_by_code(ETK_WIDGET_MOUSE_OUT_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(etk_widget_leave), widget);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_mouse_down_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_signal_key_down_cb), NULL);

   if (ETK_IS_TOPLEVEL(widget))
      etk_object_notification_callback_add(ETK_OBJECT(widget), "evas", _etk_widget_toplevel_evas_changed_cb, NULL);
}

/* Destroys the widget */
static void _etk_widget_destructor(Etk_Widget *widget)
{
   if (!widget)
      return;

   free(widget->theme_file);
   free(widget->theme_group);
   free(widget->theme_group_full);
}

/* Called when etk_object_destroy() is called on a widget. We use this to remove every reference to the
 * widget (reference from the parent, from the children, ...). We do that in this callback and not in the
 * destructor because the destructor is not called immediately when the widget is destroyed */
Etk_Bool _etk_widget_destroyed_cb(Etk_Object *object, void *data)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   Etk_Widget *theme_child;

   if (!(widget = ETK_WIDGET(object)))
      return ETK_TRUE;

   _etk_widget_unrealize(widget);

   /* Unparent the widget and destroy its children */
   etk_widget_parent_set(widget, NULL);
   while (widget->children)
   {
      child = ETK_WIDGET(widget->children->data);
      etk_object_destroy(ETK_OBJECT(child));
   }

   /* Remove any reference to the widget in the theme-parent/theme-children */
   while (widget->theme_children)
   {
      theme_child = ETK_WIDGET(widget->theme_children->data);
      etk_widget_theme_parent_set(theme_child, NULL);
   }
   if (widget->theme_parent)
      widget->theme_parent->theme_children = eina_list_remove(widget->theme_parent->theme_children, widget);

   if (widget->clip)
      _etk_widget_remove_from_clip(widget, widget->clip);
   widget->focus_order = eina_list_free(widget->focus_order);

   return ETK_TRUE;
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_widget_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_WIDGET_PARENT_PROPERTY:
         etk_widget_parent_set(widget, ETK_WIDGET(etk_property_value_object_get(value)));
         break;
      case ETK_WIDGET_THEME_FILE_PROPERTY:
         etk_widget_theme_file_set(widget, etk_property_value_string_get(value));
         break;
      case ETK_WIDGET_THEME_GROUP_PROPERTY:
         etk_widget_theme_group_set(widget, etk_property_value_string_get(value));
         break;
      case ETK_WIDGET_THEME_PARENT_PROPERTY:
         etk_widget_theme_parent_set(widget, ETK_WIDGET(etk_property_value_object_get(value)));
         break;
      case ETK_WIDGET_REQUESTED_WIDTH_PROPERTY:
         etk_widget_size_request_set(widget, etk_property_value_int_get(value), widget->requested_size.h);
         break;
      case ETK_WIDGET_REQUESTED_HEIGHT_PROPERTY:
         etk_widget_size_request_set(widget, widget->requested_size.w, etk_property_value_int_get(value));
         break;
      case ETK_WIDGET_VISIBLE_PROPERTY:
         if (etk_property_value_bool_get(value))
            etk_widget_show(widget);
         else
            etk_widget_hide(widget);
         break;
      case ETK_WIDGET_DISABLED_PROPERTY:
         etk_widget_disabled_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_INTERNAL_PROPERTY:
         etk_widget_internal_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY:
         etk_widget_repeat_mouse_events_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY:
         etk_widget_pass_mouse_events_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY:
         etk_widget_has_event_object_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_FOCUSABLE_PROPERTY:
         etk_widget_focusable_set(widget, etk_property_value_bool_get(value));
         break;
      case ETK_WIDGET_FOCUS_ON_CLICK_PROPERTY:
         widget->focus_on_click = etk_property_value_bool_get(value);
         etk_object_notify(object, "focus-on-click");
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_widget_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_WIDGET_PARENT_PROPERTY:
         etk_property_value_object_set(value, ETK_OBJECT(widget->parent));
         break;
      case ETK_WIDGET_THEME_FILE_PROPERTY:
         etk_property_value_string_set(value, widget->theme_file);
         break;
      case ETK_WIDGET_THEME_GROUP_PROPERTY:
         etk_property_value_string_set(value, widget->theme_group);
         break;
      case ETK_WIDGET_THEME_PARENT_PROPERTY:
         etk_property_value_object_set(value, ETK_OBJECT(widget->theme_parent));
         break;
      case ETK_WIDGET_REQUESTED_WIDTH_PROPERTY:
         etk_property_value_int_set(value, widget->requested_size.w);
         break;
      case ETK_WIDGET_REQUESTED_HEIGHT_PROPERTY:
         etk_property_value_int_set(value, widget->requested_size.h);
         break;
      case ETK_WIDGET_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, widget->visible);
         break;
      case ETK_WIDGET_DISABLED_PROPERTY:
         etk_property_value_bool_set(value, widget->disabled);
         break;
      case ETK_WIDGET_INTERNAL_PROPERTY:
         etk_property_value_bool_set(value, widget->internal);
         break;
      case ETK_WIDGET_REPEAT_MOUSE_EVENTS_PROPERTY:
         etk_property_value_bool_set(value, widget->repeat_mouse_events);
         break;
      case ETK_WIDGET_PASS_MOUSE_EVENTS_PROPERTY:
         etk_property_value_bool_set(value, widget->pass_mouse_events);
         break;
      case ETK_WIDGET_HAS_EVENT_OBJECT_PROPERTY:
         etk_property_value_bool_set(value, widget->has_event_object);
         break;
      case ETK_WIDGET_FOCUSABLE_PROPERTY:
         etk_property_value_bool_set(value, widget->focusable);
         break;
      case ETK_WIDGET_FOCUS_ON_CLICK_PROPERTY:
         etk_property_value_bool_set(value, widget->focus_on_click);
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

/* Default handler for the "entered" signal */
static Etk_Bool _etk_widget_entered_handler(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(widget, "etk,state,enter", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "left" signal */
static Etk_Bool _etk_widget_left_handler(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(widget, "etk,state,leave", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "focused" signal */
static Etk_Bool _etk_widget_focused_handler(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(widget, "etk,state,focused", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "unfocused" signal */
static Etk_Bool _etk_widget_unfocused_handler(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(widget, "etk,state,unfocused", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "shown" signal */
static Etk_Bool _etk_widget_shown_handler(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(widget, "etk,state,shown", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "enabled" signal */
static Etk_Bool _etk_widget_enabled_handler(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(widget, "etk,state,enabled", ETK_FALSE);
   return ETK_TRUE;
}

/* Default handler for the "disabled" signal */
static Etk_Bool _etk_widget_disabled_handler(Etk_Widget *widget)
{
   if (!widget)
      return ETK_TRUE;
   etk_widget_theme_signal_emit(widget, "etk,state,disabled", ETK_FALSE);
   return ETK_TRUE;
}

/* Evas Callback: Called when the mouse pointer enters the widget */
static void _etk_widget_mouse_in_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_In event;

   if (!(widget = ETK_WIDGET(data)))
      return;

   if (!widget->pass_mouse_events && !widget->disabled)
   {
      etk_event_mouse_in_wrap(widget, event_info, &event);
      etk_signal_emit(ETK_WIDGET_MOUSE_IN_SIGNAL, ETK_OBJECT(widget), &event);
   }
}

/* Evas Callback: Called when the mouse pointer leaves the widget */
static void _etk_widget_mouse_out_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Out event;

   if (!(widget = ETK_WIDGET(data)))
      return;

   if (!widget->pass_mouse_events && !widget->disabled)
   {
      etk_event_mouse_out_wrap(widget, event_info, &event);
      etk_signal_emit(ETK_WIDGET_MOUSE_OUT_SIGNAL, ETK_OBJECT(widget), &event);
   }
}

/* Evas Callback: Called when the mouse pointer moves over the widget */
static void _etk_widget_mouse_move_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Move event;

   if (!(widget = ETK_WIDGET(data)))
      return;

   if (!widget->pass_mouse_events && !widget->disabled)
   {
      etk_event_mouse_move_wrap(widget, event_info, &event);
      etk_signal_emit(ETK_WIDGET_MOUSE_MOVE_SIGNAL, ETK_OBJECT(widget), &event);
   }

   if (widget->parent)
      _etk_widget_mouse_move_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the widget is pressed by the mouse */
static void _etk_widget_mouse_down_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Down event;

   if (!(widget = ETK_WIDGET(data)))
      return;

   if (!widget->pass_mouse_events && !widget->disabled)
   {
      etk_event_mouse_down_wrap(widget, event_info, &event);
      etk_signal_emit(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(widget), &event);
   }

   if ((widget->pass_mouse_events || widget->repeat_mouse_events) && widget->parent)
      _etk_widget_mouse_down_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the widget is released by the mouse */
static void _etk_widget_mouse_up_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Up event;

   if (!(widget = ETK_WIDGET(data)))
      return;

   if (!widget->pass_mouse_events && !widget->disabled)
   {
      etk_event_mouse_up_wrap(widget, event_info, &event);
      etk_signal_emit(ETK_WIDGET_MOUSE_UP_SIGNAL, ETK_OBJECT(widget), &event);

      if (ETK_INSIDE(event.canvas.x, event.canvas.y,
            widget->geometry.x, widget->geometry.y, widget->geometry.w, widget->geometry.h) &&
	  (!(event.modifiers & ETK_MODIFIER_ON_HOLD)))
      {
         etk_signal_emit(ETK_WIDGET_MOUSE_CLICK_SIGNAL, ETK_OBJECT(widget), &event);
      }
   }

   if ((widget->pass_mouse_events || widget->repeat_mouse_events) && widget->parent)
      _etk_widget_mouse_up_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the mouse wheel is used over the widget */
static void _etk_widget_mouse_wheel_cb(void *data, Evas *evas, Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Event_Mouse_Wheel event;
   Etk_Bool propagate;

   if (!(widget = ETK_WIDGET(data)) || etk_widget_disabled_get(widget))
      return;

   etk_event_mouse_wheel_wrap(widget, event_info, &event);
   propagate = etk_signal_emit(ETK_WIDGET_MOUSE_WHEEL_SIGNAL, ETK_OBJECT(widget), &event);

   if (propagate && widget->parent)
      _etk_widget_mouse_wheel_cb(widget->parent, evas, NULL, event_info);
}

/* Evas Callback: Called when the widget loses focus */
static void _etk_widget_focus_out_cb(void *data, Evas *evas,
                                     Evas_Object *object, void *event_info)
{
   Etk_Widget *widget;
   Etk_Widget *focused;

   if (!(widget = ETK_WIDGET(data)))
      return;

   focused = etk_toplevel_focused_widget_get(widget->toplevel_parent);
   if (focused)
      etk_widget_unfocus(focused);
}

/* Signal Callback: Called when the user presses a key, if the widget is focused */
static Etk_Bool _etk_widget_signal_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Widget *widget;
   Etk_Toplevel *toplevel;

   if (!(widget = ETK_WIDGET(object)) || !event || !(toplevel = widget->toplevel_parent) || !event->keyname)
      return ETK_TRUE;

   if (strcmp(event->keyname, "Tab") == 0)
   {
      Etk_Widget *to_focus;
      if (event->modifiers & ETK_MODIFIER_SHIFT)
         to_focus = etk_toplevel_focused_widget_prev_get(toplevel);
      else
         to_focus = etk_toplevel_focused_widget_next_get(toplevel);

      if (to_focus)
         etk_widget_focus(to_focus);
      else
         etk_widget_unfocus(widget);

      return ETK_FALSE;
   }

   return ETK_TRUE;
}

/* Signal Callback: Called when the widget is pressed by the mouse */
static Etk_Bool _etk_widget_signal_mouse_down_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(object)))
      return ETK_TRUE;
   if (widget->focus_on_click)
      etk_widget_focus(widget);
   return ETK_TRUE;
}

/* Called when the evas of a toplevel widget is changed */
static void _etk_widget_toplevel_evas_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Widget *widget;
   Evas *evas;

   if (!(widget = ETK_WIDGET(object)))
      return;

   evas = etk_toplevel_evas_get(ETK_TOPLEVEL(widget));
   if (evas && (!widget->smart_object || evas_object_evas_get(widget->smart_object) != evas))
      _etk_widget_realize_children(widget, ETK_TRUE, ETK_TRUE);
   else if (!evas && widget->smart_object)
      _etk_widget_unrealize_all(widget);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Realizes the widget: it will load the theme and allocate the graphical ressources */
static void _etk_widget_realize(Etk_Widget *widget)
{
   Evas *evas = NULL;
   Eina_List *l;
   Etk_Widget *child;

   if (!widget || !(evas = etk_widget_toplevel_evas_get(widget)))
      return;

   if (widget->realized)
      _etk_widget_unrealize(widget);

   /* First, we create the smart object */
   widget->smart_object = _etk_widget_smart_object_add(evas, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_IN, _etk_widget_mouse_in_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_OUT, _etk_widget_mouse_out_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_MOVE, _etk_widget_mouse_move_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_DOWN, _etk_widget_mouse_down_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_UP, _etk_widget_mouse_up_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_MOUSE_WHEEL, _etk_widget_mouse_wheel_cb, widget);
   evas_object_event_callback_add(widget->smart_object, EVAS_CALLBACK_FOCUS_OUT,
                                  _etk_widget_focus_out_cb, widget);

   if (widget->parent && widget->parent->smart_object)
      _etk_widget_object_add_to_smart(widget->parent, widget->smart_object, (widget->clip == NULL));
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (child->smart_object)
         _etk_widget_object_add_to_smart(widget, child->smart_object, (child->clip == NULL));
   }

   if (widget->visible && (ETK_IS_TOPLEVEL(widget)
         || (widget->parent && widget->parent->smart_object && evas_object_visible_get(widget->parent->smart_object))))
      evas_object_show(widget->smart_object);
   else
      evas_object_hide(widget->smart_object);

   evas_object_propagate_events_set(widget->smart_object, 0);
   evas_object_pass_events_set(widget->smart_object, (widget->pass_mouse_events || widget->disabled));

   /* Then, we create the theme-object */
   widget->theme_object = edje_object_add(evas);
   if (etk_theme_edje_object_set(widget->theme_object, etk_widget_theme_file_get(widget), widget->theme_group_full, NULL))
   {
      if (etk_widget_theme_data_get(widget, "inset", "%d %d %d %d",
         &widget->inset.left, &widget->inset.right, &widget->inset.top, &widget->inset.bottom) != 4)
      {
         widget->inset.left = 0;
         widget->inset.right = 0;
         widget->inset.top = 0;
         widget->inset.bottom = 0;
      }
      evas_object_show(widget->theme_object);
      _etk_widget_object_add_to_smart(widget, widget->theme_object, ETK_TRUE);
      evas_object_lower(widget->theme_object);

      /* We create the content-object if the part ETK_WIDGET_CONTENT_PART exists:
       * the content-object is a smart object that will be swallowed in this part, and
       * whose member-objects are the member-objects and children's smart-objects of the widget.
       * This way, we have a powerful and generic way to swallow children in any type of widgets */
      if (edje_object_part_exists(widget->theme_object, ETK_WIDGET_CONTENT_PART))
      {
         widget->content_object = _etk_widget_content_object_add(evas, widget);
         evas_object_show(widget->content_object);
         edje_object_part_swallow(widget->theme_object, ETK_WIDGET_CONTENT_PART, widget->content_object);

         widget->inset.left = 0;
         widget->inset.right = 0;
         widget->inset.top = 0;
         widget->inset.bottom = 0;
      }

      if (widget->disabled)
         etk_widget_theme_signal_emit(widget, "etk,state,disabled", ETK_FALSE);
      if (etk_widget_is_focused(widget))
         etk_widget_theme_signal_emit(widget, "etk,state,focused", ETK_FALSE);
   }
   else
   {
      evas_object_del(widget->theme_object);
      widget->theme_object = NULL;

      /* And we create the event-object if necessary */
      if (widget->has_event_object)
      {
         widget->event_object = evas_object_rectangle_add(evas);
         evas_object_color_set(widget->event_object, 0, 0, 0, 0);
         evas_object_show(widget->event_object);
         _etk_widget_object_add_to_smart(widget, widget->event_object, ETK_TRUE);
         evas_object_lower(widget->event_object);
      }
   }

   /* Finally, we clip the widget and we set the color of the smart object */
   if (widget->clip)
      evas_object_clip_set(widget->smart_object, widget->clip);
   evas_object_color_set(widget->smart_object, widget->color.r, widget->color.g, widget->color.b, widget->color.a);

   widget->need_theme_size_recalc = ETK_TRUE;
   widget->realized = ETK_TRUE;
   etk_signal_emit(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(widget));
   etk_widget_size_recalc_queue(widget);
}

/* TODO: Fix emission of "etk,state,shown" */

/* Unrealizes the widget: it will unload the theme and free the graphical ressources */
static void _etk_widget_unrealize(Etk_Widget *widget)
{
   Etk_Widget_Member_Object *m;
   Etk_Widget_Swallowed_Object *swo;
   Evas_Object *object;
   Etk_Bool delete_obj;

   if (!widget || !widget->realized)
      return;

   etk_signal_emit(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(widget));

   while (widget->swallowed_objects)
   {
      swo = widget->swallowed_objects->data;
      object = swo->object;
      delete_obj = (swo->widget == NULL);
      _etk_widget_unswallow_full(widget, widget->swallowed_objects);
      if (delete_obj)
         evas_object_del(object);
   }
   while (widget->member_objects)
   {
      m = widget->member_objects->data;
      object = m->object;
      etk_widget_member_object_del(widget, object);
      evas_object_del(object);
   }
   if (widget->content_object)
   {
      evas_object_del(widget->content_object);
      widget->content_object = NULL;
   }
   if (widget->theme_object)
   {
      evas_object_del(widget->theme_object);
      widget->theme_object = NULL;
   }
   if (widget->event_object)
   {
      evas_object_del(widget->event_object);
      widget->event_object = NULL;
   }
   if (widget->smart_object)
   {
      evas_object_del(widget->smart_object);
      widget->smart_object = NULL;
   }

   widget->inset.left = 0;
   widget->inset.right = 0;
   widget->inset.top = 0;
   widget->inset.bottom = 0;

   widget->realized = ETK_FALSE;
}

/* Updates the full theme-group string of the widget and of its theme-children */
static void _etk_widget_theme_group_full_update(Etk_Widget *widget)
{
   Eina_List *l;
   char *parent_group;

   if (!widget)
      return;

   free(widget->theme_group_full);
   if (widget->theme_group && widget->theme_group[0] != '\0')
   {
      if (widget->theme_parent && (parent_group = widget->theme_parent->theme_group_full) && parent_group[0] != '\0')
      {
         widget->theme_group_full = malloc(strlen(parent_group) + strlen(widget->theme_group) + 2);
         sprintf(widget->theme_group_full, "%s/%s", parent_group, widget->theme_group);
      }
      else
         widget->theme_group_full = strdup(widget->theme_group);
   }
   else
      widget->theme_group_full = NULL;

   /* Updates the full theme-group of the theme-children */
   for (l = widget->theme_children; l; l = l->next)
      _etk_widget_theme_group_full_update(ETK_WIDGET(l->data));
}

/* Sets recursively the toplevel parent of the widget and of its children. Used by etk_widget_parent_set() */
static void _etk_widget_toplevel_parent_set(Etk_Widget *widget, Etk_Toplevel *toplevel_parent)
{
   Eina_List *l;

   if (!widget)
      return;

   if (etk_widget_is_focused(widget))
      etk_widget_unfocus(widget);
   widget->toplevel_parent = toplevel_parent;

   for (l = widget->children; l; l = l->next)
      _etk_widget_toplevel_parent_set(ETK_WIDGET(l->data), toplevel_parent);
}

/* Realizes the widget and all its children */
static void _etk_widget_realize_children(Etk_Widget *widget, Etk_Bool realize, Etk_Bool evas_changed)
{
   Eina_List *l;
   Etk_Widget *child;

   if (!widget || !etk_widget_toplevel_evas_get(widget))
      return;

   if (realize)
      _etk_widget_realize(widget);
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (evas_changed || (!child->theme_parent && !child->theme_file))
         _etk_widget_realize_children(child, ETK_TRUE, evas_changed);
   }
}

/* Realizes the widget and all its theme-children */
static void _etk_widget_realize_theme_children(Etk_Widget *widget, Etk_Bool realize, Etk_Bool theme_group_changed)
{
   Eina_List *l;
   Etk_Widget *child;

   if (!widget || !etk_widget_toplevel_evas_get(widget))
      return;

   if (realize)
      _etk_widget_realize(widget);
   for (l = widget->theme_children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (theme_group_changed || !child->theme_file)
         _etk_widget_realize_theme_children(child, ETK_TRUE, theme_group_changed);
   }
}

/* Unrealizes the widget and all its children */
static void _etk_widget_unrealize_all(Etk_Widget *widget)
{
   Eina_List *l;

   if (!widget)
      return;

   _etk_widget_unrealize(widget);
   for (l = widget->children; l; l = l->next)
      _etk_widget_unrealize_all(ETK_WIDGET(l->data));
}

/* Calculates the minimum size of the theme-object of "widget" */
static void _etk_widget_theme_min_size_calc(Etk_Widget *widget, int *w, int *h, Etk_Bool hidden_has_no_size)
{
   if (!widget)
      return;

   if (!widget->visible && hidden_has_no_size)
   {
      if (w)   *w = 0;
      if (h)   *h = 0;
      return;
   }

   if (widget->need_theme_size_recalc)
   {
      if (widget->theme_object)
      {
         int min_calc_width, min_calc_height;
         int min_get_width, min_get_height;
         Eina_List *l;
         Etk_Widget_Swallowed_Object *swallowed_object;

         /* Calculate and set the min size of the swallowed objects */
         for (l = widget->swallowed_objects; l; l = l->next)
         {
            swallowed_object = l->data;
            if (swallowed_object->widget)
            {
               Etk_Size swallow_size;

               etk_widget_size_request(swallowed_object->widget, &swallow_size);
               edje_extern_object_min_size_set(swallowed_object->object, swallow_size.w, swallow_size.h);
            }
         }
         /* Calculate and set the min size of the content-object */
         if (widget->content_object)
         {
            Etk_Size size_request;

            /* TODO: use etk_widget_size_request() here? */
            size_request = widget->requested_size;
            if ((size_request.w < 0 || size_request.h < 0) && widget->size_request)
            {
               Etk_Size calced_size;

               widget->size_request(widget, &calced_size);
               if (size_request.w < 0)
                  size_request.w = calced_size.w;
               if (size_request.h < 0)
                  size_request.h = calced_size.h;
            }
            size_request.w = ETK_MAX(size_request.w, 0);
            size_request.h = ETK_MAX(size_request.h, 0);
            edje_extern_object_min_size_set(widget->content_object, size_request.w, size_request.h);
         }

         /* Calculate the min size of the theme-object */
         edje_object_message_signal_process(widget->theme_object);
         edje_object_size_min_calc(widget->theme_object, &min_calc_width, &min_calc_height);
         edje_object_size_min_get(widget->theme_object, &min_get_width, &min_get_height);
         widget->theme_min_size.w = ETK_MAX(min_calc_width, min_get_width);
         widget->theme_min_size.h = ETK_MAX(min_calc_height, min_get_height);

         /* And finally, reset the min size of the swallowed objects and of the content-object to 0x0 */
         for (l = widget->swallowed_objects; l; l = l->next)
         {
            swallowed_object = l->data;
            if (swallowed_object->widget)
               edje_extern_object_min_size_set(swallowed_object->object, 0, 0);
         }
         if (widget->content_object)
            edje_extern_object_min_size_set(widget->content_object, 0, 0);
      }
      else
      {
         widget->theme_min_size.w = 0;
         widget->theme_min_size.h = 0;
      }

      widget->theme_min_size.w = ETK_MAX(widget->theme_min_size.w, widget->inset.left + widget->inset.right);
      widget->theme_min_size.h = ETK_MAX(widget->theme_min_size.h, widget->inset.top + widget->inset.bottom);
      widget->need_theme_size_recalc = ETK_FALSE;
   }

   if (w)   *w = widget->theme_min_size.w;
   if (h)   *h = widget->theme_min_size.h;
}

/* Marks recursively all the children of "widget" as needing a redraw. Used by etk_widget_redraw_queue() */
static void _etk_widget_redraw_queue_recursive(Etk_Widget *widget)
{
   Eina_List *l;

   if (!widget)
      return;

   widget->need_redraw = ETK_TRUE;
   for (l = widget->children; l; l = l->next)
      _etk_widget_redraw_queue_recursive(ETK_WIDGET(l->data));
}

/* Makes the theme-object of the widget swallow a widget or an object into one of its part */
static void _etk_widget_swallow_full(Etk_Widget *swallower, const char *part, Evas_Object *object, Etk_Widget *widget)
{
   Etk_Widget *prev_widget;
   Evas_Object *prev_swallowed;
   Etk_Widget_Swallowed_Object *swo;

   if (!swallower || !part || !swallower->theme_object || (!widget && !object))
      return;

   if ((prev_swallowed = edje_object_part_swallow_get(swallower->theme_object, part)))
      etk_widget_unswallow_object(swallower, prev_swallowed);

   if (object)
   {
      if ((prev_widget = ETK_WIDGET(evas_object_data_get(object, "_Etk_Widget::Parent"))))
         etk_widget_member_object_del(prev_widget, object);
      else if ((prev_widget = ETK_WIDGET(evas_object_data_get(object, "_Etk_Widget::Swallower"))))
         etk_widget_unswallow_object(prev_widget, object);
   }

   swo = malloc(sizeof(Etk_Widget_Swallowed_Object));
   swo->object = object;
   swo->part = strdup(part);
   swo->widget = widget;
   swallower->swallowed_objects = eina_list_append(swallower->swallowed_objects, swo);

   if (object)
   {
      edje_object_part_swallow(swallower->theme_object, part, object);
      evas_object_event_callback_add(object, EVAS_CALLBACK_FREE, _etk_widget_swallowed_object_deleted_cb, swallower);
      evas_object_data_set(object, "_Etk_Widget::Swallower", swallower);
      etk_widget_size_recalc_queue(swallower);
   }
   else
      etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(widget), ETK_CALLBACK(_etk_widget_swallowed_widget_realized_cb), NULL);
}

/* Makes the theme-object of the widget unswallow an object */
static void _etk_widget_unswallow_full(Etk_Widget *swallower, Eina_List *swo_node)
{
   Evas_Object *object;
   Etk_Widget_Swallowed_Object *swo;

   if (!swallower || !swo_node)
      return;

   swo = swo_node->data;
   if ((object = swo->object))
   {
      edje_object_part_unswallow(swallower->theme_object, object);
      evas_object_event_callback_del(object, EVAS_CALLBACK_FREE, _etk_widget_swallowed_object_deleted_cb);
      evas_object_data_del(object, "_Etk_Widget::Swallower");
   }

   free(swo->part);
   free(swo);
   swallower->swallowed_objects = eina_list_remove_list(swallower->swallowed_objects, swo_node);

   etk_widget_size_recalc_queue(swallower);
}

/* Adds an object to the widget smart object */
static void _etk_widget_object_add_to_smart(Etk_Widget *widget, Evas_Object *object, Etk_Bool clip)
{
   Evas_Object *parent;
   Evas_Object *clip_obj;

   if (!widget || !object)
      return;
   if (!(parent = widget->content_object ? widget->content_object : widget->smart_object))
      return;

   if (!evas_object_visible_get(parent))
      evas_object_hide(object);
   if (clip && (clip_obj = evas_object_clip_get(parent)))
      evas_object_clip_set(object, clip_obj);
   evas_object_smart_member_add(object, parent);

   evas_object_raise(object);
}

/* Adds "widget" to the list of widgets clipped by "clip" */
static void _etk_widget_add_to_clip(Etk_Widget *widget, Evas_Object *clip)
{
   Eina_List *clipped_widgets;

   if (!widget || !clip)
      return;

   if (!(clipped_widgets = evas_object_data_get(clip, "_Etk_Widget::Clipped_Widgets")))
   {
      clipped_widgets = eina_list_append(NULL, widget);
      evas_object_event_callback_add(clip, EVAS_CALLBACK_FREE, _etk_widget_clip_deleted_cb, NULL);
      evas_object_data_set(clip, "_Etk_Widget::Clipped_Widgets", clipped_widgets);
   }
   else if (!eina_list_data_find(clipped_widgets, widget))
      eina_list_append(clipped_widgets, widget);
}

/* Removes "widget" from the list of widgets clipped by "clip" */
static void _etk_widget_remove_from_clip(Etk_Widget *widget, Evas_Object *clip)
{
   Eina_List *clipped_widgets;

   if (!widget || !clip)
      return;

   if ((clipped_widgets = evas_object_data_get(widget->clip, "_Etk_Widget::Clipped_Widgets")))
   {
      Eina_List *widget_node;
      Etk_Bool need_update;

      if ((widget_node = eina_list_data_find_list(clipped_widgets, widget)))
      {
         need_update = (clipped_widgets == widget_node);
         clipped_widgets = eina_list_remove_list(clipped_widgets, widget_node);

         if (!clipped_widgets)
         {
            evas_object_data_del(widget->clip, "_Etk_Widget::Clipped_Widgets");
            evas_object_event_callback_del(widget->clip, EVAS_CALLBACK_FREE, _etk_widget_clip_deleted_cb);
         }
         else if (need_update)
            evas_object_data_set(widget->clip, "_Etk_Widget::Clipped_Widgets", clipped_widgets);
      }
   }
}

/* Calculates the real color of the widget according to the color of its parents */
static void _etk_widget_real_color_get(Etk_Widget *widget, int *r, int *g, int *b, int *a)
{
   Etk_Widget *parent;

   if (!widget)
      return;

   *r = widget->color.r;
   *g = widget->color.g;
   *b = widget->color.b;
   *a = widget->color.a;
   for (parent = widget->parent; parent && parent->propagate_color; parent = parent->parent)
   {
      *r = (*r * parent->color.r) / 255;
      *g = (*g * parent->color.g) / 255;
      *b = (*b * parent->color.b) / 255;
      *a = (*a * parent->color.a) / 255;
   }
}

/* Finds if an evas object is a member-object of the widget */
static Eina_List *_etk_widget_member_object_find(Etk_Widget *widget, Evas_Object *object)
{
   Eina_List *l;
   Etk_Widget_Member_Object *m;

   if (!widget || !object)
      return NULL;

   for (l = widget->member_objects; l; l = l->next)
   {
      m = l->data;
      if (m->object == object)
         return l;
   }
   return NULL;
}

/* Called when a member-object requests to be shown. Shows it only if all the parents are visible */
static void _etk_widget_member_object_intercept_show_cb(void *data, Evas_Object *obj)
{
   Etk_Widget *widget;
   Eina_List *l;
   Etk_Widget_Member_Object *member_object;
   Evas_Object *parent;

   if (!_etk_widget_intercept_show_hide)
   {
      evas_object_show(obj);
      return;
   }

   if (!(widget = ETK_WIDGET(data)))
      return;

   if ((l = _etk_widget_member_object_find(widget, obj)))
   {
      member_object = l->data;
      member_object->visible = ETK_TRUE;
   }

   parent = widget->content_object ? widget->content_object : widget->smart_object;
   if (parent && evas_object_visible_get(parent))
      evas_object_show(obj);
}


/* Called when a member-object of the widget requests to be hidden */
static void _etk_widget_member_object_intercept_hide_cb(void *data, Evas_Object *obj)
{
   Eina_List *l;
   Etk_Widget *widget;
   Etk_Widget_Member_Object *member_object;

   evas_object_hide(obj);

   if (!_etk_widget_intercept_show_hide || !(widget = ETK_WIDGET(data)))
      return;

   if ((l = _etk_widget_member_object_find(widget, obj)))
   {
      member_object = l->data;
      member_object->visible = ETK_FALSE;
   }
}

/* Called when a member-object of the widget is deleted */
static void _etk_widget_member_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(data)))
      return;
   etk_widget_member_object_del(widget, obj);
}

/* Called when the clip of the widget is deleted */
static void _etk_widget_clip_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eina_List *clipped_widgets, *l;
   Etk_Widget *widget;

   if (!obj || !(clipped_widgets = evas_object_data_get(obj, "_Etk_Widget::Clipped_Widgets")))
      return;

   for (l = clipped_widgets; l; l = l->next)
   {
      widget = ETK_WIDGET(l->data);
      widget->clip = NULL;
   }
}

/* Called when an object swallowed by the widget is deleted */
static void _etk_widget_swallowed_object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Widget *widget;
   Eina_List *l;
   Etk_Widget_Swallowed_Object *swo;

   if (!(widget = ETK_WIDGET(data)))
      return;

   for (l = widget->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->object == obj)
      {
         if (swo->widget)
         {
            swo->object = NULL;
            etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(swo->widget),
               ETK_CALLBACK(_etk_widget_swallowed_widget_realized_cb), NULL);
         }
         else
         {
            swo->object = NULL;
            _etk_widget_unswallow_full(widget, l);
         }

         break;
      }
   }
}

/* Called when a swallowed widget is realized */
static Etk_Bool _etk_widget_swallowed_widget_realized_cb(Etk_Object *object, void *data)
{
   Etk_Widget *swallower, *swallowed;
   Eina_List *l;
   Etk_Widget_Swallowed_Object *swo;
   Evas_Object *obj;

   if (!(swallowed = ETK_WIDGET(object)) || !(swallower = swallowed->parent))
      return ETK_TRUE;

   for (l = swallower->swallowed_objects; l; l = l->next)
   {
      swo = l->data;
      if (swo->widget == swallowed)
      {
         if ((obj = swallowed->smart_object))
         {
            swo->object = obj;
            edje_object_part_swallow(swallower->theme_object, swo->part, obj);
            evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, _etk_widget_swallowed_object_deleted_cb, swallower);
            etk_widget_size_recalc_queue(swallower);
         }
         break;
      }
   }

   etk_signal_disconnect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(swallowed), ETK_CALLBACK(_etk_widget_swallowed_widget_realized_cb), NULL);
   return ETK_TRUE;
}

/**************************
 *
 * Etk_Widget's smart-object
 *
 **************************/

/* Creates a new smart object for the widget */
static Evas_Object *_etk_widget_smart_object_add(Evas *evas, Etk_Widget *widget)
{
   Evas_Object *new_object;

   if (!evas || !widget)
      return NULL;

   if (!_etk_widget_smart_object_smart)
   {
      static const Evas_Smart_Class sc =
      {
         "Widget_Object",
         EVAS_SMART_CLASS_VERSION,
         NULL,
         NULL,
         _etk_widget_smart_object_move_cb, /* move */
         _etk_widget_smart_object_resize_cb, /* resize */
         _etk_widget_smart_object_show_cb, /* show */
         _etk_widget_smart_object_hide_cb, /* hide */
         _etk_widget_smart_object_color_set_cb, /* color_set */
         _etk_widget_smart_object_clip_set_cb, /* clip_set */
         _etk_widget_smart_object_clip_unset_cb, /* clip_unset */
         NULL,
         NULL,
         NULL,
         NULL
      };
      _etk_widget_smart_object_smart = evas_smart_class_new(&sc);
   }

   new_object = evas_object_smart_add(evas, _etk_widget_smart_object_smart);
   evas_object_smart_data_set(new_object, widget);
   return new_object;
}

/* Called when the smart object is moved */
static void _etk_widget_smart_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Widget *widget, *child;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   x += widget->padding.left;
   y += widget->padding.top;

   if (x != widget->geometry.x || y != widget->geometry.y)
   {
      Eina_List *l;
      Etk_Widget_Member_Object *m;
      Evas_Coord child_x, child_y;
      int x_offset, y_offset;

      x_offset = x - widget->geometry.x;
      y_offset = y - widget->geometry.y;
      widget->geometry.x = x;
      widget->geometry.y = y;
      if (!widget->content_object)
      {
         widget->inner_geometry.x = widget->geometry.x + widget->inset.left;
         widget->inner_geometry.y = widget->geometry.y + widget->inset.top;
      }

      if (widget->theme_object)
      {
         evas_object_geometry_get(widget->theme_object, &child_x, &child_y, NULL, NULL);
         _evas_object_smart_move(widget->theme_object, child_x + x_offset, child_y + y_offset);
      }
      if (widget->event_object)
         _evas_object_smart_move(widget->event_object, widget->geometry.x, widget->geometry.y);
      /* Move the member-objects and the children to the right place */
      if (!widget->content_object)
      {
         for (l = widget->member_objects; l; l = l->next)
         {
            m = l->data;
            evas_object_geometry_get(m->object, &child_x, &child_y, NULL, NULL);
            _evas_object_smart_move(m->object, child_x + x_offset, child_y + y_offset);
         }
         for (l = widget->children; l; l = l->next)
         {
            child = l->data;
            if (!child->swallowed)
            {
               evas_object_geometry_get(child->smart_object, &child_x, &child_y, NULL, NULL);
               _evas_object_smart_move(child->smart_object, child_x + x_offset, child_y + y_offset);
            }
         }
      }

      etk_object_notify(ETK_OBJECT(widget), "geometry");
   }
}

/* Called when the smart object is resized */
static void _etk_widget_smart_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Widget *widget;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   w -= widget->padding.left + widget->padding.right;
   h -= widget->padding.top + widget->padding.bottom;

   if (w != widget->geometry.w || h != widget->geometry.h || widget->need_redraw)
   {
      widget->geometry.w = w;
      widget->geometry.h = h;
      if (!widget->content_object)
      {
         widget->inner_geometry.x = widget->geometry.x + widget->inset.left;
         widget->inner_geometry.y = widget->geometry.y +  widget->inset.top;
         widget->inner_geometry.w = widget->geometry.w - widget->inset.left - widget->inset.right;
         widget->inner_geometry.h = widget->geometry.h - widget->inset.top - widget->inset.bottom;
      }

      if (widget->theme_object)
      {
         _evas_object_smart_move(widget->theme_object, widget->geometry.x, widget->geometry.y);
         _evas_object_smart_resize(widget->theme_object, widget->geometry.w, widget->geometry.h);
      }
      if (widget->event_object)
      {
         _evas_object_smart_move(widget->event_object, widget->geometry.x, widget->geometry.y);
         _evas_object_smart_resize(widget->event_object, widget->geometry.w, widget->geometry.h);
      }
      if ((!widget->content_object || widget->need_redraw) && widget->size_allocate)
         widget->size_allocate(widget, widget->inner_geometry);

      widget->need_redraw = ETK_FALSE;
      etk_object_notify(ETK_OBJECT(widget), "geometry");
   }
}

/* Called when the smart object is shown */
static void _etk_widget_smart_object_show_cb(Evas_Object *obj)
{
   Eina_List *l;
   Etk_Widget_Member_Object *m;
   Etk_Widget *widget, *child;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   if (widget->theme_object)
      evas_object_show(widget->theme_object);
   if (widget->event_object)
      evas_object_show(widget->event_object);
   for (l = widget->member_objects; l; l = l->next)
   {
      m = l->data;
      if (m->visible)
      {
         _etk_widget_intercept_show_hide = ETK_FALSE;
         evas_object_show(m->object);
         _etk_widget_intercept_show_hide = ETK_TRUE;
      }
   }
   for (l = widget->children; l; l = l->next)
   {
      child = l->data;
      if (child->visible)
         evas_object_show(child->smart_object);
   }
}

/* Called when the smart object is hidden */
static void _etk_widget_smart_object_hide_cb(Evas_Object *obj)
{
   Eina_List *l;
   Etk_Widget_Member_Object *m;
   Etk_Widget *widget, *child;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   if (widget->theme_object)
      evas_object_hide(widget->theme_object);
   if (widget->event_object)
      evas_object_hide(widget->event_object);
   for (l = widget->member_objects; l; l = l->next)
   {
      m = l->data;
      _etk_widget_intercept_show_hide = ETK_FALSE;
      evas_object_hide(m->object);
      _etk_widget_intercept_show_hide = ETK_TRUE;
   }
   for (l = widget->children; l; l = l->next)
   {
      child = l->data;
      if (!child->swallowed)
         evas_object_hide(child->smart_object);
   }
}

/* Called when the smart object's color is set */
static void _etk_widget_smart_object_color_set_cb(Evas_Object *obj, int r, int g, int b, int a)
{
   Eina_List *l;
   Etk_Widget_Member_Object *m;
   Etk_Widget *widget, *child;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   widget->color.r = r;
   widget->color.g = g;
   widget->color.b = b;
   widget->color.a = a;

   if (widget->theme_object)
   {
      _etk_widget_real_color_get(widget, &r, &g, &b, &a);
      evas_object_color_set(widget->theme_object, r, g, b, a);
   }

   for (l = widget->member_objects; l; l = l->next)
   {
      m = l->data;
      evas_object_color_set(m->object, widget->color.r, widget->color.g, widget->color.b, widget->color.a);
   }
   if (widget->propagate_color)
   {
      for (l = widget->children; l; l = l->next)
      {
         child = l->data;
         if (!child->swallowed)
         {
            evas_object_color_set(child->smart_object, widget->color.r,
                  widget->color.g, widget->color.b, widget->color.a);
         }
      }
   }

   etk_object_notify(ETK_OBJECT(widget), "color");
}


/* Called when a clip is set to the smart object */
static void _etk_widget_smart_object_clip_set_cb(Evas_Object *object, Evas_Object *clip)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   Etk_Widget_Member_Object *member_object;
   Eina_List *l;

   if (!object || !clip || !(widget = ETK_WIDGET(evas_object_smart_data_get(object))))
      return;

   if (widget->clip)
      etk_widget_clip_unset(widget);

   if (widget->theme_object)
      evas_object_clip_set(widget->theme_object, clip);
   if (widget->event_object)
      evas_object_clip_set(widget->event_object, clip);

   /* Clip the member-objects and the children */
   if (!widget->content_object)
   {
      for (l = widget->member_objects; l; l = l->next)
      {
         member_object = l->data;
         if (!evas_object_clip_get(member_object->object))
            evas_object_clip_set(member_object->object, clip);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = ETK_WIDGET(l->data);
         if (!child->swallowed && !etk_widget_clip_get(child))
            etk_widget_clip_set(child, clip);
      }
   }

   _etk_widget_add_to_clip(widget, clip);
   widget->clip = clip;
}

/* Called when the clip of the smart object is unset */
static void _etk_widget_smart_object_clip_unset_cb(Evas_Object *object)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   Etk_Widget_Member_Object *member_object;
   Eina_List *l;

   if (!object || !(widget = ETK_WIDGET(evas_object_smart_data_get(object))) || !widget->clip)
      return;

   if (widget->theme_object)
      evas_object_clip_unset(widget->theme_object);
   if (widget->event_object)
      evas_object_clip_unset(widget->event_object);

   /* Unclip the member-objects and the children */
   if (!widget->content_object)
   {
      for (l = widget->member_objects; l; l = l->next)
      {
         member_object = l->data;
         if (evas_object_clip_get(member_object->object) == widget->clip)
            evas_object_clip_unset(member_object->object);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = ETK_WIDGET(l->data);
         if (!child->swallowed && etk_widget_clip_get(child) == widget->clip)
            etk_widget_clip_unset(child);
      }
   }

   _etk_widget_remove_from_clip(widget, widget->clip);
   widget->clip = NULL;
}

/**************************
 *
 * Etk_Widget's content-object
 *
 **************************/

/* Creates a new content-object for the widget */
static Evas_Object *_etk_widget_content_object_add(Evas *evas, Etk_Widget *widget)
{
   Evas_Object *new_object;

   if (!evas || !widget)
      return NULL;

   if (!_etk_widget_content_object_smart)
   {
      static const Evas_Smart_Class sc =
      {
         "Content_Object",
         EVAS_SMART_CLASS_VERSION,
         NULL,
         NULL,
         _etk_widget_content_object_move_cb, /* move */
         _etk_widget_content_object_resize_cb, /* resize */
         NULL, /* show */
         NULL, /* hide */
         NULL, /* color_set */
         _etk_widget_content_object_clip_set_cb, /* clip_set */
         _etk_widget_content_object_clip_unset_cb, /* clip_unset */
         NULL,
         NULL,
         NULL,
         NULL
      };
      _etk_widget_content_object_smart = evas_smart_class_new(&sc);
   }

   new_object = evas_object_smart_add(evas, _etk_widget_content_object_smart);
   evas_object_smart_data_set(new_object, widget);

   return new_object;
}

/* Called when the content-object of the widget is moved */
static void _etk_widget_content_object_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Widget *widget;
   Evas_Coord prev_x, prev_y;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   widget->inner_geometry.x = x;
   widget->inner_geometry.y = y;

   evas_object_geometry_get(obj, &prev_x, &prev_y, NULL, NULL);
   if (prev_x != x || prev_y != y)
   {
      Etk_Widget *child;
      Etk_Widget_Member_Object *m;
      Eina_List *l;
      Evas_Coord child_x, child_y;
      int offset_x, offset_y;

      offset_x = x - prev_x;
      offset_y = y - prev_y;

      /* Move the member-objects and the children to the right place */
      for (l = widget->member_objects; l; l = l->next)
      {
         m = l->data;
         evas_object_geometry_get(m->object, &child_x, &child_y, NULL, NULL);
         _evas_object_smart_move(m->object, child_x + offset_x, child_y + offset_y);
      }
      for (l = widget->children; l; l = l->next)
      {
         child = l->data;
         if (!child->swallowed)
         {
            evas_object_geometry_get(child->smart_object, &child_x, &child_y, NULL, NULL);
            _evas_object_smart_move(child->smart_object, child_x + offset_x, child_y + offset_y);
         }
      }
   }
}

/* Called when the content-object of the widget is resized */
static void _etk_widget_content_object_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Widget *widget;
   Evas_Coord prev_w, prev_h;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   widget->inner_geometry.w = w;
   widget->inner_geometry.h = h;

   evas_object_geometry_get(obj, NULL, NULL, &prev_w, &prev_h);
   if ((prev_w != w || prev_h != h || widget->need_redraw) && widget->size_allocate)
      widget->size_allocate(widget, widget->inner_geometry);
}

/* Called when the content-object of the widget is clipped */
static void _etk_widget_content_object_clip_set_cb(Evas_Object *obj, Evas_Object *clip)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   Etk_Widget_Member_Object *member_object;
   Eina_List *l;

   if (!obj || !clip || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;

   /* Clip the member-objects and the children */
   for (l = widget->member_objects; l; l = l->next)
   {
      member_object = l->data;
      if (!evas_object_clip_get(member_object->object))
         evas_object_clip_set(member_object->object, clip);
   }
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (!child->swallowed && !etk_widget_clip_get(child))
         etk_widget_clip_set(child, clip);
   }
}

/* Called when the content-object of the widget is unclipped */
static void _etk_widget_content_object_clip_unset_cb(Evas_Object *obj)
{
   Etk_Widget *widget;
   Etk_Widget *child;
   Etk_Widget_Member_Object *member_object;
   Evas_Object *prev_clip;
   Eina_List *l;

   if (!obj || !(widget = ETK_WIDGET(evas_object_smart_data_get(obj))))
      return;
   if (!(prev_clip = evas_object_clip_get(obj)))
      return;

   /* Unclip the member-objects and the children */
   for (l = widget->member_objects; l; l = l->next)
   {
      member_object = l->data;
      if (evas_object_clip_get(member_object->object) == prev_clip)
         evas_object_clip_unset(member_object->object);
   }
   for (l = widget->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (!child->swallowed && etk_widget_clip_get(child) == prev_clip)
         etk_widget_clip_unset(child);
   }
}

/* Moves an Evas object and call the "move" smart-method even if the object has not been moved */
static void _evas_object_smart_move(Evas_Object *object, Evas_Coord x, Evas_Coord y)
{
   Evas_Smart *smart;
   const Evas_Smart_Class *sc;
   Evas_Coord prev_x, prev_y;
   
   if (!object)
      return;
   
   evas_object_geometry_get(object, &prev_x, &prev_y, NULL, NULL);
   evas_object_move(object, x, y);
   if (x == prev_x && y == prev_y
         && (smart = evas_object_smart_smart_get(object)) && (sc = evas_smart_class_get(smart)) && sc->move)
      sc->move(object, x, y);
}

/* Resizes an Evas object and call the "resize" smart-method even if the object has not been resized */
static void _evas_object_smart_resize(Evas_Object *object, Evas_Coord w, Evas_Coord h)
{
   Evas_Smart *smart;
   const Evas_Smart_Class *sc;
   Evas_Coord prev_w, prev_h;
   
   if (!object)
      return;
   
   evas_object_geometry_get(object, NULL, NULL, &prev_w, &prev_h);
   evas_object_resize(object, w, h);
   if (w == prev_w && h == prev_h
         && (smart = evas_object_smart_smart_get(object)) && (sc = evas_smart_class_get(smart)) && sc->resize)
      sc->resize(object, w, h);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Widget
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *
 * \par Signals:
 * @signal_name "shown": Emitted when the widget is shown
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been shown
 * @signal_data
 * \par
 * @signal_name "hidden": Emitted when the widget is hidden
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been hidden
 * @signal_data
 * \par
 * @signal_name "realized": Emitted when the widget is realized (i.e. when the graphical resources of
 * the widget are allocated - when the widget is attached to an Evas, or when its theme is changed)
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been realized
 * @signal_data
 * \par
 * @signal_name "unrealized": Emitted when the widget is unrealized (i.e. when its graphical resources are deallocated)
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been unrealized
 * @signal_data
 * \par
 * @signal_name "size-requested": Emitted each time the function etk_widget_size_request() is called on a widget
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Size *size, void *data)
 * @signal_arg widget: the widget whose size has been requested
 * @signal_arg size: The result of the call to etk_widget_size_request()
 * @signal_data
 * \par
 * @signal_name "mouse-in": Emitted when the mouse enters the widget
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Mouse_In *event, void *data)
 * @signal_arg widget: the widget that the mouse has entered
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse-out": Emitted when the mouse leaves the widget
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Mouse_Out *event, void *data)
 * @signal_arg widget: the widget that the mouse has left
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse-move": Emitted when the mouse moves over the widget
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Mouse_Move *event, void *data)
 * @signal_arg widget: the widget above which the mouse is moving
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse-down": Emitted when the user presses the widget with the mouse
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Mouse_Down *event, void *data)
 * @signal_arg widget: the widget that has been pressed
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse-up": Emitted when the user releases the widget with the mouse
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data)
 * @signal_arg widget: the widget that has been released
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse-click": Emitted when the user has clicked on the widget (i.e. the mouse button has been released
 * and the mouse is still above the widget)
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Mouse_Up *event, void *data)
 * @signal_arg widget: the widget that has been clicked
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "mouse-wheel": Emitted when the mouse wheel is used over the widget
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Mouse_Wheel *event, void *data)
 * @signal_arg widget: the widget above which the mouse wheel has been used
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "key-down": Emitted on the focused widget when a key has been pressed
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Key_Down *event, void *data)
 * @signal_arg widget: the focused widget
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "key-up": Emitted on the focused widget when a key has been released
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Event_Key_Up *event, void *data)
 * @signal_arg widget: the focused widget
 * @signal_arg event: The event data
 * @signal_data
 * \par
 * @signal_name "entered": Emitted when the widget is entered (not necessarily with the mouse)
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been entered
 * @signal_data
 * \par
 * @signal_name "left": Emitted when the widget is left (not necessarily with the mouse)
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been left
 * @signal_data
 * \par
 * @signal_name "focused": Emitted when the widget is focused
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been focused
 * @signal_data
 * \par
 * @signal_name "unfocused": Emitted when the widget is unfocused
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget that has been unfocused
 * @signal_data
 * \par
 * @signal_name "scroll-size-changed": Emitted when the scroll-size of the widget has changed
 * (only for widgets with a scrolling abilty)
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, void *data)
 * @signal_arg widget: the widget whose scroll-size has changed
 * @signal_data
 * \par
 * @signal_name "selection-received": Emitted when a selection has been received (after a clipboard request for example)
 * @signal_cb Etk_Bool callback(Etk_Widget *widget, Etk_Selection_Event *event, void *data)
 * @signal_arg widget: the widget whose scroll-size has changed
 * @signal_arg event: the selection data
 * @signal_data
 *
 * (TODOC: dnd signals)
 *
 * \par Properties:
 * @prop_name "parent": The parent of the widget
 * @prop_type Pointer (Etk_Widget *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "theme-file": The path to the theme-file used by the widget (NULL if the widget used
 * the default theme-file)
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "theme-group": The theme-group used by the widget
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "theme-parent": The theme-parent of the widget
 * @prop_type Pointer (Etk_Widget *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "geometry": The geometry of the widget (use etk_widget_geometry_get() to get it)
 * @prop_type Other (Etk_Geometry)
 * @prop_na
 * \par
 * @prop_name "padding": The padding on the different sides of the widget
 * (see etk_widget_padding_set() and etk_widget_padding_get())
 * @prop_type Other (quadruplets of integers)
 * @prop_na
 * \par
 * @prop_name "requested-width": The width requested for the widget (-1 means it's calculated automatically)
 * @prop_type Integer
 * @prop_rw
 * @prop_val -1
 * \par
 * @prop_name "requested-height": The height requested for the widget (-1 means it's calculated automatically)
 * @prop_type Integer
 * @prop_rw
 * @prop_val -1
 * \par
 * @prop_name "visible": Whether or not the widget is visible
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "color": The color of the widget
 * @prop_type Other (quadruplets of integers)
 * @prop_na
 * \par
 * @prop_name "propagate-color": Whether or not the color of the widget should propagated automatically to its children
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "internal": Whether or not the widget is an internal widget. See etk_widget_internal_set() for more info
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "focusable": Whether or not the widget can be focused
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "focus-on-click": Whether or not the widget should be automatically focused when it is clicked
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "repeat-mouse-events": Whether or not the mouse-events received by the widget should
 * be propagated to its parent
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "pass-mouse-events": Whether or not the mouse-events received by the widget should
 * be ignored by the widget and directly passed to its parent
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "has-event-object": Whether or not the widget uses an event-object to grab the mouse-events
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 */
