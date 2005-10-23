/** @file etk_scale.h */
#ifndef _ETK_SCALE_H_
#define _ETK_SCALE_H_

#include "etk_range.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Scale Etk_Scale
 * @{
 */

/** @brief Gets the type of a scale */
#define ETK_SCALE_TYPE       (etk_scale_type_get())
/** @brief Casts the object to an Etk_Scale */
#define ETK_SCALE(obj)       (ETK_OBJECT_CAST((obj), ETK_SCALE_TYPE, Etk_Scale))
/** @brief Checks if the object is an Etk_Scale */
#define ETK_IS_SCALE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SCALE_TYPE))

/** @brief Gets the type of a horizontal scale */
#define ETK_HSCALE_TYPE       (etk_hscale_type_get())
/** @brief Casts the object to an Etk_HScale */
#define ETK_HSCALE(obj)       (ETK_OBJECT_CAST((obj), ETK_HSCALE_TYPE, Etk_HScale))
/** @brief Checks if the object is an Etk_HScale */
#define ETK_IS_HSCALE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HSCALE_TYPE))

/** @brief Gets the type of a vertical scale */
#define ETK_VSCALE_TYPE       (etk_vscale_type_get())
/** @brief Casts the object to an Etk_VScale */
#define ETK_VSCALE(obj)       (ETK_OBJECT_CAST((obj), ETK_VSCALE_TYPE, Etk_VScale))
/** @brief Checks if the object is an Etk_VScale */
#define ETK_IS_VSCALE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VSCALE_TYPE))

/**
 * @struct Etk_Scale
 * @brief An Etk_Scale is a widget with a cursor you can move to change a value
 */
struct _Etk_Scale
{
   /* private: */
   /* Inherit from Etk_Range */
   Etk_Range range;
};

/**
 * @struct Etk_HScale
 * @brief An horizontal scale
 */
struct _Etk_HScale
{
   /* private: */
   /* Inherit from Etk_Scale */
   Etk_Scale scale;
};

/**
 * @struct Etk_VScale
 * @brief A vertical scale
 */
struct _Etk_VScale
{
   /* private: */
   /* Inherit from Etk_Scale */
   Etk_Scale scale;
};

Etk_Type *etk_scale_type_get();

Etk_Type *etk_hscale_type_get();
Etk_Widget *etk_hscale_new(double lower, double upper, double value, double step_increment, double page_increment);

Etk_Type *etk_vscale_type_get();
Etk_Widget *etk_vscale_new(double lower, double upper, double value, double step_increment, double page_increment);

/** @} */

#endif
