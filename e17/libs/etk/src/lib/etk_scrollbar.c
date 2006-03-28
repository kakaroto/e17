/** @file etk_scrollbar.c */
#include "etk_scrollbar.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
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
static void _etk_scrollbar_realize_cb(Etk_Object *object, void *data);
static void _etk_scrollbar_drag_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_scrollbar_value_changed_handler(Etk_Range *range, double value);
static void _etk_scrollbar_page_size_changed_cb(Etk_Object *object, const char *property_name, void *data);
static void _etk_scrollbar_range_changed_cb(Etk_Object *object, const char *property_name, void *data);
static void _etk_scrollbar_mouse_wheel(Etk_Object *object, void *event, void *data);

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
 * @brief Gets the type of an Etk_Scrollbar
 * @return Returns the type on an Etk_Scrollbar
 */
Etk_Type *etk_scrollbar_type_get()
{
   static Etk_Type *scrollbar_type = NULL;

   if (!scrollbar_type)
   {
      scrollbar_type = etk_type_new("Etk_Scrollbar", ETK_RANGE_TYPE, sizeof(Etk_Scrollbar), ETK_CONSTRUCTOR(_etk_scrollbar_constructor), NULL);
   }

   return scrollbar_type;
}

/**
 * @brief Gets the type of an Etk_HScrollbar
 * @return Returns the type on an Etk_HScrollbar
 */
Etk_Type *etk_hscrollbar_type_get()
{
   static Etk_Type *hscrollbar_type = NULL;

   if (!hscrollbar_type)
   {
      hscrollbar_type = etk_type_new("Etk_HScrollbar", ETK_SCROLLBAR_TYPE, sizeof(Etk_HScrollbar), NULL, NULL);
   }

   return hscrollbar_type;
}

/**
 * @brief Creates a new horizontal scrollbar
 * @param lower the lower bound of the scrollbar
 * @param upper the upper bound of the scrollbar
 * @param value the initial value of the scrollbar
 * @param step_increment the step increment value. Used when the arrow of a scrollbar is clicked, or when the keyboard arrows are pressed (for a scale)
 * @param page_increment the page increment value. Used when the trough of a scrollbar is clicked, or when page up/down are pressed
 * @param page_size the page size value of the scrollbar: this value is used to know the size of the drag button of the scrollbar
 * @return Returns the new horizontal scrollbar widget
 */
Etk_Widget *etk_hscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size)
{
   return etk_widget_new(ETK_HSCROLLBAR_TYPE, "theme_group", "hscrollbar", "lower", lower, "upper", upper, "value", value,
      "step_increment", step_increment, "page_increment", page_increment, "page_size", page_size, NULL);
}

/**
 * @brief Gets the type of an Etk_VScrollbar
 * @return Returns the type on an Etk_VScrollbar
 */
Etk_Type *etk_vscrollbar_type_get()
{
   static Etk_Type *vscrollbar_type = NULL;

   if (!vscrollbar_type)
   {
      vscrollbar_type = etk_type_new("Etk_VScrollbar", ETK_SCROLLBAR_TYPE, sizeof(Etk_VScrollbar), NULL, NULL);
   }

   return vscrollbar_type;
}

/**
 * @brief Creates a new vertical scrollbar
 * @param lower the lower bound of the scrollbar
 * @param upper the upper bound of the scrollbar
 * @param value the initial value of the scrollbar
 * @param step_increment the step increment value. Used when the arrow of a scrollbar is clicked, or when the keyboard arrows are pressed (for a scale)
 * @param page_increment the page increment value. Used when the trough of a scrollbar is clicked, or when page up/down are pressed
 * @param page_size the page size value of the scrollbar: this value is used to know the size of the drag button of the scrollbar
 * @return Returns the new vertical scrollbar widget
 */
