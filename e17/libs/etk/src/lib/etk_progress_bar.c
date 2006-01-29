/** @file etk_progress_bar.c */
#include "etk_progress_bar.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_label.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Progress_Bar
* @{
 */


enum _Etk_Progress_Bar_Property_Id
{
   ETK_PROGRESS_BAR_LABEL_PROPERTY,
   ETK_PROGRESS_BAR_FRACTION_PROPERTY,
   ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY
};

enum _Etk_Progress_Bar_Activity_Dir
{
   ETK_PROGRESS_BAR_ACTIVITY_DIR_LEFT,
   ETK_PROGRESS_BAR_ACTIVITY_DIR_RIGHT
};

static void _etk_progress_bar_constructor(Etk_Progress_Bar *progress_bar);
static void _etk_progress_bar_destructor(Etk_Progress_Bar *progress_bar);
static void _etk_progress_bar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_progress_bar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_progress_bar_child_create(Etk_Progress_Bar *progress_bar);

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
      progress_bar_type = etk_type_new("Etk_Progress_Bar", ETK_BIN_TYPE, sizeof(Etk_Progress_Bar), ETK_CONSTRUCTOR(_etk_progress_bar_constructor), NULL);

      etk_type_property_add(progress_bar_type, "label", ETK_PROGRESS_BAR_LABEL_PROPERTY, ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));
      etk_type_property_add(progress_bar_type, "fraction", ETK_PROGRESS_BAR_FRACTION_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(progress_bar_type, "pulse_step", ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.1));
      
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
   return etk_widget_new(ETK_PROGRESS_BAR_TYPE, "theme_group", "progress_bar", NULL);
}

/**
 * @brief Creates a new progress bar with a label
 * @return Returns the new progress bar widget
 */
Etk_Widget *etk_progress_bar_new_with_text(const char *text)
{
   return etk_widget_new(ETK_PROGRESS_BAR_TYPE, "theme_group", "progress_bar", "label", text, NULL);
}

/**
 * @brief Sets the text of the progress bar
 * @param progress_bar a progress bar
 * @param text the text to set
 */
void etk_progress_bar_text_set(Etk_Progress_Bar *progress_bar, const char *text)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(progress_bar)) || !progress_bar->label)
      return;

   etk_label_set(ETK_LABEL(progress_bar->label), text);
   _etk_progress_bar_child_create(progress_bar);
}

/**
 * @brief Gets the text of the progress bar
 * @param progress_bar a progess bar
 * @return Returns the text of the progress bar
 */
const char *etk_progress_bar_text_get(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar || !progress_bar->label)
      return NULL;
   return etk_label_get(ETK_LABEL(progress_bar->label));;
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
   
   if(fraction < 0.0) fraction = 0.0;
   if(fraction > 1.0) fraction = 1.0;
   
   edje_object_part_drag_size_set(widget->theme_object, "filler", fraction, 0.0);
}

/**
 * @brief Gets the fraction (percentage) of the progress bar
 * @param progress_bar a progess bar
 * @return fraction the fraction (percentage) to set
 */
double etk_progress_bar_fraction_get(Etk_Progress_Bar *progress_bar)
{
   Etk_Widget *widget;
   double dw;
   double dh;
   
   if (!(widget = ETK_WIDGET(progress_bar)))
     return -0.1;
   
   edje_object_part_drag_size_get(widget->theme_object, "filler", &dw, &dh);
      
   return dw;
}

/**
 * @brief Causes the progress bar to enter pulse mode, subsequent calls simple move the bar around
 * @param progress_bar a progess bar
 */
