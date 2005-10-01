/** @file etk_range.h */
#ifndef _ETK_RANGE_H_
#define _ETK_RANGE_H_

#include "etk_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Range Etk_Range
 * @{
 */

/** @brief Gets the type of a range */
#define ETK_RANGE_TYPE       (etk_range_type_get())
/** @brief Casts the object to an Etk_Range */
#define ETK_RANGE(obj)       (ETK_OBJECT_CAST((obj), ETK_RANGE_TYPE, Etk_Range))
/** @brief Checks if the object is an Etk_Range */
#define ETK_IS_RANGE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_RANGE_TYPE))

struct _Etk_Range
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
   
   Etk_Adjustment *adjustment;
};
/*
Etk_Type *etk_range_type_get();
Etk_Widget *etk_range_new();

Etk_Adjustment *etk_range_adjustment_get(Etk_Range *range);
void etk_range_adjustment_set (Etk_Range *range, Etk_Adjustment *adjustment);

double etk_range_value_get(Etk_Range *range);
void etk_range_value_set(Etk_Range *range, double value);

void etk_range_range_set(Etk_Range *range, double lower, double upper);

void etk_range_increments_set(Etk_Range *range, const double step, const double page);*/


/** @} */

#endif
