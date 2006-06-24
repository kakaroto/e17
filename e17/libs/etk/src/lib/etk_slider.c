/** @file etk_slider.c */
#include "etk_slider.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Slider
 * @{
 */

static void _etk_slider_constructor(Etk_Slider *slider);
static void _etk_slider_realize_cb(Etk_Object *object, void *data);
static void _etk_slider_key_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_slider_mouse_wheel(Etk_Object *object, void *event, void *data);
static void _etk_slider_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_slider_value_changed_handler(Etk_Range *range, double value);
static void _etk_slider_range_changed_cb(Etk_Object *object, const char *property_name, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Slider
 * @return Returns the type on an Etk_Slider
 */
Etk_Type *etk_slider_type_get()
{
   static Etk_Type *slider_type = NULL;

   if (!slider_type)
      slider_type = etk_type_new("Etk_Slider", ETK_RANGE_TYPE, sizeof(Etk_Slider), ETK_CONSTRUCTOR(_etk_slider_constructor), NULL);

   return slider_type;
}

/**
 * @brief Gets the type of an Etk_HSlider
 * @return Returns the type on an Etk_HSlider
 */
Etk_Type *etk_hslider_type_get()
{
   static Etk_Type *hslider_type = NULL;

   if (!hslider_type)
      hslider_type = etk_type_new("Etk_HSlider", ETK_SLIDER_TYPE, sizeof(Etk_HSlider), NULL, NULL);

   return hslider_type;
}

/**
 * @brief Creates a new horizontal slider
 * @return Returns the new horizontal slider widget
 */
Etk_Widget *etk_hslider_new(double lower, double upper, double value, double step_increment, double page_increment)
{
   return etk_widget_new(ETK_HSLIDER_TYPE, "theme_group", "hslider", "focusable", ETK_TRUE, "lower", lower, "upper", upper,
      "value", value, "step_increment", step_increment, "page_increment", page_increment, "focus_on_press", ETK_TRUE, NULL);
}

/**
 * @brief Gets the type of an Etk_VSlider
 * @return Returns the type on an Etk_VSlider
 */
Etk_Type *etk_vslider_type_get()
{
   static Etk_Type *vslider_type = NULL;

   if (!vslider_type)
      vslider_type = etk_type_new("Etk_VSlider", ETK_SLIDER_TYPE, sizeof(Etk_VSlider), NULL, NULL);

   return vslider_type;
}

/**
 * @brief Creates a new vertical slider
 * @return Returns the new vertical slider widget
 */
Etk_Widget *etk_vslider_new(double lower, double upper, double value, double step_increment, double page_increment)
{
   return etk_widget_new(ETK_VSLIDER_TYPE, "theme_group", "vslider", "focusable", ETK_TRUE, "lower", lower, "upper", upper,
      "value", value, "step_increment", step_increment, "page_increment", page_increment, NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the slider */
static void _etk_slider_constructor(Etk_Slider *slider)
{
   if (!slider)
      return;

   slider->dragging = ETK_FALSE;
   ETK_RANGE(slider)->value_changed = _etk_slider_value_changed_handler;
   etk_signal_connect("realize", ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_realize_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_key_down_cb), NULL);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_mouse_wheel), NULL);
   etk_object_notification_callback_add(ETK_OBJECT(slider), "lower", _etk_slider_range_changed_cb, NULL);
   etk_object_notification_callback_add(ETK_OBJECT(slider), "upper", _etk_slider_range_changed_cb, NULL);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the slider is realized */
static void _etk_slider_realize_cb(Etk_Object *object, void *data)
{
   Evas_Object *theme_object;

   if (!object || !(theme_object = ETK_WIDGET(object)->theme_object))
      return;

   _etk_slider_value_changed_handler(ETK_RANGE(object), ETK_RANGE(object)->value);
   edje_object_signal_callback_add(theme_object, "drag*", "drag", _etk_slider_cursor_dragged_cb, object);
}

/* Called when the user presses a key */
static void _etk_slider_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *key_event = event;
   Etk_Range *range;
   Etk_Bool propagate = ETK_FALSE;

   if (!(range = ETK_RANGE(object)))
      return;

   if (strcmp(key_event->key, "Right") == 0 || strcmp(key_event->key, "Down") == 0)
      etk_range_value_set(range, range->value + range->step_increment);
   else if (strcmp(key_event->key, "Left") == 0 || strcmp(key_event->key, "Up") == 0)
      etk_range_value_set(range, range->value - range->step_increment);
   else if (strcmp(key_event->key, "Home") == 0)
      etk_range_value_set(range, range->lower);
   else if (strcmp(key_event->key, "End") == 0)
      etk_range_value_set(range, range->upper);
   else if (strcmp(key_event->key, "Next") == 0)
      etk_range_value_set(range, range->value + range->page_increment);
   else if (strcmp(key_event->key, "Prior") == 0)
      etk_range_value_set(range, range->value - range->page_increment);
   else
      propagate = ETK_TRUE;
   
   if (!propagate)
      etk_widget_key_event_propagation_stop();
}

/* Called when the user wants to change the value the mouse wheel */
static void _etk_slider_mouse_wheel(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_Wheel *wheel_event = event;
   Etk_Range *slider_range;
   
   if (!(slider_range = ETK_RANGE(object)))
      return;
   
   etk_range_value_set(slider_range, slider_range->value + wheel_event->z * slider_range->step_increment);
   etk_widget_key_event_propagation_stop();
}

/* Called when the cursor of the slider is dragged */
static void _etk_slider_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Range *range;
   double v;

   if (!(range = ETK_RANGE(data)))
      return;

   if (strcmp(emission, "drag,start") == 0)
      ETK_SLIDER(range)->dragging = ETK_TRUE;
   else if (strcmp(emission, "drag,stop") == 0)
      ETK_SLIDER(range)->dragging = ETK_FALSE;
   else if (strcmp(emission, "drag") == 0)
   {
      if (ETK_IS_HSLIDER(range))
         edje_object_part_drag_value_get(obj, "drag", &v, NULL);
      else
         edje_object_part_drag_value_get(obj, "drag", NULL, &v);
      etk_range_value_set(range, range->lower + v * (range->upper - range->lower));
   }
}

