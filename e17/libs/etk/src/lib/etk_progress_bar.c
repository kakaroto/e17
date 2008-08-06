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

/** @file etk_progress_bar.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

enum Etk_Progress_Bar_Property_Id
{
   ETK_PROGRESS_BAR_TEXT_PROPERTY,
   ETK_PROGRESS_BAR_FRACTION_PROPERTY,
   ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY
};

static void _etk_progress_bar_constructor(Etk_Progress_Bar *progress_bar);
static void _etk_progress_bar_destructor(Etk_Progress_Bar *progress_bar);
static void _etk_progress_bar_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_progress_bar_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_progress_bar_update(Etk_Progress_Bar *progress_bar);
static Etk_Bool _etk_progress_bar_realized_cb(Etk_Object *object, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Progress_Bar
 * @return Returns the type of an Etk_Progress_Bar
 */
Etk_Type *etk_progress_bar_type_get(void)
{
   static Etk_Type *progress_bar_type = NULL;

   if (!progress_bar_type)
   {
      progress_bar_type = etk_type_new("Etk_Progress_Bar", ETK_WIDGET_TYPE,
         sizeof(Etk_Progress_Bar),
         ETK_CONSTRUCTOR(_etk_progress_bar_constructor),
         ETK_DESTRUCTOR(_etk_progress_bar_destructor), NULL);

      etk_type_property_add(progress_bar_type, "text", ETK_PROGRESS_BAR_TEXT_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(progress_bar_type, "fraction", ETK_PROGRESS_BAR_FRACTION_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));
      etk_type_property_add(progress_bar_type, "pulse-step", ETK_PROGRESS_BAR_PULSE_STEP_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.1));

      progress_bar_type->property_set = _etk_progress_bar_property_set;
      progress_bar_type->property_get = _etk_progress_bar_property_get;
   }

   return progress_bar_type;
}

/**
 * @brief Creates a new progress bar
 * @return Returns the new progress bar widget
 */
Etk_Widget *etk_progress_bar_new(void)
{
   return etk_widget_new(ETK_PROGRESS_BAR_TYPE, "theme-group", "progress_bar", NULL);
}

/**
 * @brief Creates a new progress bar with a text
 * @param text the text to display in the progress bar
 * @return Returns the new progress bar widget
 */
Etk_Widget *etk_progress_bar_new_with_text(const char *text)
{
   return etk_widget_new(ETK_PROGRESS_BAR_TYPE, "theme-group", "progress_bar", "text", text, NULL);
}

/**
 * @brief Sets the text of the progress bar
 * @param progress_bar a progress bar
 * @param text the text to set
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
 * @param progress_bar a progress bar
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
 * @param progress_bar a progress bar
 * @param fraction the fraction (percentage) to set
 */
void etk_progress_bar_fraction_set(Etk_Progress_Bar *progress_bar, double fraction)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(progress_bar)))
      return;

   fraction = ETK_CLAMP(fraction, 0.0, 1.0);
   if (progress_bar->fraction != fraction || progress_bar->is_pulsing == ETK_TRUE)
   {
      progress_bar->fraction = fraction;
      progress_bar->is_pulsing = ETK_FALSE;
      _etk_progress_bar_update(progress_bar);
      etk_object_notify(ETK_OBJECT(progress_bar), "fraction");
   }
}

/**
 * @brief Gets the fraction (percentage) of the progress bar
 * @param progress_bar a progress bar
 * @return Returns the fraction (percentage) of the progress bar
 */
double etk_progress_bar_fraction_get(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return 0.0;
   return progress_bar->fraction;
}

/**
 * @brief Causes the progress bar to enter pulse mode, subsequent calls simply move the bar around
 * @param progress_bar a progress bar
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
 * @brief Set the step-size for the pulse mode, between 0.0 and 1.0.
 * The step-size is the fraction of total progress to move the filler when it is pulsed.
 * @param progress_bar a progress bar
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
      etk_object_notify(ETK_OBJECT(progress_bar), "pulse-step");
   }
}

/**
 * @brief Get the step-size for the pulse mode
 * @param progress_bar a progress bar
 * @return Returns the step-size of the pulse mode
 */
double etk_progress_bar_pulse_step_get(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return 0.0;
   return progress_bar->pulse_step;
}

