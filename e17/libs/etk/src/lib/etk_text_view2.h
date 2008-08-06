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

/** @file etk_text_view2.h */
#ifndef _ETK_TEXT_VIEW2_H_
#define _ETK_TEXT_VIEW2_H_

#include <Evas.h>

#include "etk_widget.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Text_View2 Etk_Text_View2
 * @brief TODOC: brief description
 * @{
 */

/** @brief Gets the type of a text view */
#define ETK_TEXT_VIEW2_TYPE       (etk_text_view2_type_get())
/** @brief Casts the object to an Etk_Text_View2 */
#define ETK_TEXT_VIEW2(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXT_VIEW2_TYPE, Etk_Text_View2))
/** @brief Checks if the object is an Etk_Text_View2 */
#define ETK_IS_TEXT_VIEW2(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXT_VIEW2_TYPE))

/**
 * TODOC
 */
struct Etk_Text_View2
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Textblock2 *textblock;
   Evas_Object *tbo;
};

Etk_Type            *etk_text_view2_type_get(void);
Etk_Widget          *etk_text_view2_new(void);

Etk_Textblock2      *etk_text_view2_textblock_get(Etk_Text_View2 *text_view);
Etk_Textblock2_Iter *etk_text_view2_cursor_get(Etk_Text_View2 *text_view);
Etk_Textblock2_Iter *etk_text_view2_selection_bound_get(Etk_Text_View2 *text_view);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
