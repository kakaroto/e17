/** @file etk_adjustment.c */
#include <stdlib.h>
#include "etk_adjustment.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Adjustment
 * @{
 */

enum _Etk_Adjustment_Signal_Id
{
   ETK_ADJUSTMENT_CHANGED_SIGNAL,
   ETK_ADJUSTMENT_VALUE_CHANGED_SIGNAL,
   ETK_ADJUSTMENT_NUM_SIGNALS
};

enum _Etk_Adjustment_Property_Id
{
   ETK_ADJUSTMENT_VALUE_PROPERTY,
   ETK_ADJUSTMENT_LOWER_PROPERTY,
   ETK_ADJUSTMENT_UPPER_PROPERTY,
   ETK_ADJUSTMENT_STEP_INCREMENT_PROPERTY,
   ETK_ADJUSTMENT_PAGE_INCREMENT_PROPERTY,
   ETK_ADJUSTMENT_PAGE_SIZE_PROPERTY
};

static void _etk_adjustment_constructor(Etk_Adjustment *adjustment);
static void _etk_adjustment_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_adjustment_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static Etk_Signal *_etk_adjustment_signals[ETK_ADJUSTMENT_NUM_SIGNALS];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Adjustment
 * @return Returns the type on an Etk_Adjustment
 */
Etk_Type *etk_adjustment_type_get()
{
   static Etk_Type *adjustment_type = NULL;

   if (!adjustment_type)
   {
      adjustment_type = etk_type_new("Etk_Adjustment", ETK_OBJECT_TYPE, sizeof(Etk_Adjustment),
         ETK_CONSTRUCTOR(_etk_adjustment_constructor), NULL, NULL);

      _etk_adjustment_signals[ETK_ADJUSTMENT_CHANGED_SIGNAL] = etk_signal_new("changed", adjustment_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_adjustment_signals[ETK_ADJUSTMENT_VALUE_CHANGED_SIGNAL] = etk_signal_new("value_changed", adjustment_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);

      etk_type_property_add(adjustment_type, "value", ETK_ADJUSTMENT_VALUE_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(adjustment_type, "lower", ETK_ADJUSTMENT_LOWER_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(adjustment_type, "upper", ETK_ADJUSTMENT_UPPER_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(adjustment_type, "step_increment", ETK_ADJUSTMENT_STEP_INCREMENT_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(adjustment_type, "page_increment", ETK_ADJUSTMENT_PAGE_INCREMENT_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));
      etk_type_property_add(adjustment_type, "page_size", ETK_ADJUSTMENT_PAGE_SIZE_PROPERTY, ETK_PROPERTY_DOUBLE, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_double(0.0));

      adjustment_type->property_set = _etk_adjustment_property_set;
      adjustment_type->property_get = _etk_adjustment_property_get;
   }

   return adjustment_type;
}

/**
 * @brief Creates a new adjustment
 * @param value the value of the new adjustment
 * @param lower the lower bound of the new adjustment
 * @param upper the upper bound of the new adjustment
 * @param step_increment the step increment of the new adjustment: for example, in a scrollbar, it's the increment value used when the user clicks on the arrows
 * @param page_increment the step increment of the new adjustment: for example, in a scrollbar, it's the increment value used when the user clicks in the through
 * @param page_size the size of the visible area in a scrollbar
 * @return Returns the new adjustment
 */
Etk_Object *etk_adjustment_new(double value, double lower, double upper, double step_increment, double page_increment, double page_size)
{
   return etk_object_new(ETK_ADJUSTMENT_TYPE, "value", value, "lower", lower, "upper", upper,
      "step-increment", step_increment, "page-increment", page_increment, "page-size", page_size, NULL);
}

/**
 * @brief Sets the value of the adjustment
 * @param adjustment a adjustment
 * @param value the value to set
 */
void etk_adjustment_value_set(Etk_Adjustment *adjustment, double value)
{
   if (!adjustment)
      return;

   if (value < adjustment->lower)
      value = adjustment->lower;
   if (value > adjustment->upper)
      value = adjustment->upper;

   /* Update only if necessary */
   if (adjustment->value == value)
     return;

   adjustment->value = value;
   etk_adjustment_value_changed(adjustment);
}

/**
 * @brief Gets the value of the adjustment
 * @param adjustment a adjustment
 * @return Returns the value of the adjustment
 */
double etk_adjustment_value_get(Etk_Adjustment *adjustment)
{
   if (!adjustment)
      return 0.0;
   return adjustment->value;
}

/**
 * @brief Changes the value of the adjustment to make sure than the range [lower;upper] is in the page @n
 * (between adjustment->value and adjustment->value + adjustment->page_size)
 * @param adjustment an adjustment
 * @param lower the lower bound of the range
 * @param upper the upper bound of the range
 */
void etk_adjustment_clamp_page(Etk_Adjustment *adjustment, double lower, double upper)
{
   if (!adjustment)
      return;

   /* Update only if necessary */
   if (lower > upper)
     return;

   if (lower < adjustment->lower)
      lower = adjustment->lower;
   if (upper > adjustment->upper)
      upper = adjustment->upper;

   if (adjustment->value + adjustment->page_size < upper)
   {
      adjustment->value = upper - adjustment->page_size;
      etk_adjustment_value_changed(adjustment);
   }
   if (adjustment->value > lower)
   {
      adjustment->value = lower;
      etk_adjustment_value_changed(adjustment);
   }
}

/**
 * @brief Emits the "changed" signal
 * @param adjustment an adjustment
 */
void etk_adjustment_changed(Etk_Adjustment *adjustment)
{
   if (!adjustment)
      return;

   etk_signal_emit(_etk_adjustment_signals[ETK_ADJUSTMENT_CHANGED_SIGNAL], ETK_OBJECT(adjustment), NULL);
}

/**
 * @brief Emits the "value-changed" signal
 * @param adjustment a adjustment
 */
void etk_adjustment_value_changed(Etk_Adjustment *adjustment)
{
   if (!adjustment)
      return;

   etk_signal_emit(_etk_adjustment_signals[ETK_ADJUSTMENT_VALUE_CHANGED_SIGNAL], ETK_OBJECT(adjustment), NULL);
   etk_object_notify(ETK_OBJECT(adjustment), "value");
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the members */
static void _etk_adjustment_constructor(Etk_Adjustment *adjustment)
{
   if (!adjustment)
      return;

   adjustment->value = 0.0;
   adjustment->lower = 0.0;
   adjustment->upper = 0.0;
   adjustment->step_increment = 0.0;
   adjustment->page_increment = 0.0;
   adjustment->page_size = 0.0;
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_adjustment_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Adjustment *adjustment;

   if (!(adjustment = ETK_ADJUSTMENT(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_ADJUSTMENT_VALUE_PROPERTY:
         adjustment->value =  etk_property_value_double_get(value);
         etk_object_notify(ETK_OBJECT(adjustment), "value");
         break;
      case ETK_ADJUSTMENT_LOWER_PROPERTY:
         adjustment->lower = etk_property_value_double_get(value);
         etk_object_notify(ETK_OBJECT(adjustment), "lower");
         break;
      case ETK_ADJUSTMENT_UPPER_PROPERTY:
         adjustment->upper = etk_property_value_double_get(value);
         etk_object_notify(ETK_OBJECT(adjustment), "upper");
         break;
      case ETK_ADJUSTMENT_STEP_INCREMENT_PROPERTY:
         adjustment->step_increment = etk_property_value_double_get(value);
         etk_object_notify(ETK_OBJECT(adjustment), "step_increment");
         break;
      case ETK_ADJUSTMENT_PAGE_INCREMENT_PROPERTY:
         adjustment->page_increment = etk_property_value_double_get(value);
         etk_object_notify(ETK_OBJECT(adjustment), "page_increment");
         break;
      case ETK_ADJUSTMENT_PAGE_SIZE_PROPERTY:
         adjustment->page_size = etk_property_value_double_get(value);
         etk_object_notify(ETK_OBJECT(adjustment), "page_size");
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_adjustment_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Adjustment *adjustment;

   if (!(adjustment = ETK_ADJUSTMENT(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_ADJUSTMENT_VALUE_PROPERTY:
         etk_property_value_double_set(value, adjustment->value);
         break;
      case ETK_ADJUSTMENT_LOWER_PROPERTY:
         etk_property_value_double_set(value, adjustment->lower);
         break;
      case ETK_ADJUSTMENT_UPPER_PROPERTY:
         etk_property_value_double_set(value, adjustment->upper);
         break;
      case ETK_ADJUSTMENT_STEP_INCREMENT_PROPERTY:
         etk_property_value_double_set(value, adjustment->step_increment);
         break;
      case ETK_ADJUSTMENT_PAGE_INCREMENT_PROPERTY:
         etk_property_value_double_set(value, adjustment->page_increment);
         break;
      case ETK_ADJUSTMENT_PAGE_SIZE_PROPERTY:
         etk_property_value_double_set(value, adjustment->page_size);
         break;
      default:
         break;
   }
}

/** @} */