/**
 * @brief Sets the growth direction of the progress bar (left to right, or right to left)
 * @param progress_bar a progress bar
 * @param direction the direction to use
 */
void etk_progress_bar_direction_set(Etk_Progress_Bar *progress_bar, Etk_Progress_Bar_Direction direction)
{
   if (!progress_bar || progress_bar->direction == direction)
      return;

   progress_bar->direction = direction;
   _etk_progress_bar_update(progress_bar);
   etk_object_notify(ETK_OBJECT(progress_bar), "direction");
}

/**
 * @brief Gets the growth direction of the progress bar (left to right, or right to left)
 * @param progress_bar a progress bar
 * @return Returns the growth direction of the progress bar
 */
Etk_Progress_Bar_Direction etk_progress_bar_direction_get(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return ETK_PROGRESS_BAR_LEFT_TO_RIGHT;
   return progress_bar->direction;
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
   progress_bar->direction = ETK_PROGRESS_BAR_LEFT_TO_RIGHT;
   progress_bar->is_pulsing = ETK_FALSE;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(progress_bar), ETK_CALLBACK(_etk_progress_bar_realized_cb), NULL);
}

/* Destroys the progress bar */
static void _etk_progress_bar_destructor(Etk_Progress_Bar *progress_bar)
{
   if (!progress_bar)
      return;
   free(progress_bar->text);
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
 * Callbacks and handlers
 *
 **************************/

/* Called when the progress bar is realized */
static Etk_Bool _etk_progress_bar_realized_cb(Etk_Object *object, void *data)
{
   Etk_Progress_Bar *progress_bar;
   float w;

   if (!(progress_bar = ETK_PROGRESS_BAR(object)))
      return ETK_TRUE;

   if (etk_widget_theme_data_get(ETK_WIDGET(object), "filler_pulse_width", "%f", &w) != 1)
      w = 0.3;
   progress_bar->filler_pulse_w = ETK_CLAMP(w, 0.1, 0.9);

   _etk_progress_bar_update(progress_bar);
   return ETK_TRUE;
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
      if (progress_bar->direction == ETK_PROGRESS_BAR_LEFT_TO_RIGHT)
         edje_object_part_drag_value_set(widget->theme_object, "etk.dragable.filler", 0.0, 0.0);
      else
         edje_object_part_drag_value_set(widget->theme_object, "etk.dragable.filler", 1.0, 0.0);
      edje_object_part_drag_size_set(widget->theme_object, "etk.dragable.filler", progress_bar->fraction, 0.0);
   }
   else
   {
      edje_object_part_drag_value_set(widget->theme_object, "etk.dragable.filler", progress_bar->pulse_pos, 0.0);
      edje_object_part_drag_size_set(widget->theme_object, "etk.dragable.filler", progress_bar->filler_pulse_w, 0.0);
   }

   etk_widget_theme_part_text_set(widget, "etk.text.text", progress_bar->text ? progress_bar->text : "");
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Progress_Bar
 *
 * @image html widgets/progress_bar.png
 * A progress bar can work in two different modes: the progress mode and the pulse mode.
 * - In the progress mode, the progress bar has a growing bar that indicates the progress of the current operation.
 * You can change the fraction of the total progress with etk_progress_bar_fraction_set(). This mode is used when you
 * can have an accurate value of the progress.
 * - In the pulse mode, the bar has a constant width and bounces inside the progress bar. It is moved each time you call
 * etk_progress_bar_pulse(). You can also set the pulse-step with etk_progress_bar_pulse_step_set(). This mode is used
 * when you can't know the progress of the operation (for example, it can be used for a download when you can't know the
 * total size of the file) @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Progress_Bar
 *
 * \par Properties:
 * @prop_name "text": The text to display in the progress bar
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "fraction": The progress, between 0.0 and 1.0, of the progress bar
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.0
 * \par
 * @prop_name "direction": The growth direction of the progress mode
 * @prop_type Integer (Etk_Progress_Bar_Direction)
 * @prop_rw
 * @prop_val ETK_PROGRESS_BAR_LEFT_TO_RIGHT
 * \par
 * @prop_name "pulse-step": The fraction of the total progress to move the filler when the progress bar is pulsed
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.1
 */