/* Default handler for the "value_changed" signal */
static void _etk_slider_value_changed_handler(Etk_Range *range, double value)
{
   Etk_Slider *slider;
   Evas_Object *theme_object;
   double percent;

   if (!(slider = ETK_SLIDER(range)) || !(theme_object = ETK_WIDGET(slider)->theme_object))
      return;

   if (range->upper > range->lower)
      percent = ETK_CLAMP((value - range->lower) / (range->upper - range->lower - range->page_size), 0.0, 1.0);
   else
      percent = 0.0;
   
   if (!slider->dragging)
   {
      if (ETK_IS_HSLIDER(slider))
         edje_object_part_drag_value_set(theme_object, "drag", percent, 0.0);
      else
         edje_object_part_drag_value_set(theme_object, "drag", 0.0, percent);
   }
}

/* Called when the range of the slider is changed */
static void _etk_slider_range_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Range *range;
   Evas_Object *theme_object;
   double percent;

   if (!(range = ETK_RANGE(object)) || !(theme_object = ETK_WIDGET(range)->theme_object))
      return;

   /* Update the position of the drag button in the slider */
   if (range->upper - range->page_size > range->lower)
      percent = ETK_CLAMP((range->value - range->lower) / (range->upper - range->lower - range->page_size), 0.0, 1.0);
   else
      percent = 0.0;
   if (ETK_IS_HSLIDER(range))
      edje_object_part_drag_value_set(theme_object, "drag", percent, 0.0);
   else
      edje_object_part_drag_value_set(theme_object, "drag", 0.0, percent);
}

/** @} */
