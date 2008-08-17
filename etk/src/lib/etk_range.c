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

/** @file etk_range.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_range.h"

#include <stdlib.h>
#include <string.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Range
 * @{
 */

int ETK_RANGE_VALUE_CHANGED_SIGNAL;

enum Etk_Range_Property_Id
{
   ETK_RANGE_LOWER_PROPERTY,
   ETK_RANGE_UPPER_PROPERTY,
   ETK_RANGE_VALUE_PROPERTY,
   ETK_RANGE_STEP_INC_PROPERTY,
   ETK_RANGE_PAGE_INC_PROPERTY,
   ETK_RANGE_PAGE_SIZE_PROPERTY
};

static void _etk_range_constructor(Etk_Range *range);
static void _etk_range_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_range_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Range
 * @return Returns the type of an Etk_Range
 */
Etk_Type *etk_range_type_get(void)
{
   static Etk_Type *range_type = NULL;

   if (!range_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_HANDLER(ETK_RANGE_VALUE_CHANGED_SIGNAL,
            "value-changed", Etk_Range, value_changed_handler,
            etk_marshaller_DOUBLE),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      range_type = etk_type_new("Etk_Range", ETK_WIDGET_TYPE,
         sizeof(Etk_Range), ETK_CONSTRUCTOR(_etk_range_constructor), NULL,
         signals);

      etk_type_property_add(range_type, "lower", ETK_RANGE_LOWER_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));
      etk_type_property_add(range_type, "upper", ETK_RANGE_UPPER_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));
      etk_type_property_add(range_type, "value", ETK_RANGE_VALUE_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));
      etk_type_property_add(range_type, "step-increment", ETK_RANGE_STEP_INC_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));
      etk_type_property_add(range_type, "page-increment", ETK_RANGE_PAGE_INC_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));
      etk_type_property_add(range_type, "page-size", ETK_RANGE_PAGE_SIZE_PROPERTY,
         ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_double(0.0));

      range_type->property_set = _etk_range_property_set;
      range_type->property_get = _etk_range_property_get;
   }

   return range_type;
}

/**
 * @brief Sets the value of the range
 * @param range a range
 * @param value the value to set to the range
 * @return Returns ETK_TRUE if the value has actually been changed, ETK_FALSE otherwise (because
 * the current value is equal to the new value)
 */
Etk_Bool etk_range_value_set(Etk_Range *range, double value)
{
   double new_value;

   if (!range)
      return ETK_FALSE;

   new_value = ETK_CLAMP(value, range->lower, range->upper - range->page_size);
   if (new_value != range->value)
   {
      range->value = new_value;
      etk_signal_emit(ETK_RANGE_VALUE_CHANGED_SIGNAL, ETK_OBJECT(range), range->value);
      etk_object_notify(ETK_OBJECT(range), "value");
      return ETK_TRUE;
   }

   return ETK_FALSE;
}

/**
 * @brief Gets the value of the range
 * @param range a range
 * @return Returns the value of the range
 */
double etk_range_value_get(Etk_Range *range)
{
   if (!range)
      return 0.0;
   return range->value;
}

/**
 * @brief Sets the range of values that the range widget can take
 * @param range a range
 * @param lower the lower bound
 * @param upper the upper bound
 */
void etk_range_range_set(Etk_Range *range, double lower, double upper)
{
   if (!range)
      return;

   if (upper < lower + range->page_size)
      upper = lower + range->page_size;

   if (range->lower != lower)
   {
      range->lower = lower;
      etk_object_notify(ETK_OBJECT(range), "lower");
   }
   if (range->upper != upper)
   {
      range->upper = upper;
      etk_object_notify(ETK_OBJECT(range), "upper");
   }

   etk_range_value_set(range, range->value);
}

/**
 * @brief Gets the range of values that the range widget can take
 * @param range a range
 * @param lower the location where to store the lower bound
 * @param upper the location where to store the upper bound
 */
void etk_range_range_get(Etk_Range *range, double *lower, double *upper)
{
   if (lower)
      *lower = range ? range->lower : 0.0;
   if (upper)
      *upper = range ? range->upper : 0.0;
}

/**
 * @brief Sets the increment values of the range
 * @param range a range
 * @param step the step increment value. Used when a arrow button of a scrollbar is clicked, or when the keyboard
 * arrows are pressed
 * @param page the page increment value. Used when the trough of a scrollbar is clicked, or when the page up/down keys
 * are pressed
 */
void etk_range_increments_set(Etk_Range *range, double step, double page)
{
   if (!range)
      return;

   if (range->step_increment != step)
   {
      range->step_increment = step;
      etk_object_notify(ETK_OBJECT(range), "step-increment");
   }
   if (range->page_increment != page)
   {
      range->page_increment = page;
      etk_object_notify(ETK_OBJECT(range), "page-increment");
   }
}

/**
 * @brief Gets the increment values of the range
 * @param range a range
 * @param step the location where to store the step increment value
 * @param page the location where to store the page increment value
 */
