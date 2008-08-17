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

/** @file etk_slider.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_slider.h"

#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_event.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Slider
 * @{
 */

#define UPDATE_DELAY 0.3

enum Etk_Slider_Property_Id
{
   ETK_SLIDER_LABEL_FORMAT_PROPERTY,
   ETK_SLIDER_INVERTED_PROPERTY,
   ETK_SLIDER_UPDATE_POLICY_PROPERTY
};

static void _etk_slider_constructor(Etk_Slider *slider);
static void _etk_slider_destructor(Etk_Slider *slider);
static void _etk_slider_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_slider_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static Etk_Bool _etk_slider_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_slider_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_slider_mouse_wheel_cb(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data);
static void _etk_slider_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static Etk_Bool _etk_slider_value_changed_handler(Etk_Range *range, double value);
static void _etk_slider_range_changed_cb(Etk_Object *object, const char *property_name, void *data);
static int _etk_slider_update_timer_cb(void *data);
static double _etk_slider_value_get_from_edje(Etk_Slider *slider);
static void _etk_slider_label_update(Etk_Slider *slider);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Slider
 * @return Returns the type of an Etk_Slider
 */
Etk_Type *etk_slider_type_get(void)
{
   static Etk_Type *slider_type = NULL;

   if (!slider_type)
   {
      slider_type = etk_type_new("Etk_Slider", ETK_RANGE_TYPE, sizeof(Etk_Slider),
         ETK_CONSTRUCTOR(_etk_slider_constructor), ETK_DESTRUCTOR(_etk_slider_destructor), NULL);

      etk_type_property_add(slider_type, "label-format", ETK_SLIDER_LABEL_FORMAT_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(slider_type, "inverted", ETK_SLIDER_INVERTED_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(slider_type, "update-policy", ETK_SLIDER_UPDATE_POLICY_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_SLIDER_CONTINUOUS));

      slider_type->property_set = _etk_slider_property_set;
      slider_type->property_get = _etk_slider_property_get;
   }

   return slider_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_HSlider
 * @return Returns the type of an Etk_HSlider
 */
Etk_Type *etk_hslider_type_get(void)
{
   static Etk_Type *hslider_type = NULL;

   if (!hslider_type)
      hslider_type = etk_type_new("Etk_HSlider", ETK_SLIDER_TYPE, sizeof(Etk_HSlider), NULL, NULL, NULL);

   return hslider_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_VSlider
 * @return Returns the type of an Etk_VSlider
 */
Etk_Type *etk_vslider_type_get(void)
{
   static Etk_Type *vslider_type = NULL;

   if (!vslider_type)
      vslider_type = etk_type_new("Etk_VSlider", ETK_SLIDER_TYPE, sizeof(Etk_VSlider), NULL, NULL, NULL);

   return vslider_type;
}

/**
 * @brief Creates a new horizontal slider
 * @param lower the lower bound of the slider
 * @param upper the upper bound of the slider
 * @param value the initial value of the slider
 * @param step_increment the step-increment value. This value is added or substracted to the current value of the
 * slider when an arrow key is pressed, or when the mouse wheel is used
 * @param page_increment the page-increment value. This value is added or substracted to the current value of the
 * slider when the page up/down keys are pressed
 * @return Returns the new horizontal slider widget
 */
Etk_Widget *etk_hslider_new(double lower, double upper, double value, double step_increment, double page_increment)
{
   return etk_widget_new(ETK_HSLIDER_TYPE, "theme-group", "hslider", "focusable", ETK_TRUE,
      "lower", lower, "upper", upper, "value", value, "step-increment", step_increment,
      "page-increment", page_increment, "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Creates a new vertical slider
 * @param lower the lower bound of the slider
 * @param upper the upper bound of the slider
 * @param value the initial value of the slider
 * @param step_increment the step-increment value. This value is added or substracted to the current value of the
 * slider when an arrow key is pressed, or when the mouse wheel is used
 * @param page_increment the page-increment value. This value is added or substracted to the current value of the
 * slider when the page up/down keys are pressed
 * @return Returns the new vertical slider widget
 */
Etk_Widget *etk_vslider_new(double lower, double upper, double value, double step_increment, double page_increment)
{
   return etk_widget_new(ETK_VSLIDER_TYPE, "theme-group", "vslider", "focusable", ETK_TRUE,
      "lower", lower, "upper", upper, "value", value, "step-increment", step_increment,
      "page-increment", page_increment, "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Sets the format of the label to display next to the slider
 * @param slider a slider
 * @param label_format the format of the slider's label. It uses the same format as printf(). Since the value is
 * a double, you have to use "%.2f" if you want to display the value with two digits for example. @n
 * NULL will hide the label
 */
void etk_slider_label_set(Etk_Slider *slider, const char *label_format)
{
   if (!slider || slider->format == label_format)
      return;

   free(slider->format);
   slider->format = label_format ? strdup(label_format) : NULL;

   etk_widget_theme_signal_emit(ETK_WIDGET(slider),
      label_format ? "etk,action,show,label" : "etk,action,hide,label", ETK_TRUE);
   _etk_slider_label_update(slider);

   etk_object_notify(ETK_OBJECT(slider), "label-format");
}

/**
 * @brief Gets the format of the label displayed next to the slider
 * @param slider a slider
 * @return Returns the format string of the slider's label (NULL means the label is hidden)
 */
const char *etk_slider_label_get(Etk_Slider *slider)
{
   if (!slider)
      return NULL;
   return slider->format;
}

/**
 * @brief Sets whether or not the slider is inverted. An inverted slider has its ends inverted: the maximum bound is
 * at the left end for an Etk_HSlider, or at the bottom end (for an Etk_VSlider).
 * @param slider a slider
 * @param inverted ETK_TRUE to invert the slider, ETK_FALSE otherwise
 */
void etk_slider_inverted_set(Etk_Slider *slider, Etk_Bool inverted)
{
   if (!slider || slider->inverted == inverted)
      return;

   slider->inverted = inverted;
   if (ETK_WIDGET(slider)->theme_object)
   {
      _etk_slider_value_changed_handler(ETK_RANGE(slider), ETK_RANGE(slider)->value);
      etk_widget_theme_signal_emit(ETK_WIDGET(slider),
         inverted ? "etk,state,inverted" : "etk,state,normal", ETK_FALSE);
   }
   etk_object_notify(ETK_OBJECT(slider), "inverted");
}

/**
 * @brief Gets whether or not the slider is inverted
 * @param slider a slider
 * @return Returns ETK_TRUE if the slider is inverted, ETK_FALSE otherwise
 */
Etk_Bool etk_slider_inverted_get(Etk_Slider *slider)
{
   if (!slider)
      return ETK_FALSE;
   return slider->inverted;
}

/**
 * @brief Sets the update-policy of the slider
 * @param slider
 * @param policy the update-policy to set to the timer
 */
void etk_slider_update_policy_set(Etk_Slider *slider, Etk_Slider_Update_Policy policy)
{
   if (!slider || slider->policy == policy)
      return;

   slider->policy = policy;
   if (slider->update_timer)
   {
      ecore_timer_del(slider->update_timer);
      slider->update_timer = NULL;
   }
   etk_object_notify(ETK_OBJECT(slider), "update-policy");
}

/**
 * @brief Gets the update-policy of the slider
 * @param slider a slider
 * @return Returns the update-policy of the slider
 */
Etk_Slider_Update_Policy etk_slider_update_policy_get(Etk_Slider *slider)
{
   if (!slider)
      return ETK_SLIDER_CONTINUOUS;
   return slider->policy;
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

   slider->format = NULL;
   slider->inverted = ETK_FALSE;
   slider->dragging = ETK_FALSE;
   slider->policy = ETK_SLIDER_CONTINUOUS;
   slider->update_timer = NULL;

   ETK_RANGE(slider)->value_changed_handler = _etk_slider_value_changed_handler;
   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_key_down_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_WHEEL_SIGNAL, ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_mouse_wheel_cb), NULL);
   etk_object_notification_callback_add(ETK_OBJECT(slider), "lower", _etk_slider_range_changed_cb, NULL);
   etk_object_notification_callback_add(ETK_OBJECT(slider), "upper", _etk_slider_range_changed_cb, NULL);
}

/* Destroys the slider */
static void _etk_slider_destructor(Etk_Slider *slider)
{
   if (!slider)
      return;

   if (slider->update_timer)
      ecore_timer_del(slider->update_timer);
   free(slider->format);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_slider_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Slider *slider;

   if (!(slider = ETK_SLIDER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_SLIDER_LABEL_FORMAT_PROPERTY:
         etk_slider_label_set(slider, etk_property_value_string_get(value));
         break;
      case ETK_SLIDER_INVERTED_PROPERTY:
         etk_slider_inverted_set(slider, etk_property_value_bool_get(value));
         break;
      case ETK_SLIDER_UPDATE_POLICY_PROPERTY:
         etk_slider_update_policy_set(slider, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_slider_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Slider *slider;

   if (!(slider = ETK_SLIDER(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_SLIDER_LABEL_FORMAT_PROPERTY:
         etk_property_value_string_set(value, slider->format);
         break;
      case ETK_SLIDER_INVERTED_PROPERTY:
         etk_property_value_bool_set(value, slider->inverted);
         break;
      case ETK_SLIDER_UPDATE_POLICY_PROPERTY:
         etk_property_value_int_set(value, slider->policy);
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

/* Called when the slider is realized */
static Etk_Bool _etk_slider_realized_cb(Etk_Object *object, void *data)
{
   Etk_Slider *slider;
   Evas_Object *theme_object;

   if (!(slider = ETK_SLIDER(object)) || !(theme_object = ETK_WIDGET(slider)->theme_object))
      return ETK_TRUE;

   etk_widget_theme_signal_emit(ETK_WIDGET(slider),
      slider->inverted ? "etk,state,inverted" : "etk,state,normal", ETK_FALSE);
   etk_widget_theme_signal_emit(ETK_WIDGET(object),
      slider->format ? "etk,action,show,label" : "etk,action,hide,label", ETK_TRUE);
   edje_object_signal_callback_add(theme_object, "drag*", "etk.dragable.slider",
      _etk_slider_cursor_dragged_cb, object);

   _etk_slider_value_changed_handler(ETK_RANGE(slider), ETK_RANGE(slider)->value);
   return ETK_TRUE;
}

/* Called when the user presses a key */
static Etk_Bool _etk_slider_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Range *range;
   Etk_Bool propagate = ETK_FALSE;
   int dir;

   if (!(range = ETK_RANGE(object)))
      return ETK_TRUE;

   dir = ETK_SLIDER(range)->inverted ? -1 : 1;
   if (strcmp(event->keyname, "Right") == 0 || strcmp(event->keyname, "Up") == 0)
      etk_range_value_set(range, range->value + dir * range->step_increment);
   else if (strcmp(event->keyname, "Left") == 0 || strcmp(event->keyname, "Down") == 0)
      etk_range_value_set(range, range->value - dir * range->step_increment);
   else if (strcmp(event->keyname, "Home") == 0)
      etk_range_value_set(range, range->lower);
   else if (strcmp(event->keyname, "End") == 0)
      etk_range_value_set(range, range->upper);
   else if (strcmp(event->keyname, "Next") == 0)
      etk_range_value_set(range, range->value - dir * range->page_increment);
   else if (strcmp(event->keyname, "Prior") == 0)
      etk_range_value_set(range, range->value + dir * range->page_increment);
   else
      propagate = ETK_TRUE;

   return propagate;
}

/* Called when the user wants to change the value with the mouse wheel */
static Etk_Bool _etk_slider_mouse_wheel_cb(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data)
{
   Etk_Range *range;
   int dir;

   if (!(range = ETK_RANGE(object)))
      return ETK_TRUE;

   dir = ETK_SLIDER(range)->inverted ? 1 : -1;
   etk_range_value_set(range, range->value + dir * event->z * range->step_increment);
   return ETK_FALSE;
}

/* Called when the cursor of the slider is dragged */
static void _etk_slider_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Etk_Slider *slider;
   double v;

   if (!(slider = ETK_SLIDER(data)))
      return;

   if (strcmp(emission, "drag,start") == 0)
      slider->dragging = ETK_TRUE;
   else if (strcmp(emission, "drag,stop") == 0)
   {
      /* The drag-button is released: we change the value of the slider if it uses the
       * discontinuous or the delayed update-policy */
      if (slider->policy == ETK_SLIDER_DISCONTINUOUS || slider->policy == ETK_SLIDER_DELAYED)
      {
         v = _etk_slider_value_get_from_edje(slider);
         etk_range_value_set(ETK_RANGE(slider), v);
         if (slider->update_timer)
         {
            ecore_timer_del(slider->update_timer);
            slider->update_timer = NULL;
         }
      }
      slider->dragging = ETK_FALSE;
   }
   else if (strcmp(emission, "drag") == 0)
   {
      /* The drag-button has been moved */
      if (slider->policy == ETK_SLIDER_CONTINUOUS)
      {
         v = _etk_slider_value_get_from_edje(slider);
         etk_range_value_set(ETK_RANGE(slider), v);
      }
      else
      {
         if (slider->policy == ETK_SLIDER_DELAYED)
         {
            if (slider->update_timer)
               ecore_timer_del(slider->update_timer);
            slider->update_timer = ecore_timer_add(UPDATE_DELAY, _etk_slider_update_timer_cb, slider);
         }

         _etk_slider_label_update(slider);
      }
   }
}

/* Default handler for the "value-changed" signal */
static Etk_Bool _etk_slider_value_changed_handler(Etk_Range *range, double value)
{
   Etk_Slider *slider;
   Evas_Object *theme_object;
   double percent;

   if (!(slider = ETK_SLIDER(range)) || !(theme_object = ETK_WIDGET(slider)->theme_object))
      return ETK_TRUE;

   if (range->upper - range->page_size > range->lower)
      percent = ETK_CLAMP((value - range->lower) / (range->upper - range->lower - range->page_size), 0.0, 1.0);
   else
      percent = 0.0;

   if (!slider->dragging)
   {
      if (ETK_IS_HSLIDER(slider))
      {
         percent = slider->inverted ? (1.0 - percent) : percent;
         edje_object_part_drag_value_set(theme_object, "etk.dragable.slider", percent, 0.0);
      }
      else
      {
         percent = slider->inverted ? percent : (1.0 - percent);
         edje_object_part_drag_value_set(theme_object, "etk.dragable.slider", 0.0, percent);
      }
   }

   _etk_slider_label_update(slider);
   return ETK_TRUE;
}

/* Called when the range of the slider is changed */
static void _etk_slider_range_changed_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Range *range;

   if (!(range = ETK_RANGE(object)))
      return;
   _etk_slider_value_changed_handler(range, range->value);
}

/* Timer used to update the slider's value when its policy is ETK_SLIDER_DELAYED */
static int _etk_slider_update_timer_cb(void *data)
{
   Etk_Slider *slider;
   double v;

   if (!(slider = ETK_SLIDER(data)))
      return 0;

   v = _etk_slider_value_get_from_edje(slider);
   etk_range_value_set(ETK_RANGE(slider), v);
   slider->update_timer = NULL;
   return 0;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Gets the value of the slider from the position of the drag-object */
static double _etk_slider_value_get_from_edje(Etk_Slider *slider)
{
   Etk_Range *range;
   double v;

   if (!(range = ETK_RANGE(slider)) || !ETK_WIDGET(slider)->theme_object || range->upper <= range->lower)
      return 0.0;

   if (ETK_IS_HSLIDER(slider))
   {
      edje_object_part_drag_value_get(ETK_WIDGET(slider)->theme_object, "etk.dragable.slider", &v, NULL);
      v = slider->inverted ? (1.0 - v) : v;
   }
   else
   {
      edje_object_part_drag_value_get(ETK_WIDGET(slider)->theme_object, "etk.dragable.slider", NULL, &v);
      v = slider->inverted ? v : (1.0 - v);
   }

   v = range->lower + v * (range->upper - range->lower);
   return v;
}

/* Updates the label of the slider's theme-object */
static void _etk_slider_label_update(Etk_Slider *slider)
{
   char label[256];
   double v;

   if (!slider)
      return;

   if (slider->format)
   {
      v = _etk_slider_value_get_from_edje(slider);
      snprintf(label, sizeof(label), slider->format, v);
   }
   else
      label[0] = '\0';
   etk_widget_theme_part_text_set(ETK_WIDGET(slider), "etk.text.label", label);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Slider
 *
 * @image html widgets/slider.png
 * Etk_Slider is the base class for Etk_HSlider (for horizontal sliders) and Etk_VSlider (for vertical sliders). @n
 * Since Etk_Slider inherits from Etk_Range, you can use all the @a etk_range_*() functions to get or set the value of
 * a slider, or to change its bounds. You can also use the @a "value-changed" signal to be notified when the value
 * of a slider is changed. @n @n
 * A slider can have different update-policies: by default, it uses a continuous update-policy, meaning the value of
 * the slider will be changed each timer the slider's button is moved. But a slider can also use a discontinuous
 * update-policy (the value will be changed only when the drag-button is released) or a delayed update-policy (the
 * value will be changed after a brief timeout where no slider motion occurs). The update-policy can be changed with
 * etk_slider_update_policy_set(). @n \n
 * By default, the maximum bound of an Etk_HSlider is the right end, and the top end for an Etk_VSlider. But you can
 * invert the ends of a slider with etk_slider_inverted_set(). @n @n
 * Sliders can also have their own label. For example, if you want to use a slider to control a value in centimeters,
 * you can add the associated label with:
 * @code
 * //Display the value of the slider with the format "2.17 cm"
 * etk_slider_label_set(slider, "%.2f cm");
 * @endcode @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Range
 *       - Etk_Slider
 *         - Etk_HSlider
 *         - Etk_VSlider
 *
 * \par Properties:
 * @prop_name "label-format": The format of the slider's label, or NULL if the label is hidden
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "inverted": Whether or not the slider is inverted
 * @prop_type Boolean (char *)
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "update-policy": The update-policy of the slider (continuous, discontinuous or delayed)
 * @prop_type Integer (Etk_Slider_Update_Policy)
 * @prop_rw
 * @prop_val ETK_SLIDER_CONTINUOUS
 */
