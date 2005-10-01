/** @file etk_adjustment.h */
#ifndef _ETK_ADJUSTMENT_H_
#define _ETK_ADJUSTMENT_H_

#include "etk_object.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Adjustment Etk_Adjustment
 * @{
 */

/** @brief Gets the type of an adjustment */
#define ETK_ADJUSTMENT_TYPE       (etk_adjustment_type_get())
/** @brief Casts the object to an Etk_Adjustment */
#define ETK_ADJUSTMENT(obj)       (ETK_OBJECT_CAST((obj), ETK_ADJUSTMENT_TYPE, Etk_Adjustment))
/** @brief Checks if the object is an Etk_Adjustment */
#define ETK_IS_ADJUSTMENT(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ADJUSTMENT_TYPE))

struct _Etk_Adjustment
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;

   double value;

   double lower;
   double upper;

   double step_increment;
   double page_increment;
   double page_size;
};

Etk_Type *etk_adjustment_type_get();
Etk_Object *etk_adjustment_new(double value, double lower, double upper, double step_increment, double page_increment, double    );

double etk_adjustment_value_get(Etk_Adjustment *adjustment);
void etk_adjustment_value_set(Etk_Adjustment *adjustment, double value);

void etk_adjustment_clamp_page(Etk_Adjustment *adjustment, double lower, double upper);

void etk_adjustment_changed(Etk_Adjustment *adjustment);
void etk_adjustment_value_changed(Etk_Adjustment *adjustment);

/** @} */

#endif
