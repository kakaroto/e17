/** @file etk_hbox.h */
#ifndef _ETK_HBOX_H_
#define _ETK_HBOX_H_

#include "etk_box.h"
#include "etk_types.h"

/**
 * @defgroup Etk_HBox Etk_HBox
 * @{
 */

/** @brief Gets the type of a hbox */
#define ETK_HBOX_TYPE       (etk_hbox_type_get())
/** @brief Casts the object to an Etk_HBox */
#define ETK_HBOX(obj)       (ETK_OBJECT_CAST((obj), ETK_HBOX_TYPE, Etk_HBox))
/** @brief Checks if the object is an Etk_HBox */
#define ETK_IS_HBOX(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HBOX_TYPE))

/**
 * @struct Etk_HBox
 * @brief An Etk_HBox is a container that can contain several children packed in the horizontal direction 
 */
struct _Etk_HBox
{
   /* private: */
   /* Inherit from Etk_Box */
   Etk_Box box;

   int *requested_sizes;
};

Etk_Type *etk_hbox_type_get();
Etk_Widget *etk_hbox_new(Etk_Bool homogeneous, int spacing);

/** @} */

#endif
