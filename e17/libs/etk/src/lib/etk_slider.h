/** @file etk_slider.h */
#ifndef _ETK_SLIDER_H_
#define _ETK_SLIDER_H_

#include "etk_range.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Slider Etk_Slider
 * @brief An Etk_Slider is a widget with a cursor that the user can drag to change its value
 * @{
 */

/** Gets the type of a slider */
#define ETK_SLIDER_TYPE       (etk_slider_type_get())
/** Casts the object to an Etk_Slider */
#define ETK_SLIDER(obj)       (ETK_OBJECT_CAST((obj), ETK_SLIDER_TYPE, Etk_Slider))
/** Checks if the object is an Etk_Slider */
#define ETK_IS_SLIDER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SLIDER_TYPE))

/** Gets the type of a horizontal slider */
#define ETK_HSLIDER_TYPE       (etk_hslider_type_get())
/** Casts the object to an Etk_HSlider */
#define ETK_HSLIDER(obj)       (ETK_OBJECT_CAST((obj), ETK_HSLIDER_TYPE, Etk_HSlider))
/** Checks if the object is an Etk_HSlider */
#define ETK_IS_HSLIDER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HSLIDER_TYPE))

/** Gets the type of a vertical slider */
#define ETK_VSLIDER_TYPE       (etk_vslider_type_get())
/** Casts the object to an Etk_VSlider */
#define ETK_VSLIDER(obj)       (ETK_OBJECT_CAST((obj), ETK_VSLIDER_TYPE, Etk_VSlider))
/** Checks if the object is an Etk_VSlider */
#define ETK_IS_VSLIDER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VSLIDER_TYPE))

/**
 * @brief A widget with a cursor that the user can drag to change its value
 * @structinfo
 */
struct Etk_Slider
{
   /* private: */
   /* Inherit from Etk_Range */
   Etk_Range range;
   
   char *format;
   Etk_Bool dragging;
};

/**
 * @brief An horizontal slider
 * @structinfo
 */
struct Etk_HSlider
{
   /* private: */
   /* Inherit from Etk_Slider */
   Etk_Slider slider;
};

/**
 * @brief A vertical slider
 * @structinfo
 */
struct Etk_VSlider
{
   /* private: */
   /* Inherit from Etk_Slider */
   Etk_Slider slider;
};

Etk_Type *etk_slider_type_get(void);
Etk_Type *etk_vslider_type_get(void);
Etk_Type *etk_hslider_type_get(void);

Etk_Widget *etk_hslider_new(double lower, double upper, double value, double step_increment, double page_increment);
Etk_Widget *etk_vslider_new(double lower, double upper, double value, double step_increment, double page_increment);

void        etk_slider_label_set(Etk_Slider *slider, const char *label_format);
const char *etk_slider_label_get(Etk_Slider *slider);

/** @} */

#endif
