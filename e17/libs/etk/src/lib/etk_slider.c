/** @file etk_slider.c */
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

enum Etk_Slider_Property_Id
{
   ETK_SLIDER_LABEL_FORMAT_PROPERTY
};

static void _etk_slider_constructor(Etk_Slider *slider);
static void _etk_slider_destructor(Etk_Slider *slider);
static void _etk_slider_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_slider_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_slider_realize_cb(Etk_Object *object, void *data);
static void _etk_slider_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static void _etk_slider_mouse_wheel(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data);
static void _etk_slider_cursor_dragged_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _etk_slider_value_changed_handler(Etk_Range *range, double value);
static void _etk_slider_range_changed_cb(Etk_Object *object, const char *property_name, void *data);
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
         ETK_CONSTRUCTOR(_etk_slider_constructor), ETK_DESTRUCTOR(_etk_slider_destructor));
      
      etk_type_property_add(slider_type, "label_format", ETK_SLIDER_LABEL_FORMAT_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      
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
      hslider_type = etk_type_new("Etk_HSlider", ETK_SLIDER_TYPE, sizeof(Etk_HSlider), NULL, NULL);

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
      vslider_type = etk_type_new("Etk_VSlider", ETK_SLIDER_TYPE, sizeof(Etk_VSlider), NULL, NULL);

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
   return etk_widget_new(ETK_HSLIDER_TYPE, "theme_group", "hslider", "focusable", ETK_TRUE,
      "lower", lower, "upper", upper, "value", value, "step_increment", step_increment,
      "page_increment", page_increment, "focus_on_click", ETK_TRUE, NULL);
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
   return etk_widget_new(ETK_VSLIDER_TYPE, "theme_group", "vslider", "focusable", ETK_TRUE,
      "lower", lower, "upper", upper, "value", value, "step_increment", step_increment,
      "page_increment", page_increment, "focus_on_click", ETK_TRUE, NULL);
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
   
   etk_object_notify(ETK_OBJECT(slider), "label_format");
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
   slider->format = NULL;
   
   ETK_RANGE(slider)->value_changed = _etk_slider_value_changed_handler;
   etk_signal_connect("realize", ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_realize_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_key_down_cb), NULL);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(slider), ETK_CALLBACK(_etk_slider_mouse_wheel), NULL);
   etk_object_notification_callback_add(ETK_OBJECT(slider), "lower", _etk_slider_range_changed_cb, NULL);
   etk_object_notification_callback_add(ETK_OBJECT(slider), "upper", _etk_slider_range_changed_cb, NULL);
}

/* Destroys the slider */
static void _etk_slider_destructor(Etk_Slider *slider)
{
   if (!slider)
      return;
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
static void _etk_slider_realize_cb(Etk_Object *object, void *data)
{
   Etk_Slider *slider;
   Evas_Object *theme_object;

   if (!(slider = ETK_SLIDER(object)) || !(theme_object = ETK_WIDGET(slider)->theme_object))
      return;

   etk_widget_theme_signal_emit(ETK_WIDGET(object),
      slider->format ? "etk,action,show,label" : "etk,action,hide,label", ETK_TRUE);
   edje_object_signal_callback_add(theme_object, "drag*", "etk.dragable.slider",
      _etk_slider_cursor_dragged_cb, object);
   
   _etk_slider_value_changed_handler(ETK_RANGE(slider), ETK_RANGE(slider)->value);
}

/* Called when the user presses a key */
static void _etk_slider_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Range *range;
   Etk_Bool propagate = ETK_FALSE;

   if (!(range = ETK_RANGE(object)))
      return;

   if (strcmp(event->keyname, "Right") == 0 || strcmp(event->keyname, "Down") == 0)
      etk_range_value_set(range, range->value + range->step_increment);
   else if (strcmp(event->keyname, "Left") == 0 || strcmp(event->keyname, "Up") == 0)
      etk_range_value_set(range, range->value - range->step_increment);
   else if (strcmp(event->keyname, "Home") == 0)
      etk_range_value_set(range, range->lower);
   else if (strcmp(event->keyname, "End") == 0)
      etk_range_value_set(range, range->upper);
   else if (strcmp(event->keyname, "Next") == 0)
      etk_range_value_set(range, range->value + range->page_increment);
   else if (strcmp(event->keyname, "Prior") == 0)
      etk_range_value_set(range, range->value - range->page_increment);
   else
      propagate = ETK_TRUE;
   
   if (!propagate)
      etk_signal_stop();
}

/* Called when the user wants to change the value with the mouse wheel */
static void _etk_slider_mouse_wheel(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data)
{
   Etk_Range *slider_range;
   
   if (!(slider_range = ETK_RANGE(object)))
      return;
   
   etk_range_value_set(slider_range, slider_range->value + event->z * slider_range->step_increment);
   etk_signal_stop();
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
         edje_object_part_drag_value_get(obj, "etk.dragable.slider", &v, NULL);
      else
         edje_object_part_drag_value_get(obj, "etk.dragable.slider", NULL, &v);
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

   if (range->upper - range->page_size > range->lower)
      percent = ETK_CLAMP((value - range->lower) / (range->upper - range->lower - range->page_size), 0.0, 1.0);
   else
      percent = 0.0;
   
   if (!slider->dragging)
   {
      if (ETK_IS_HSLIDER(slider))
         edje_object_part_drag_value_set(theme_object, "etk.dragable.slider", percent, 0.0);
      else
         edje_object_part_drag_value_set(theme_object, "etk.dragable.slider", 0.0, percent);
   }
   
   _etk_slider_label_update(slider);
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
      edje_object_part_drag_value_set(theme_object, "etk.dragable.slider", percent, 0.0);
   else
      edje_object_part_drag_value_set(theme_object, "etk.dragable.slider", 0.0, percent);
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Updates the label of the slider's theme-object */
static void _etk_slider_label_update(Etk_Slider *slider)
{
   char label[256];
   
   if (!slider || !slider->format)
      return;
   
   snprintf(label, sizeof(label), slider->format, etk_range_value_get(ETK_RANGE(slider)));
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
 * a slider, or to change its bounds. You can also use the @a "value_changed" signal to be notified when the value
 * of a slider is changed. @n
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
 * @prop_name "label_format": The format of the slider's label, or NULL if the label is hidden
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 */