void etk_range_increments_get(Etk_Range *range, double *step, double *page)
{
   if (step)
      *step = range ? range->step_increment : 0.0;
   if (page)
      *page = range ? range->page_increment : 0.0;
}

/**
 * @brief Sets the page size of the range: this value controls the size of the drag button of a scrollbar for example
 * @param range a range
 * @param page_size the value to set
 */
void etk_range_page_size_set(Etk_Range *range, double page_size)
{
   if (!range)
      return;

   page_size = ETK_MIN(page_size, range->upper - range->lower);
   if (page_size != range->page_size)
   {
      range->page_size = page_size;
      etk_object_notify(ETK_OBJECT(range), "page-size");
   }
   etk_range_value_set(range, range->value);
}

/**
 * @brief Gets the page size of the range
 * @param range a range
 * @return Returns the page size of the range
 */
double etk_range_page_size_get(Etk_Range *range)
{
   if (!range)
      return 0.0;
   return range->page_size;
}


/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the range */
static void _etk_range_constructor(Etk_Range *range)
{
   if (!range)
      return;

   range->lower = 0.0;
   range->upper = 0.0;
   range->value = 0.0;
   range->step_increment = 0.0;
   range->page_increment = 0.0;
   range->page_size = 0.0;
   range->value_changed_handler = NULL;
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_range_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Range *range;

   if (!(range = ETK_RANGE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_RANGE_LOWER_PROPERTY:
         etk_range_range_set(range, etk_property_value_double_get(value), range->upper);
         break;
      case ETK_RANGE_UPPER_PROPERTY:
         etk_range_range_set(range, range->lower, etk_property_value_double_get(value));
         break;
      case ETK_RANGE_VALUE_PROPERTY:
         etk_range_value_set(range, etk_property_value_double_get(value));
         break;
      case ETK_RANGE_STEP_INC_PROPERTY:
         etk_range_increments_set(range, etk_property_value_double_get(value), range->page_increment);
         break;
      case ETK_RANGE_PAGE_INC_PROPERTY:
         etk_range_increments_set(range, range->step_increment, etk_property_value_double_get(value));
         break;
      case ETK_RANGE_PAGE_SIZE_PROPERTY:
         etk_range_page_size_set(range, etk_property_value_double_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_range_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Range *range;

   if (!(range = ETK_RANGE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_RANGE_LOWER_PROPERTY:
         etk_property_value_double_set(value, range->lower);
         break;
      case ETK_RANGE_UPPER_PROPERTY:
         etk_property_value_double_set(value, range->upper);
         break;
      case ETK_RANGE_VALUE_PROPERTY:
         etk_property_value_double_set(value, range->value);
         break;
      case ETK_RANGE_STEP_INC_PROPERTY:
         etk_property_value_double_set(value, range->step_increment);
         break;
      case ETK_RANGE_PAGE_INC_PROPERTY:
         etk_property_value_double_set(value, range->page_increment);
         break;
      case ETK_RANGE_PAGE_SIZE_PROPERTY:
         etk_property_value_double_set(value, range->page_size);
         break;
      default:
         break;
   }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Range
 *
 * A range has a <b>value</b> that can't be lower than the <b>lower bound</b> of the range, or greater than the
 * <b>upper bound</b> of the range. @n
 * Three other value have to be set when a new range is created:
 * - the <b>"step increment"</b> value is the value added or substracted to the current value of the range, when the
 * arrow keys are used, or when the arrow buttons of a scrollbar are clicked for example. It is also the value used
 * when the user uses the mouse wheel.
 * - the <b>"page increment"</b> value is the value added or substracted to current value of the range, when the page
 * up/down keys are pressed, or when the trough of a scrollbar is clicked for example.
 * - the <b>"page size"</b> value is the size of viewport, used to calculate the size of the drag button of a scrollbar
 * @n @n
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Range
 *
 * \par Signals:
 * @signal_name "value-changed": Emitted when the value of the range is changed
 * @signal_cb Etk_Bool callback(Etk_Range *range, double value, void *data)
 * @signal_arg range: the range whose value has been changed
 * @signal_arg value: the new value of the range
 * @signal_data
 *
 * \par Properties:
 * @prop_name "value": The value of the range
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.0
 * \par
 * @prop_name "lower": The lower bound of the range. The value of the range can't be lower than its lower bound
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.0
 * \par
 * @prop_name "upper": The upper bound of the range. The value of the range can't be greater than its upper bound
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.0
 * \par
 * @prop_name "step-increment": The step increment value of the range. See the description of Etk_Range for more info
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.0
 * \par
 * @prop_name "page-increment": The page increment value of the range. See the description of Etk_Range for more info
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.0
 * \par
 * @prop_name "page-size": The page-size of the range. See the description of Etk_Range for more info
 * @prop_type Double
 * @prop_rw
 * @prop_val 0.0
 */
