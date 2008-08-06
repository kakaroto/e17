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

/** @file etk_text_view.h */
#ifndef _ETK_TEXT_VIEW_H_
#define _ETK_TEXT_VIEW_H_

#include <Evas.h>

#include "etk_widget.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Text_View Etk_Text_View
 * @{
 */

/** @brief Gets the type of a text view */
#define ETK_TEXT_VIEW_TYPE       (etk_text_view_type_get())
/** @brief Casts the object to an Etk_Text_View */
#define ETK_TEXT_VIEW(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXT_VIEW_TYPE, Etk_Text_View))
/** @brief Checks if the object is an Etk_Text_View */
#define ETK_IS_TEXT_VIEW(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXT_VIEW_TYPE))

extern int ETK_TEXT_VIEW_TEXT_CHANGED_SIGNAL;

/**
 * @struct Etk_Text_View
 * @brief An Etk_Text_View is a widget that allows the user to edit multi-line text
 */
struct Etk_Text_View
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Textblock *textblock;
   Evas_Object *textblock_object;
};

Etk_Type *etk_text_view_type_get(void);
Etk_Widget *etk_text_view_new(void);

Etk_Textblock *etk_text_view_textblock_get(Etk_Text_View *text_view);
Etk_Textblock_Iter *etk_text_view_cursor_get(Etk_Text_View *text_view);
Etk_Textblock_Iter *etk_text_view_selection_bound_get(Etk_Text_View *text_view);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
