/** @file etk_range.c */
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

enum _Etk_Range_Signal_Id
{
   ETK_RANGE_VALUE_CHANGED_SIGNAL,
   ETK_RANGE_NUM_SIGNALS
};

enum _Etk_Range_Property_Id
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

static Etk_Signal *_etk_range_signals[ETK_RANGE_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Range
 * @return Returns the type on an Etk_Range
 */
Etk_Type *etk_range_type_get()
{
   static Etk_Type *range_type = NULL;

   if (!range_type)
   {
      range_type = etk_type_new("Etk_Range", ETK_WIDGET_TYPE, sizeof(Etk_Range), ETK_CONSTRUCTOR(_etk_range_constructor), NULL);

      _etk_range_signals[ETK_RANGE_VALUE_CHANGED_SIGNAL] = etk_signal_new("value_changed", range_type, ETK_MEMBER_OFFSET(Etk_Range, value_changed), etk_marshaller_VOID__DOUBLE, NULL, NULL);

      etk_type_property_add(range_type, "lower", ETK_RANGE_LOWER_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(range_type, "upper", ETK_RANGE_UPPER_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(range_type, "value", ETK_RANGE_VALUE_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(range_type, "step_increment", ETK_RANGE_STEP_INC_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(range_type, "page_increment", ETK_RANGE_PAGE_INC_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(range_type, "page_size", ETK_RANGE_PAGE_SIZE_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      
      range_type->property_set = _etk_range_property_set;
      range_type->property_get = _etk_range_property_get;
   }

   return range_type;
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
 * @brief Sets the value of the range.
 * @param range a range
 * @param value the value to set to the range
 */
void etk_range_value_set(Etk_Range *range, double value)
{
   double new_value;
   
   if (!range)
      return;

   new_value = ETK_CLAMP(value, range->lower, range->upper - range->page_size);
   if (new_value != range->value)
   {
      range->value = new_value;
      etk_signal_emit(_etk_range_signals[ETK_RANGE_VALUE_CHANGED_SIGNAL], ETK_OBJECT(range), NULL, range->value);
      etk_object_notify(ETK_OBJECT(range), "value");
   }
}

/**
 * @brief Sets the range of the values that the range widget can have
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
 * @brief Sets the increment value of the range
 * @param range a range
 * @param step the step increment value. Used when the arrow of a scrollbar is clicked, or when the keyboard arrows are pressed (for a scale)
 * @param page the page increment value. Used when the trough of a scrollbar is clicked, or when page up/down are pressed
 */
void etk_range_increments_set(Etk_Range *range, double step, double page)
{
   if (!range)
      return;

   if (range->step_increment != step)
   {
      range->step_increment = step;
      etk_object_notify(ETK_OBJECT(range), "step_increment");
   }
   if (range->page_increment != page)
   {
      range->page_increment = page;
      etk_object_notify(ETK_OBJECT(range), "page_increment");
   }
}

/**
 * @brief Sets the page size of the range: this value will be used by the scrollbars to know the size of the drag button
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
      etk_object_notify(ETK_OBJECT(range), "page_size");
      etk_widget_redraw_queue(ETK_WIDGET(range));
   }
}

/**
 * @brief Gets the page size of the range
 * @param range a range
 * @return Returns the page size of the range
 */
double etk_scrollbar_page_size_get(Etk_Range *range)
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

/* Initializes the members */
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

   range->value_changed = NULL;
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