void etk_progress_bar_pulse(Etk_Progress_Bar *progress_bar)
{
   Etk_Widget *widget;
   double new_drag_value;
   double dx;
   double dy;
   
   if (!(widget = ETK_WIDGET(progress_bar)))
     return;
   
   edje_object_part_drag_size_set(widget->theme_object, "filler", 0.3, 0.0);
   
   edje_object_part_drag_value_get(widget->theme_object, "filler", &dx, &dy);
   new_drag_value = dx;
   
   if(progress_bar->activity_dir == ETK_PROGRESS_BAR_ACTIVITY_DIR_LEFT)
   {   
      new_drag_value += progress_bar->pulse_step;
      if(new_drag_value > 1.0)
      {
	 progress_bar->activity_dir = ETK_PROGRESS_BAR_ACTIVITY_DIR_RIGHT;
      }   
   } else {
      
      new_drag_value -= progress_bar->pulse_step;
      if(new_drag_value < 0.0)
      {
         progress_bar->activity_dir = ETK_PROGRESS_BAR_ACTIVITY_DIR_LEFT;
      }
   }   
   
   edje_object_part_drag_value_set(widget->theme_object, "filler", new_drag_value, 0.0);
}

/**
 * @brief Set the step size for the activity mode.
 * @param progress_bar a progess bar
 * @param pulse_step the step size
 */
void etk_progress_bar_pulse_step_set(Etk_Progress_Bar *progress_bar, double pulse_step)
{
   Etk_Widget *widget;
   
   if (!(widget = ETK_WIDGET(progress_bar)))
     return;
 
   if(pulse_step > 1.0 || pulse_step < 0.0)
     return;
   
   progress_bar->pulse_step = pulse_step;
}

/**
 * @brief Get the step size for the activity mode.
 * @param progress_bar a progess bar
 * @return Returns the step size
 */
double etk_progress_bar_pulse_step_get(Etk_Progress_Bar *progress_bar)
{
   Etk_Widget *widget;
   
   if (!(widget = ETK_WIDGET(progress_bar)))
     return;
 
   return progress_bar->pulse_step;
}

#if 0
/* TODO: we cant allow this until we have vertical labels */
/**
 * @brief Set the orientation of the progress bar.
 * @param progress_bar a progess bar
 * @param orientation the new orientation
 */
void etk_progress_bar_pulse_orientation_set(Etk_Progress_Bar *progress_bar, Etk_Progress_Bar_Orientation orientation)
{
   Etk_Widget *widget;
   
   if (!(widget = ETK_WIDGET(progress_bar)))
     return;
 
   if(progress_bar->orientation == orientation)
     return;
   
   progress_bar->orientation = orientation;
}
#endif 

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

   progress_bar->activity_dir = ETK_PROGRESS_BAR_ACTIVITY_DIR_LEFT;
   progress_bar->pulse_step = 0.1;
   progress_bar->orientation = ETK_PROGRESS_BAR_LEFT_TO_RIGHT;
   
   progress_bar->label = etk_label_new(NULL);
   etk_widget_visibility_locked_set(progress_bar->label, ETK_TRUE);
   etk_label_alignment_set(ETK_LABEL(progress_bar->label), 0.5, 0.5);
   etk_widget_pass_events_set(progress_bar->label, ETK_TRUE);
   etk_container_add(ETK_CONTAINER(progress_bar), progress_bar->label);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_progress_bar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Progress_Bar *progress_bar;

   if (!(progress_bar = ETK_PROGRESS_BAR(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_PROGRESS_BAR_LABEL_PROPERTY:
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
      case ETK_PROGRESS_BAR_LABEL_PROPERTY:
         etk_property_value_string_set(value, etk_progress_bar_text_get(progress_bar));
         break;
      case ETK_PROGRESS_BAR_FRACTION_PROPERTY:
         etk_property_value_double_set(value, etk_progress_bar_fraction_get(progress_bar));
         break;
      case ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY:
         etk_property_value_double_set(value, etk_progress_bar_pulse_step_get(progress_bar));
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

/* Creates the child of the progress_bar */
static void _etk_progress_bar_child_create(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return;

   if (progress_bar->label)
   {
      etk_container_add(ETK_CONTAINER(progress_bar), progress_bar->label);
      etk_widget_show(ETK_WIDGET(progress_bar->label));
   }
}

/** @} */
