/** @file etk_vbox.h */
#ifndef _ETK_VBOX_H_
#define _ETK_VBOX_H_

#include "etk_box.h"
#include "etk_types.h"

/**
 * @defgroup Etk_VBox Etk_VBox
 * @{
 */

/** @brief Gets the type of a vbox */
#define ETK_VBOX_TYPE       (etk_vbox_type_get())
/** @brief Casts the object to an Etk_VBox */
#define ETK_VBOX(obj)       (ETK_OBJECT_CAST((obj), ETK_VBOX_TYPE, Etk_VBox))
/** @brief Checks if the object is an Etk_VBox */
#define ETK_IS_VBOX(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VBOX_TYPE))

/**
 * @struct Etk_VBox
 * @brief An Etk_VBox is a container that can contain several children packed in the vertical direction 
 */
struct _Etk_VBox
{
   /* private: */
   /* Inherit from Etk_Box */
   Etk_Box box;

   int *requested_sizes;
};

Etk_Type *etk_vbox_type_get();
Etk_Widget *etk_vbox_new(Etk_Bool homogeneous, int spacing);

/** @} */

#endif
