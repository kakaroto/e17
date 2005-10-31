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
 * @struct Etk_Scrolled_View
 * @brief A scrolled_view is a bin container that have a label. It's useful to group some widgets that need to be together
 */
struct _Etk_Scrolled_View
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Etk_Widget *hscrollbar;
   Etk_Widget *vscrollbar;
};

Etk_Type *etk_scrolled_view_type_get();
Etk_Widget *etk_scrolled_view_new();

Etk_Widget *etk_scrolled_view_hscrollbar_get(Etk_Scrolled_View *scrolled_view);
Etk_Widget *etk_scrolled_view_vscrollbar_get(Etk_Scrolled_View *scrolled_view);

void etk_scrolled_view_add_with_viewport(Etk_Scrolled_View *scrolled_view, Etk_Widget *child);

/** @} */

#endif
