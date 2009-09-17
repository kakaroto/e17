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

/** @file etk_scrollbar.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_scrollbar.h"

#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_event.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Scrollbar
 * @{
 */

#define ETK_SCROLLBAR_FIRST_DELAY 0.4
#define ETK_SCROLLBAR_REPEAT_DELAY 0.05

static void _etk_scrollbar_constructor(Etk_Scrollbar *scrollbar);
static Etk_Bool _etk_scrollbar_realized_cb(Etk_Object *object, void *data);
static void _etk_scrollbar_drag_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static Etk_Bool _etk_scrollbar_value_changed_handler(Etk_Range *range, double value);
static void _etk_scrollbar_page_size_changed_cb(Etk_Object *object, const char *property_name, void *data);
static void _etk_scrollbar_range_changed_cb(Etk_Object *object, const char *property_name, void *data);
static Etk_Bool _etk_scrollbar_mouse_wheel_cb(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data);

static void _etk_scrollbar_scroll_start_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_scrollbar_scroll_stop_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static int _etk_scrollbar_step_decrement_cb(void *data);
static int _etk_scrollbar_step_increment_cb(void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Scrollbar
 * @return Returns the type of an Etk_Scrollbar
 */
Etk_Type *etk_scrollbar_type_get(void)
{
   static Etk_Type *scrollbar_type = NULL;

   if (!scrollbar_type)
   {
      scrollbar_type = etk_type_new("Etk_Scrollbar", ETK_RANGE_TYPE,
         sizeof(Etk_Scrollbar),
         ETK_CONSTRUCTOR(_etk_scrollbar_constructor), NULL, NULL);
   }

   return scrollbar_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_HScrollbar
 * @return Returns the type on an Etk_HScrollbar
 */
Etk_Type *etk_hscrollbar_type_get(void)
{
   static Etk_Type *hscrollbar_type = NULL;

   if (!hscrollbar_type)
   {
      hscrollbar_type = etk_type_new("Etk_HScrollbar", ETK_SCROLLBAR_TYPE,
         sizeof(Etk_HScrollbar),
         NULL, NULL, NULL);
   }

   return hscrollbar_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_VScrollbar
 * @return Returns the type of an Etk_VScrollbar
 */
Etk_Type *etk_vscrollbar_type_get(void)
{
   static Etk_Type *vscrollbar_type = NULL;

   if (!vscrollbar_type)
   {
      vscrollbar_type = etk_type_new("Etk_VScrollbar", ETK_SCROLLBAR_TYPE,
         sizeof(Etk_VScrollbar), NULL, NULL, NULL);
   }

   return vscrollbar_type;
}

/**
 * @brief Creates a new horizontal scrollbar
 * @param lower the lower bound of the scrollbar
 * @param upper the upper bound of the scrollbar
 * @param value the initial value of the scrollbar
 * @param step_increment the step-increment value. This value is added or substracted to the current value of the
 * scrollbar when an arrow button of the scrollbar is clicked, or when the mouse wheel is used
 * @param page_increment the page-increment value. This value is added or substracted to the current value of the
 * scrollbar when the page up/down keys are pressed or when the user clicks on the trough of the scrollbar
 * @param page_size the page size value: this value will determine the size of the drag button of the scrollbar
 * @return Returns the new horizontal scrollbar widget
 */
Etk_Widget *etk_hscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size)
{
   return etk_widget_new(ETK_HSCROLLBAR_TYPE, "theme-group", "hscrollbar", "lower", lower, "upper", upper,
      "value", value, "step-increment", step_increment, "page-increment", page_increment, "page-size", page_size, NULL);
}

/**
 * @brief Creates a new vertical scrollbar
 * @param lower the lower bound of the scrollbar
 * @param upper the upper bound of the scrollbar
 * @param value the initial value of the scrollbar
 * @param step_increment the-step increment value. This value is added or substracted to the current value of the
 * scrollbar when an arrow button of the scrollbar is clicked, or when the mouse wheel is used
 * @param page_increment the page-increment value. This value is added or substracted to the current value of the
 * scrollbar when the page up/down keys are pressed or when the user clicks on the trough of the scrollbar
 * @param page_size the page size value: this value will determine the size of the drag button of the scrollbar
 * @return Returns the new vertical scrollbar widget
 */
Etk_Widget *etk_vscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size)
{
   return etk_widget_new(ETK_VSCROLLBAR_TYPE, "theme-group", "vscrollbar", "lower", lower, "upper", upper,
      "value", value, "step-increment", step_increment, "page-increment", page_increment, "page-size", page_size, NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the scrollbar */
static void _etk_scrollbar_constructor(Etk_Scrollbar *scrollbar)
{
   if (!scrollbar)
      return;

   scrollbar->scrolling_timer = NULL;
   scrollbar->first_scroll = ETK_FALSE;

   scrollbar->dragging = ETK_FALSE;
   ETK_RANGE(scrollbar)->value_changed_handler = _etk_scrollbar_value_changed_handler;
   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(scrollbar), ETK_CALLBACK(_etk_scrollbar_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_WHEEL_SIGNAL, ETK_OBJECT(scrollbar), ETK_CALLBACK(_etk_scrollbar_mouse_wheel_cb), NULL);
   etk_object_notification_callback_add(ETK_OBJECT(scrollbar), "page-size", _etk_scrollbar_page_size_changed_cb, NULL);
   etk_object_notification_callback_add(ETK_OBJECT(scrollbar), "lower", _etk_scrollbar_range_changed_cb, NULL);
   etk_object_notification_callback_add(ETK_OBJECT(scrollbar), "upper", _etk_scrollbar_range_changed_cb, NULL);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the hscrollbar is realized */
static Etk_Bool _etk_scrollbar_realized_cb(Etk_Object *object, void *data)
{
   Evas_Object *theme_object;

   if (!object || !(theme_object = ETK_WIDGET(object)->theme_object))
      return ETK_TRUE;

   _etk_scrollbar_range_changed_cb(object, NULL, NULL);
   edje_object_signal_callback_add(theme_object, "drag*", "etk.dragable.bar", _etk_scrollbar_drag_dragged_cb, object);
   /* TODO: change the signal name to "etk,action,scroll,down/up,start" and "etk,action,scroll,stop" */
   edje_object_signal_callback_add(theme_object, "scroll_*_start", "", _etk_scrollbar_scroll_start_cb, object);
   edje_object_signal_callback_add(theme_object, "scroll_stop", "", _etk_scrollbar_scroll_stop_cb, object);
   return ETK_TRUE;
}

/* Called when the drag button of the scrollbar is dragged */
static void _etk_scrollbar_drag_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Range *range;
   double percent;

   if (!(range = ETK_RANGE(data)))
      return;

   if (strcmp(emission, "drag,start") == 0)
      ETK_SCROLLBAR(range)->dragging = ETK_TRUE;
   else if (strcmp(emission, "drag,stop") == 0)
      ETK_SCROLLBAR(range)->dragging = ETK_FALSE;
   else if (strcmp(emission, "drag") == 0)
   {
      if (ETK_IS_HSCROLLBAR(range))
         edje_object_part_drag_value_get(obj, "etk.dragable.bar", &percent, NULL);
      else
         edje_object_part_drag_value_get(obj, "etk.dragable.bar", NULL, &percent);
      etk_range_value_set(range, range->lower + percent * (range->upper - range->lower - range->page_size));
   }
}

/* Called when the user wants to scroll the scrollbar with the mouse wheel */
static Etk_Bool _etk_scrollbar_mouse_wheel_cb(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data)
{
   Etk_Range *scrollbar_range;

   if (!(scrollbar_range = ETK_RANGE(object)))
      return ETK_TRUE;
   etk_range_value_set(scrollbar_range, scrollbar_range->value + event->z * scrollbar_range->step_increment);

   return ETK_FALSE;
}

/* Default handler for the "value-changed" signal of a scrollbar */
static Etk_Bool _etk_scrollbar_value_changed_handler(Etk_Range *range, double value)
{
   Etk_Scrollbar *scrollbar;
   Evas_Object *theme_object;
   double percent;

   if (!(scrollbar = ETK_SCROLLBAR(range)) || !(theme_object = ETK_WIDGET(range)->theme_object))
      return ETK_TRUE;

   if (range->upper - range->page_size > range->lower)
      percent = ETK_CLAMP((value - range->lower) / (range->upper - range->lower - range->page_size), 0.0, 1.0);
   else
      percent = 0.0;

   if (!scrollbar->dragging)
   {
      if (ETK_IS_HSCROLLBAR(scrollbar))
         edje_object_part_drag_value_set(theme_object, "etk.dragable.bar", percent, 0.0);
      else
         edje_object_part_drag_value_set(theme_object, "etk.dragable.bar", 0.0, percent);
   }

   return ETK_TRUE;
}

/* Called when the page size of the scrollbar is changed */
static void _etk_scrollbar_page_size_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Range *range;
   Evas_Object *theme_object;
   double new_drag_size;

   if (!(range = ETK_RANGE(object)) || !(theme_object = ETK_WIDGET(range)->theme_object))
      return;

   if (range->upper > range->lower)
      new_drag_size = (double)range->page_size / (range->upper - range->lower);
   else
      new_drag_size = 1.0;
   if (ETK_IS_HSCROLLBAR(range))
      edje_object_part_drag_size_set(theme_object, "etk.dragable.bar", new_drag_size, 0.0);
   else
      edje_object_part_drag_size_set(theme_object, "etk.dragable.bar", 0.0, new_drag_size);
}

/* Called when the range of the scrollbar is changed */
static void _etk_scrollbar_range_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Range *range;
   Evas_Object *theme_object;
   double new_drag_size;
   double percent;

   if (!(range = ETK_RANGE(object)) || !(theme_object = ETK_WIDGET(range)->theme_object))
      return;

   /* Update the position of the drag button in the scrollbar */
   if (range->upper - range->page_size > range->lower)
      percent = ETK_CLAMP((range->value - range->lower) / (range->upper - range->lower - range->page_size), 0.0, 1.0);
   else
      percent = 0.0;
   if (ETK_IS_HSCROLLBAR(range))
      edje_object_part_drag_value_set(theme_object, "etk.dragable.bar", percent, 0.0);
   else
      edje_object_part_drag_value_set(theme_object, "etk.dragable.bar", 0.0, percent);

   /* Update the size of the drag button */
   if (range->upper > range->lower)
      new_drag_size = (double)range->page_size / (range->upper - range->lower);
   else
      new_drag_size = 1.0;
   if (ETK_IS_HSCROLLBAR(range))
      edje_object_part_drag_size_set(theme_object, "etk.dragable.bar", new_drag_size, 0.0);
   else
      edje_object_part_drag_size_set(theme_object, "etk.dragable.bar", 0.0, new_drag_size);
}

/* Called when the user starts to press an arrow of the scrollbar */
static void _etk_scrollbar_scroll_start_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)) || !emission)
      return;

   if (scrollbar->scrolling_timer)
   {
      ecore_timer_del(scrollbar->scrolling_timer);
      scrollbar->scrolling_timer = NULL;
   }

   scrollbar->first_scroll = ETK_TRUE;
   if (strcmp(emission, "scroll_left_start") == 0 || strcmp(emission, "scroll_up_start") == 0)
      scrollbar->scrolling_timer = ecore_timer_add(0.0, _etk_scrollbar_step_decrement_cb, scrollbar);
   else if (strcmp(emission, "scroll_right_start") == 0 || strcmp(emission, "scroll_down_start") == 0)
      scrollbar->scrolling_timer = ecore_timer_add(0.0, _etk_scrollbar_step_increment_cb, scrollbar);
}

