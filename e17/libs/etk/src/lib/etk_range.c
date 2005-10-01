/** @file etk_range.c */
#include "etk_range.h"
#include <stdlib.h>
#include <string.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"

/**
 * @addtogroup Etk_Range
* @{
 */
#if 0
enum _Etk_Range_Signal_Id
{
   ETK_RANGE_NUM_SIGNALS
};

enum _Etk_Range_Property_Id
{
   ETK_RANGE_ADJUSTMENT_PROPERTY
};

static void _etk_range_constructor(Etk_Range *range);
static void _etk_range_destructor(Etk_Range *range);
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
      range_type = etk_type_new("Etk_Range", ETK_WIDGET_TYPE, sizeof(Etk_Range), ETK_CONSTRUCTOR(_etk_range_constructor), ETK_DESTRUCTOR(_etk_range_destructor), NULL);

      etk_type_property_add(range_type, "adjustment", ETK_RANGE_ADJUSTMENT_PROPERTY, ETK_PROPERTY_POINTER, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));

      range_type->property_set = _etk_range_property_set;
      range_type->property_get = _etk_range_property_get;
   }

   return range_type;
}

/**
 * @brief Creates a new range
 * @return Returns the new range widget
 */
Etk_Widget *etk_range_new()
{
   return etk_widget_new(ETK_RANGE_TYPE, "theme_group", "range", NULL);
}

/**
 * @brief Sets the adjustment of the range
 * @param range a range
 * @param adjustment the adjustment to set
 */
void etk_range_adjustment_set(Etk_Range *range, Etk_Adjustment *adjustment)
{
   Etk_Widget *widget;

   if (!(widget = ETK_WIDGET(range)))
      return;

   if (range->adjustment == adjustment)
     return;

   if (range->adjustment)
     {
        etk_object_unref(ETK_OBJECT(range->adjustment));

        range->adjustment = adjustment;
        etk_object_notify(ETK_OBJECT(range), "adjustment");
     }
}

/**
 * @brief Gets the adjustment of the range
 * @param range a range
 * @return Returns the adjustment of the range
 */
Etk_Adjustment *etk_range_adjustment_get(Etk_Range *range)
{
   if (!range)
      return NULL;

   return range->adjustment;
}

double etk_range_value_get(Etk_Range *range)
{
   if (!range)
      return 0.0;

   return etk_adjustment_value_get(range->adjustment);
}

void etk_range_value_set(Etk_Range   *range,
                         const double value)
{
   if (!range)
      return;

   etk_adjustment_value_set(range->adjustment, value);
}

void etk_range_range_set(Etk_Range   *range,
                         const double lower,
                         const double upper)
{
   if (!range)
      return;

   if (lower > upper)
      return;

   etk_adjustment_clamp_page(range->adjustment, lower, upper);
   etk_adjustment_changed(range->adjustment);
}

void etk_range_increments_set(Etk_Range   *range,
                              const double step,
                              const double page)
{
   if (!range)
      return;

   range->adjustment->step_increment = step;
   range->adjustment->page_increment = page;
   
   etk_adjustment_changed(range->adjustment);
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

   range->adjustment = ETK_ADJUSTMENT(etk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
}

/* Destroys the range */
static void _etk_range_destructor(Etk_Range *range)
{
   if (!range)
      return;

   etk_object_destroy(ETK_OBJECT(range->adjustment));
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_range_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Range *range;

   if (!(range = ETK_RANGE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_RANGE_ADJUSTMENT_PROPERTY:
         etk_range_adjustment_set(range, ETK_ADJUSTMENT(etk_property_value_string_get(value)));
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
      case ETK_RANGE_ADJUSTMENT_PROPERTY:
         etk_property_value_pointer_set(value, range->adjustment);
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

#endif

/** @} */