Etk_Widget *etk_vscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size)
{
   return etk_widget_new(ETK_VSCROLLBAR_TYPE, "theme_group", "vscrollbar", "lower", lower, "upper", upper, "value", value,
      "step_increment", step_increment, "page_increment", page_increment, "page_size", page_size, NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the scrollbar */
static void _etk_scrollbar_constructor(Etk_Scrollbar *scrollbar)
{
   if (!scrollbar)
      return;

   scrollbar->scrolling_timer = NULL;
   scrollbar->first_scroll = ETK_FALSE;

   scrollbar->dragging = ETK_FALSE;
   ETK_RANGE(scrollbar)->value_changed = _etk_scrollbar_value_changed_handler;
   etk_signal_connect("realize", ETK_OBJECT(scrollbar), ETK_CALLBACK(_etk_scrollbar_realize_cb), NULL);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(scrollbar), ETK_CALLBACK(_etk_scrollbar_mouse_wheel), NULL);
   etk_object_notification_callback_add(ETK_OBJECT(scrollbar), "page_size", _etk_scrollbar_page_size_changed_cb, NULL);
   etk_object_notification_callback_add(ETK_OBJECT(scrollbar), "lower", _etk_scrollbar_range_changed_cb, NULL);
   etk_object_notification_callback_add(ETK_OBJECT(scrollbar), "upper", _etk_scrollbar_range_changed_cb, NULL);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the hscrollbar is realized */
static void _etk_scrollbar_realize_cb(Etk_Object *object, void *data)
{
   Evas_Object *theme_object;

   if (!object || !(theme_object = ETK_WIDGET(object)->theme_object))
      return;

   _etk_scrollbar_value_changed_handler(ETK_RANGE(object), ETK_RANGE(object)->value);
   edje_object_signal_callback_add(theme_object, "drag*", "drag", _etk_scrollbar_drag_dragged_cb, object);
   edje_object_signal_callback_add(theme_object, "scroll_*_start", "", _etk_scrollbar_scroll_start_cb, object);
   edje_object_signal_callback_add(theme_object, "scroll_stop", "", _etk_scrollbar_scroll_stop_cb, object);
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
   
   if (ETK_IS_HSCROLLBAR(range))
      edje_object_part_drag_value_get(obj, "drag", &percent, NULL);
   else
      edje_object_part_drag_value_get(obj, "drag", NULL, &percent);
   etk_range_value_set(range, range->lower + percent * (range->upper - range->lower - range->page_size));
}

/* Called when the user wants to scroll the scrollbar with the mouse wheel */
static void _etk_scrollbar_mouse_wheel(Etk_Object *object, void *event, void *data)
{
   Etk_Range *scrollbar_range;
   Etk_Event_Mouse_Wheel *wheel_event;
   
   if (!(scrollbar_range = ETK_RANGE(object)) || !(wheel_event = event))
      return;
   etk_range_value_set(scrollbar_range, scrollbar_range->value + wheel_event->z * scrollbar_range->step_increment);
   etk_widget_key_event_propagation_stop();
}

/* Default handler for the "value_changed" signal of a scrollbar */
static void _etk_scrollbar_value_changed_handler(Etk_Range *range, double value)
{
   double percent;

   if (!range || !ETK_WIDGET(range)->theme_object)
      return;

   if (range->upper - range->page_size > range->lower)
      percent = ETK_CLAMP(value / (range->upper - range->lower - range->page_size), 0.0, 1.0);
   else
      percent = 0.0;

   if (!ETK_SCROLLBAR(range)->dragging)
   {
      if (ETK_IS_HSCROLLBAR(range))
         edje_object_part_drag_value_set(ETK_WIDGET(range)->theme_object, "drag", percent, 0.0);
      else
         edje_object_part_drag_value_set(ETK_WIDGET(range)->theme_object, "drag", 0.0, percent);
   }
}

/* Called when the page size of the scrollbar is changed */
static void _etk_scrollbar_page_size_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Range *range;
   Evas_Object *theme_object;
   double new_drag_size;

   if (!(range = ETK_RANGE(object)) || !(theme_object = ETK_WIDGET(range)->theme_object))
      return;

   new_drag_size = (double)range->page_size / (range->upper - range->lower);
   if (ETK_IS_HSCROLLBAR(range))
      edje_object_part_drag_size_set(theme_object, "drag", new_drag_size, 0.0);
   else
      edje_object_part_drag_size_set(theme_object, "drag", 0.0, new_drag_size);
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
      percent = ETK_CLAMP(range->value / (range->upper - range->lower - range->page_size), 0.0, 1.0);
   else
      percent = 0.0;
   if (ETK_IS_HSCROLLBAR(range))
      edje_object_part_drag_value_set(ETK_WIDGET(range)->theme_object, "drag", percent, 0.0);
   else
      edje_object_part_drag_value_set(ETK_WIDGET(range)->theme_object, "drag", 0.0, percent);
   
   /* Update the size of the drag button */
   new_drag_size = (double)range->page_size / (range->upper - range->lower);
   if (ETK_IS_HSCROLLBAR(range))
      edje_object_part_drag_size_set(theme_object, "drag", new_drag_size, 0.0);
   else
      edje_object_part_drag_size_set(theme_object, "drag", 0.0, new_drag_size);
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

   if (strcmp(emission, "scroll_left_start") == 0 || strcmp(emission, "scroll_up_start") == 0)
   {
      scrollbar->scrolling_timer = ecore_timer_add(0.0, _etk_scrollbar_step_decrement_cb, scrollbar);
      scrollbar->first_scroll = ETK_TRUE;
   }
   else if (strcmp(emission, "scroll_right_start") == 0 || strcmp(emission, "scroll_down_start") == 0)
   {
      scrollbar->scrolling_timer = ecore_timer_add(0.0, _etk_scrollbar_step_increment_cb, scrollbar);
      scrollbar->first_scroll = ETK_TRUE;
   }
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
