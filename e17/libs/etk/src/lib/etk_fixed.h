/** @file etk_fixed.h */
#ifndef _ETK_FIXED_H_
#define _ETK_FIXED_H_

#include "etk_container.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Fixed Etk_Fixed
 * @brief The Etk_Fixed widget is a container which allows you to position widgets at fixed coordinates
 * @{
 */

/** Gets the type of a fixed */
#define ETK_FIXED_TYPE       (etk_fixed_type_get())
/** Casts the object to an Etk_Fixed */
#define ETK_FIXED(obj)       (ETK_OBJECT_CAST((obj), ETK_FIXED_TYPE, Etk_Fixed))
/** Checks if the object is an Etk_Fixed */
#define ETK_IS_FIXED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_FIXED_TYPE))

/**
 * @brief @widget A container which allows you to position widgets at fixed coordinates
 * @structinfo
 */
struct Etk_Fixed
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;
   
   Evas_List *children;
   Evas_Object *clip;
};

Etk_Type   *etk_fixed_type_get(void);
Etk_Widget *etk_fixed_new(void);

void etk_fixed_put(Etk_Fixed *fixed, Etk_Widget *widget, int x, int y);
void etk_fixed_move(Etk_Fixed *fixed, Etk_Widget *widget, int x, int y);
void etk_fixed_child_position_get(Etk_Fixed *fixed, Etk_Widget *widget, int *x, int *y);

/** @} */

#endif
