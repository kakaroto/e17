/** @file etk_progress_bar.c */
#include "etk_progress_bar.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_widget.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Progress_Bar
 * @{
 */

enum _Etk_Progress_Bar_Property_Id
{
   ETK_PROGRESS_BAR_TEXT_PROPERTY,
   ETK_PROGRESS_BAR_FRACTION_PROPERTY,
   ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY
};

static void _etk_progress_bar_constructor(Etk_Progress_Bar *progress_bar);
static void _etk_progress_bar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_progress_bar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_progress_bar_update(Etk_Progress_Bar *progress_bar);
static void _etk_progress_bar_realize_cb(Etk_Object *object, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Progress_Bar
 * @return Returns the type on an Etk_Progress_Bar
 */
Etk_Type *etk_progress_bar_type_get()
{
   static Etk_Type *progress_bar_type = NULL;

   if (!progress_bar_type)
   {
      progress_bar_type = etk_type_new("Etk_Progress_Bar", ETK_WIDGET_TYPE, sizeof(Etk_Progress_Bar), ETK_CONSTRUCTOR(_etk_progress_bar_constructor), NULL);

      etk_type_property_add(progress_bar_type, "text", ETK_PROGRESS_BAR_TEXT_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(progress_bar_type, "fraction", ETK_PROGRESS_BAR_FRACTION_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));
      etk_type_property_add(progress_bar_type, "pulse_step", ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.1));
      
      progress_bar_type->property_set = _etk_progress_bar_property_set;
      progress_bar_type->property_get = _etk_progress_bar_property_get;
   }

   return progress_bar_type;
}

/**
 * @brief Creates a new progress bar
 * @return Returns the new progress bar widget
 */
Etk_Widget *etk_progress_bar_new()
{
   Etk_Widget *progress_bar;
   
   progress_bar = etk_widget_new(ETK_PROGRESS_BAR_TYPE, "theme_group", "progress_bar", NULL);
   if(!progress_bar)
     return NULL;
   
   etk_signal_connect("realize", ETK_OBJECT(progress_bar), ETK_CALLBACK(_etk_progress_bar_realize_cb), NULL);
   return progress_bar;
}

/**
 * @brief Creates a new progress bar with a label
 * @return Returns the new progress bar widget
 */
Etk_Widget *etk_progress_bar_new_with_text(const char *text)
{
   Etk_Widget *progress_bar;
   
   progress_bar =  etk_widget_new(ETK_PROGRESS_BAR_TYPE, "theme_group", "progress_bar", "text", text, NULL);
   if(!progress_bar)
     return NULL;
   
   etk_signal_connect("realize", ETK_OBJECT(progress_bar), ETK_CALLBACK(_etk_progress_bar_realize_cb), NULL);
   return progress_bar;   
}

/**
 * @brief Sets the text of the progress bar
 * @param progress_bar a progress bar
 * @param text the text to set. You can use the subsitution character %p which will be replaced by the current percentage
 */
void etk_progress_bar_text_set(Etk_Progress_Bar *progress_bar, const char *text)
{
   if (!progress_bar || progress_bar->text == text)
      return;
   
   if (progress_bar->text)
      free(progress_bar->text);
   progress_bar->text = text ? strdup(text) : NULL;
   
   _etk_progress_bar_update(progress_bar);
   etk_object_notify(ETK_OBJECT(progress_bar), "text");
}

/**
 * @brief Gets the text of the progress bar
 * @param progress_bar a progess bar
 * @return Returns the text of the progress bar
 */
const char *etk_progress_bar_text_get(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return NULL;
   return progress_bar->text;
}

/**
 * @brief Sets the fraction (percentage) of the progress bar
 * @param progress_bar a progess bar
 * @param fraction the fraction (percentage) to set
 */
void etk_progress_bar_fraction_set(Etk_Progress_Bar *progress_bar, double fraction)
{
   Etk_Widget *widget;
   
   if (!(widget = ETK_WIDGET(progress_bar)))
      return;
   
   fraction = ETK_CLAMP(fraction, 0.0, 1.0);
   if (progress_bar->fraction != fraction)
   {
      progress_bar->fraction = fraction;
      progress_bar->is_pulsing = ETK_FALSE;
      _etk_progress_bar_update(progress_bar);
      etk_object_notify(ETK_OBJECT(progress_bar), "fraction");
   }
}

/**
 * @brief Gets the fraction (percentage) of the progress bar
 * @param progress_bar a progess bar
 * @return fraction the fraction (percentage) to set
 */
double etk_progress_bar_fraction_get(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return 0.0;
   return progress_bar->fraction;
}

/**
 * @brief Causes the progress bar to enter pulse mode, subsequent calls simple move the bar around
 * @param progress_bar a progess bar
 */
void etk_progress_bar_pulse(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return;
   
   if (!progress_bar->is_pulsing)
   {
      progress_bar->pulse_pos = 0.0;
      progress_bar->pulse_dir = ETK_PROGRESS_BAR_LEFT_TO_RIGHT;
      progress_bar->is_pulsing = ETK_TRUE;
   }
   else
   {
      if (progress_bar->pulse_dir == ETK_PROGRESS_BAR_LEFT_TO_RIGHT)
      {
         progress_bar->pulse_pos += progress_bar->pulse_step;
         if (progress_bar->pulse_pos > 1.0)
         {
            progress_bar->pulse_dir = ETK_PROGRESS_BAR_RIGHT_TO_LEFT;
            progress_bar->pulse_pos = 1.0;
         }
      }
      else
      {
         progress_bar->pulse_pos -= progress_bar->pulse_step;
         if (progress_bar->pulse_pos < 0.0)
         {
            progress_bar->pulse_dir = ETK_PROGRESS_BAR_LEFT_TO_RIGHT;
            progress_bar->pulse_pos = 0.0;
         }
      }
   }
   
   _etk_progress_bar_update(progress_bar);
}

/**
 * @brief Set the step size for the activity mode.
 * @param progress_bar a progess bar
 * @param pulse_step the step size
 */
void etk_progress_bar_pulse_step_set(Etk_Progress_Bar *progress_bar, double pulse_step)
{
   if (!progress_bar)
      return;
 
   pulse_step = ETK_CLAMP(pulse_step, 0.0, 1.0);
   if (progress_bar->pulse_step != pulse_step)
   {
      progress_bar->pulse_step = pulse_step;
      etk_object_notify(ETK_OBJECT(progress_bar), "pulse_step");
   }
}

/**
 * @brief Get the step size for the activity mode.
 * @param progress_bar a progess bar
 * @return Returns the step size
 */
double etk_progress_bar_pulse_step_get(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return 0.0;
   return progress_bar->pulse_step;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_progress_bar_constructor(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return;

   progress_bar->text = NULL;
   progress_bar->fraction = 0.0;
   progress_bar->pulse_pos = 0.0;
   progress_bar->pulse_step = 0.1;
   progress_bar->pulse_dir = ETK_PROGRESS_BAR_LEFT_TO_RIGHT;
   progress_bar->orientation = ETK_PROGRESS_BAR_LEFT_TO_RIGHT;
   progress_bar->is_pulsing = ETK_FALSE;
   
   etk_signal_connect_swapped("realize", ETK_OBJECT(progress_bar), ETK_CALLBACK(_etk_progress_bar_update), progress_bar);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_progress_bar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Progress_Bar *progress_bar;

   if (!(progress_bar = ETK_PROGRESS_BAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_PROGRESS_BAR_TEXT_PROPERTY:
         etk_progress_bar_text_set(progress_bar, etk_property_value_string_get(value));
         break;
      case ETK_PROGRESS_BAR_FRACTION_PROPERTY:
         etk_progress_bar_fraction_set(progress_bar, etk_property_value_double_get(value));
         break;
      case ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY:
         etk_progress_bar_pulse_step_set(progress_bar, etk_property_value_double_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_progress_bar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Progress_Bar *progress_bar;

   if (!(progress_bar = ETK_PROGRESS_BAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_PROGRESS_BAR_TEXT_PROPERTY:
         etk_property_value_string_set(value, progress_bar->text);
         break;
      case ETK_PROGRESS_BAR_FRACTION_PROPERTY:
         etk_property_value_double_set(value, progress_bar->fraction);
         break;
      case ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY:
         etk_property_value_double_set(value, progress_bar->pulse_step);
         break;      
      default:
         break;
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Updates the label and the bar of the progress bar */
static void _etk_progress_bar_update(Etk_Progress_Bar *progress_bar)
{
   Etk_Widget *widget;
   
   if (!(widget = ETK_WIDGET(progress_bar)) || !widget->theme_object)
      return;
   
   if (!progress_bar->is_pulsing)
   {
      if (progress_bar->orientation == ETK_PROGRESS_BAR_LEFT_TO_RIGHT)
         edje_object_part_drag_value_set(widget->theme_object, "filler", 0.0, 0.0);
      else
         edje_object_part_drag_value_set(widget->theme_object, "filler", 1.0, 0.0);
      edje_object_part_drag_size_set(widget->theme_object, "filler", progress_bar->fraction, 0.0);
   }
   else
   {
      edje_object_part_drag_value_set(widget->theme_object, "filler", progress_bar->pulse_pos, 0.0);
      edje_object_part_drag_size_set(widget->theme_object, "filler", progress_bar->filler_pulse_w, 0.0);
   }
   
   etk_widget_theme_object_part_text_set(widget, "text", progress_bar->text ? progress_bar->text : "");
}

static void _etk_progress_bar_realize_cb(Etk_Object *object, void *data)
{     
   Etk_Progress_Bar *progress_bar;
   
   progress_bar = ETK_PROGRESS_BAR(object);
   if (etk_widget_theme_object_data_get(ETK_WIDGET(object), "filler_pulse_width", "%lg", &progress_bar->filler_pulse_w) != 1)
     progress_bar->filler_pulse_w = 0.3;
}

/** @} */
