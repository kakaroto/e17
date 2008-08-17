/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_scrolled_view.h */
#ifndef _ETK_SCROLLED_VIEW_H_
#define _ETK_SCROLLED_VIEW_H_

#include "etk_bin.h"
#include "etk_scrollbar.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
typedef enum
{
   ETK_POLICY_AUTO,     /**< The scrollbar is shown and hidden automatically whether or not the child can fit
                         * entirely in the scrolled view */
   ETK_POLICY_SHOW,     /**< The scrollbar is always visible */
   ETK_POLICY_HIDE,     /**< The scrollbar is always hidden */
} Etk_Scrolled_View_Policy;

struct Etk_Scrolled_View_Mouse_Drag 
{
   Etk_Position position;
   Etk_Position prev_position[5];
   Etk_Position down_position;
   Etk_Position bar_position;
   double position_timestamp;
   double prev_position_timestamp[5];
   double sample_magic;
   double timestamp;
   double old_timestamp;
   double Vx,Vy;
   int scroll_flag;
   unsigned int damping_magic;
   Etk_Bool mouse_down;
   Etk_Bool bar_pressed;
   Etk_Bool dragable;
   Etk_Bool bouncy;
};

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
   Etk_Widget *viewport;

   Etk_Scrolled_View_Policy hpolicy;
   Etk_Scrolled_View_Policy vpolicy;

   struct Etk_Scrolled_View_Mouse_Drag drag;
};



Etk_Type   *etk_scrolled_view_type_get(void);
Etk_Widget *etk_scrolled_view_new(void);

Etk_Range  *etk_scrolled_view_hscrollbar_get(Etk_Scrolled_View *scrolled_view);
Etk_Range  *etk_scrolled_view_vscrollbar_get(Etk_Scrolled_View *scrolled_view);
void        etk_scrolled_view_add_with_viewport(Etk_Scrolled_View *scrolled_view, Etk_Widget *child);
void        etk_scrolled_view_policy_set(Etk_Scrolled_View *scrolled_view, Etk_Scrolled_View_Policy hpolicy, Etk_Scrolled_View_Policy vpolicy);
void        etk_scrolled_view_policy_get(Etk_Scrolled_View *scrolled_view, Etk_Scrolled_View_Policy *hpolicy, Etk_Scrolled_View_Policy *vpolicy);
void        etk_scrolled_view_dragable_set(Etk_Scrolled_View *scrolled_view, Etk_Bool dragable);
Etk_Bool    etk_scrolled_view_dragable_get(Etk_Scrolled_View *scrolled_view);
void        etk_scrolled_view_drag_bouncy_set(Etk_Scrolled_View *scrolled_view, Etk_Bool bouncy);
Etk_Bool    etk_scrolled_view_drag_bouncy_get(Etk_Scrolled_View *scrolled_view);
double      etk_scrolled_view_drag_sample_interval_set(Etk_Scrolled_View *scrolled_view, double interval);
double      etk_scrolled_view_drag_sample_interval_get(Etk_Scrolled_View *scrolled_view);
unsigned int etk_scrolled_view_drag_damping_set(Etk_Scrolled_View *scrolled_view, unsigned int damping);
unsigned int etk_scrolled_view_drag_damping_get(Etk_Scrolled_View *scrolled_view);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
