/** @file etk_bin.h */
#ifndef _ETK_BIN_H_
#define _ETK_BIN_H_

#include "etk_container.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Bin Etk_Bin
 * @{
 */

/** @brief Gets the type of a bin */
#define ETK_BIN_TYPE       (etk_bin_type_get())
/** @brief Casts the object to an Etk_Bin */
#define ETK_BIN(obj)       (ETK_OBJECT_CAST((obj), ETK_BIN_TYPE, Etk_Bin))
/** @brief Checks if the object is an Etk_Bin */
#define ETK_IS_BIN(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_BIN_TYPE))

/**
 * @struct Etk_Bin
 * @brief An Etk_Bin is a container that can contain only one child
 */
struct _Etk_Bin
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Etk_Widget *child;
   Evas_List *child_list;
};

Etk_Type *etk_bin_type_get();

Etk_Widget *etk_bin_child_get(Etk_Bin *bin);
void etk_bin_child_set(Etk_Bin *bin, Etk_Widget *child);

/** @} */

#endif
