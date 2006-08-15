/** @file etk_bin.h */
#ifndef _ETK_BIN_H_
#define _ETK_BIN_H_

#include "etk_container.h"
#include "etk_types.h"

/* TODO: Etk_Bin
 - The "swallow" code should be included in Etk_Widget.c
 */

/**
 * @defgroup Etk_Bin Etk_Bin
 * @brief The Etk_Bin widget is a container that can contain only one child
 * @{
 */

/** Gets the type of a bin */
#define ETK_BIN_TYPE       (etk_bin_type_get())
/** Casts the object to an Etk_Bin */
#define ETK_BIN(obj)       (ETK_OBJECT_CAST((obj), ETK_BIN_TYPE, Etk_Bin))
/** Checks if the object is an Etk_Bin */
#define ETK_IS_BIN(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_BIN_TYPE))

/**
 * @brief @widget A container that can contain only one child
 * @structinfo
 */
struct Etk_Bin
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Etk_Widget *child;
};

Etk_Type *etk_bin_type_get();

Etk_Widget *etk_bin_child_get(Etk_Bin *bin);
void etk_bin_child_set(Etk_Bin *bin, Etk_Widget *child);

/** @} */

#endif
