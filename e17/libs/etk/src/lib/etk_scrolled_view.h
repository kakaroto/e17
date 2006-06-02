/** @file etk_scrolled_view.h */
#ifndef _ETK_SCROLLED_VIEW_H_
#define _ETK_SCROLLED_VIEW_H_

#include "etk_bin.h"
#include "etk_scrollbar.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Scrolled_View Etk_Scrolled_View
 * @brief The Etk_Scrolled_View widget is a container with two scrollbars that allow the user to scroll its unique child
 * @{
 */

/** Gets the type of a scrolled_view */
#define ETK_SCROLLED_VIEW_TYPE       (etk_scrolled_view_type_get())
/** Casts the object to an Etk_Scrolled_View */
#define ETK_SCROLLED_VIEW(obj)       (ETK_OBJECT_CAST((obj), ETK_SCROLLED_VIEW_TYPE, Etk_Scrolled_View))
/** Checks if the object is an Etk_Scrolled_View */
#define ETK_IS_SCROLLED_VIEW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SCROLLED_VIEW_TYPE))

/**
 * @brief Etk_Scrolled_View_Policy describes whether the scrollbar should be always visible, always hidden, 
 * or whether it should be shown/hidden automatically
 */
typedef enum Etk_Scrolled_View_Policy
{
   ETK_POLICY_SHOW,     /**< The scrollbar is always visible */
   ETK_POLICY_HIDE,     /**< The scrollbar is always hidden */
   ETK_POLICY_AUTO      /**< The scrollbar is shown and hidden automatically whether or not the child can fit
                         * entirely in the scrolled view */
} Etk_Scrolled_View_Policy;

/**
 * @brief @widget The structure of a scrolled view
 * @structinfo
 */
struct Etk_Scrolled_View
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Etk_Widget *hscrollbar;
   Etk_Widget *vscrollbar;

   Etk_Scrolled_View_Policy hpolicy;
   Etk_Scrolled_View_Policy vpolicy;
};

Etk_Type *etk_scrolled_view_type_get();
Etk_Widget *etk_scrolled_view_new();

Etk_Range *etk_scrolled_view_hscrollbar_get(Etk_Scrolled_View *scrolled_view);
Etk_Range *etk_scrolled_view_vscrollbar_get(Etk_Scrolled_View *scrolled_view);

void etk_scrolled_view_add_with_viewport(Etk_Scrolled_View *scrolled_view, Etk_Widget *child);

void etk_scrolled_view_policy_set(Etk_Scrolled_View *scrolled_view, Etk_Scrolled_View_Policy hpolicy, Etk_Scrolled_View_Policy vpolicy);
void etk_scrolled_view_policy_get(Etk_Scrolled_View *scrolled_view, Etk_Scrolled_View_Policy *hpolicy, Etk_Scrolled_View_Policy *vpolicy);

/** @} */

#endif
