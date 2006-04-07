/** @file etk_separator.h */
#ifndef _ETK_SEPARATOR_H_
#define _ETK_SEPARATOR_H_

#include "etk_widget.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Separator Etk_Separator
 * @{
 */

/** @brief Gets the type of a hseparator */
#define ETK_HSEPARATOR_TYPE       (etk_hseparator_type_get())
/** @brief Casts the object to an Etk_HSeparator */
#define ETK_HSEPARATOR(obj)       (ETK_OBJECT_CAST((obj), ETK_HSEPARATOR_TYPE, Etk_HSeparator))
/** @brief Checks if the object is an Etk_HSeparator */
#define ETK_IS_HSEPARATOR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HSEPARATOR_TYPE))

/** @brief Gets the type of a vseparator */
#define ETK_VSEPARATOR_TYPE       (etk_vseparator_type_get())
/** @brief Casts the object to an Etk_VSeparator */
#define ETK_VSEPARATOR(obj)       (ETK_OBJECT_CAST((obj), ETK_VSEPARATOR_TYPE, Etk_VSeparator))
/** @brief Checks if the object is an Etk_VSeparator */
#define ETK_IS_VSEPARATOR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VSEPARATOR_TYPE))

/**
 * @struct Etk_HSeparator
 * @brief An horizontal separator
 */
struct _Etk_HSeparator
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
};

/**
 * @struct Etk_VSeparator
 * @brief A vertical separator
 */
struct _Etk_VSeparator
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;
};

Etk_Type *etk_hseparator_type_get();
Etk_Widget *etk_hseparator_new();

Etk_Type *etk_vseparator_type_get();
Etk_Widget *etk_vseparator_new();

/** @} */

#endif
