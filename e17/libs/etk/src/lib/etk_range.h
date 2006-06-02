/** @file etk_range.h */
#ifndef _ETK_RANGE_H_
#define _ETK_RANGE_H_

#include "etk_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Range Etk_Range
 * @brief Etk_Range is an abstract class used by widgets like scrollbars or sliders
 * @{
 */

/** Gets the type of a range */
#define ETK_RANGE_TYPE       (etk_range_type_get())
/** Casts the object to an Etk_Range */
#define ETK_RANGE(obj)       (ETK_OBJECT_CAST((obj), ETK_RANGE_TYPE, Etk_Range))
/** Checks if the object is an Etk_Range */
#define ETK_IS_RANGE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_RANGE_TYPE))

/**
 * @brief @widget The structure of a range
 * @structinfo
 */
struct Etk_Range
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   double lower;
   double upper;
   double value;

   double step_increment;
   double page_increment;
   double page_size;

   void (*value_changed)(Etk_Range *range, double value);
};

Etk_Type *etk_range_type_get();

void etk_range_value_set(Etk_Range *range, double value);
double etk_range_value_get(Etk_Range *range);

void etk_range_range_set(Etk_Range *range, double lower, double upper);
void etk_range_range_get(Etk_Range *range, double *lower, double *upper);

void etk_range_increments_set(Etk_Range *range, double step, double page);
void etk_range_increments_get(Etk_Range *range, double *step, double *page);

void etk_range_page_size_set(Etk_Range *range, double page_size);
double etk_range_page_size_get(Etk_Range *range);

/** @} */

#endif
