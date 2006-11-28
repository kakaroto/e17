/** @file etk_spinner.h */
#ifndef _ETK_SPINNER_H_
#define _ETK_SPINNER_H_

#include "etk_range.h"
#include <Ecore.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Spinner Etk_Spinner
 * @brief TODOC
 * @{
 */

/** Gets the type of a spinner */
#define ETK_SPINNER_TYPE       (etk_spinner_type_get())
/** Casts the object to an Etk_Spinner */
#define ETK_SPINNER(obj)       (ETK_OBJECT_CAST((obj), ETK_SPINNER_TYPE, Etk_Spinner))
/** Checks if the object is an Etk_Spinner */
#define ETK_IS_SPINNER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SPINNER_TYPE))

/**
 * @brief @widget TODOC
 * @structinfo
 */
struct Etk_Spinner
{
   /* private: */
   /* Inherit from Etk_Range */
   Etk_Range range;

   Evas_Object *editable_object;
   Etk_Bool selection_dragging;
   
   int digits;
   char value_format[16];
   int successive_steps;
   Ecore_Timer *step_timer;
};


Etk_Type   *etk_spinner_type_get();
Etk_Widget *etk_spinner_new(double lower, double upper, double value, double step_increment, double page_increment);

void etk_spinner_digits_set(Etk_Spinner *spinner, int digits);
int  etk_spinner_digits_get(Etk_Spinner *spinner);

/** @} */

#endif