/* Called when the user stops pressing an arrow of the scrollbar */
static void _etk_scrollbar_scroll_stop_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)) || !scrollbar->scrolling_timer)
      return;

   ecore_timer_del(scrollbar->scrolling_timer);
   scrollbar->scrolling_timer = NULL;
}

/* A timer callback that increment the value of the range with the step value */
static int _etk_scrollbar_step_decrement_cb(void *data)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)))
      return 1;

   etk_range_value_set(ETK_RANGE(scrollbar), ETK_RANGE(scrollbar)->value - ETK_RANGE(scrollbar)->step_increment);
   if (scrollbar->first_scroll)
   {
      ecore_timer_interval_set(scrollbar->scrolling_timer, ETK_SCROLLBAR_FIRST_DELAY);
      scrollbar->first_scroll = ETK_FALSE;
   }
   else
      ecore_timer_interval_set(scrollbar->scrolling_timer, ETK_SCROLLBAR_REPEAT_DELAY);
   return 1;
}

/* A timer callback that decrement the value of the range with the step value */
static int _etk_scrollbar_step_increment_cb(void *data)
{
   Etk_Scrollbar *scrollbar;

   if (!(scrollbar = ETK_SCROLLBAR(data)))
      return 1;

   etk_range_value_set(ETK_RANGE(scrollbar), ETK_RANGE(scrollbar)->value + ETK_RANGE(scrollbar)->step_increment);
   if (scrollbar->first_scroll)
   {
      ecore_timer_interval_set(scrollbar->scrolling_timer, ETK_SCROLLBAR_FIRST_DELAY);
      scrollbar->first_scroll = ETK_FALSE;
   }
   else
      ecore_timer_interval_set(scrollbar->scrolling_timer, ETK_SCROLLBAR_REPEAT_DELAY);
   return 1;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Scrollbar
 *
 * @image html scrollbar.png
 * Etk_Scrollbar is an abstract class for two derived widgets: Etk_HScrollbar (a horizontal scrollbar)
 * and Etk_VScrollbar (a vertical scrollbar). @n
 * You usually do not need to use Etk_Scrollbar directly. In most of the cases, you can use Etk_Scrolled_View instead.
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Range
 *       - Etk_Scrollbar
 *         - Etk_HScrollbar
 *         - Etk_VScrollbar
 */
