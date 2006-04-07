/** @file etk_scrolled_view.h */
#ifndef _ETK_SCROLLED_VIEW_H_
#define _ETK_SCROLLED_VIEW_H_

#include "etk_bin.h"
#include "etk_scrollbar.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Scrolled_View Etk_Scrolled_View
 * @{
 */

/** @brief Gets the type of an scrolled_view */
#define ETK_SCROLLED_VIEW_TYPE       (etk_scrolled_view_type_get())
/** @brief Casts the object to an Etk_Scrolled_View */
#define ETK_SCROLLED_VIEW(obj)       (ETK_OBJECT_CAST((obj), ETK_SCROLLED_VIEW_TYPE, Etk_Scrolled_View))
/** @brief Checks if the object is an Etk_Scrolled_View */
#define ETK_IS_SCROLLED_VIEW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_SCROLLED_VIEW_TYPE))

/**
 * @enum Etk_Scrolled_View_Policy
 * @brief An Etk_Scrolled_View_Policy describes if the scrollbar should be always visible, always hidden, @n
 * or if it should be shown/hidden automatically
 */
typedef enum _Etk_Scrolled_View_Policy
{
   ETK_POLICY_SHOW,
   ETK_POLICY_HIDE,
   ETK_POLICY_AUTO
} Etk_Scrolled_View_Policy;

/**
 * @struct Etk_Scrolled_View
 * @brief A scrolled_view is a bin container which has two scrollbars that allow to scroll the child
 */
struct _Etk_Scrolled_View
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
