/** @file etk_scrollbar.h */
#ifndef _ETK_SCROLLBAR_H_
#define _ETK_SCROLLBAR_H_

#include <Evas.h>
#include <Ecore.h>
#include "etk_range.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Scrollbar Etk_Scrollbar
 * @{
 */

/** @brief Gets the type of a scrollbar */
#define ETK_SCROLLBAR_TYPE       (etk_scrollbar_type_get())
/** @brief Casts the object to an Etk_Scrollbar */
#define ETK_SCROLLBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_SCROLLBAR_TYPE, Etk_Scrollbar))
/** @brief Checks if the object is an Etk_Scrollbar */
#define ETK_IS_SCROLLBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SCROLLBAR_TYPE))

/** @brief Gets the type of a horizontal scrollbar */
#define ETK_HSCROLLBAR_TYPE       (etk_hscrollbar_type_get())
/** @brief Casts the object to an Etk_HScrollbar */
#define ETK_HSCROLLBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_HSCROLLBAR_TYPE, Etk_HScrollbar))
/** @brief Checks if the object is an Etk_HScrollbar */
#define ETK_IS_HSCROLLBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_HSCROLLBAR_TYPE))

/** @brief Gets the type of a vertical scrollbar */
#define ETK_VSCROLLBAR_TYPE       (etk_vscrollbar_type_get())
/** @brief Casts the object to an Etk_VScrollbar */
#define ETK_VSCROLLBAR(obj)       (ETK_OBJECT_CAST((obj), ETK_VSCROLLBAR_TYPE, Etk_VScrollbar))
/** @brief Checks if the object is an Etk_VScrollbar */
#define ETK_IS_VSCROLLBAR(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VSCROLLBAR_TYPE))

/**
 * @struct Etk_Scrollbar
 * @brief An Etk_Scrollbar is used mainly in Etk_Scrolled_Window
 */
struct _Etk_Scrollbar
{
   /* private: */
   /* Inherit from Etk_Range */
   Etk_Range range;

   int drag_size;
   int confine_size;
   double page_size;

   Evas_Object *drag_button;
   double drag_orig_value;
   int drag_orig_position;

   Ecore_Timer *scrolling_timer;
   Etk_Bool first_scroll;
};

/**
 * @struct Etk_HScrollbar
 * @brief An horizontal scrollbar
 */
struct _Etk_HScrollbar
{
   /* private: */
   /* Inherit from Etk_Scrollbar */
   Etk_Scrollbar scrollbar;
};

/**
 * @struct Etk_VScrollbar
 * @brief A vertical scrollbar
 */
struct _Etk_VScrollbar
{
   /* private: */
   /* Inherit from Etk_Scrollbar */
   Etk_Scrollbar scrollbar;
};

Etk_Type *etk_scrollbar_type_get();

Etk_Type *etk_hscrollbar_type_get();
Etk_Widget *etk_hscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size);

Etk_Type *etk_vscrollbar_type_get();
Etk_Widget *etk_vscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size);

void etk_scrollbar_page_size_set(Etk_Scrollbar *scrollbar, double page_size);
double etk_scrollbar_page_size_get(Etk_Scrollbar *scrollbar);

/** @} */

#endif
