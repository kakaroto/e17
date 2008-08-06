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

/** @file etk_editable.h */
#ifndef _ETK_EDITABLE_H_
#define _ETK_EDITABLE_H_

#include <Evas.h>

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Editable Etk_Editable
 * @brief An editable object is an evas smart-object that displays a single-line text and
 * allows the user to select and edit it. It is used by widgets such as the entry, the spinner, ...
 * @{
 */

Evas_Object *etk_editable_add(Evas *evas);
void         etk_editable_theme_set(Evas_Object *editable, const char *file, const char *group);
void         etk_editable_align_set(Evas_Object *editable, float align);
float        etk_editable_align_get(Evas_Object *editable);
void         etk_editable_password_mode_set(Evas_Object *editable, Etk_Bool password_mode);
Etk_Bool     etk_editable_password_mode_get(Evas_Object *editable);
void         etk_editable_disabled_set(Evas_Object *editable, Etk_Bool disabled);
Etk_Bool     etk_editable_disabled_get(Evas_Object *editable);

void         etk_editable_text_set(Evas_Object *editable, const char *text);
const char  *etk_editable_text_get(Evas_Object *editable);
char        *etk_editable_text_range_get(Evas_Object *editable, int start, int end);
int          etk_editable_text_length_get(Evas_Object *editable);
Etk_Bool     etk_editable_insert(Evas_Object *editable, int pos, const char *text);
Etk_Bool     etk_editable_delete(Evas_Object *editable, int start, int end);

void         etk_editable_cursor_pos_set(Evas_Object *editable, int pos);
int          etk_editable_cursor_pos_get(Evas_Object *editable);
void         etk_editable_cursor_move_to_start(Evas_Object *editable);
void         etk_editable_cursor_move_to_end(Evas_Object *editable);
void         etk_editable_cursor_move_left(Evas_Object *editable);
void         etk_editable_cursor_move_right(Evas_Object *editable);
void         etk_editable_cursor_show(Evas_Object *editable);
void         etk_editable_cursor_hide(Evas_Object *editable);

void         etk_editable_selection_pos_set(Evas_Object *editable, int pos);
int          etk_editable_selection_pos_get(Evas_Object *editable);
void         etk_editable_selection_move_to_start(Evas_Object *editable);
void         etk_editable_selection_move_to_end(Evas_Object *editable);
void         etk_editable_selection_move_left(Evas_Object *editable);
void         etk_editable_selection_move_right(Evas_Object *editable);
void         etk_editable_select_all(Evas_Object *editable);
void         etk_editable_unselect_all(Evas_Object *editable);
void         etk_editable_selection_show(Evas_Object *editable);
void         etk_editable_selection_hide(Evas_Object *editable);

int          etk_editable_pos_get_from_coords(Evas_Object *editable, int x, int y);
void         etk_editable_char_size_get(Evas_Object *editable, int *w, int *h);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
