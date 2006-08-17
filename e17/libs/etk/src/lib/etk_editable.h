/** @file etk_editable.h */
#ifndef _ETK_EDITABLE_H_
#define _ETK_EDITABLE_H_

#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Editable Etk_Editable
 * @brief An editable object is an evas smart object that display a single-line text
 * and allows the user to select and edit it. It is use by widgets such as entry, spinner, ...
 * @{
 */

Evas_Object *etk_editable_add(Evas *evas);

void     etk_editable_password_mode_set(Evas_Object *editable, Etk_Bool password_mode);
Etk_Bool etk_editable_password_mode_get(Evas_Object *editable);

void         etk_editable_text_set(Evas_Object *editable, const char *text);
const char  *etk_editable_text_get(Evas_Object *editable);
char        *etk_editable_text_range_get(Evas_Object *editable, int start, int end);
int          etk_editable_text_length_get(Evas_Object *editable);
Etk_Bool     etk_editable_insert(Evas_Object *editable, int pos, const char *text);
Etk_Bool     etk_editable_delete(Evas_Object *editable, int start, int end);

void etk_editable_cursor_pos_set(Evas_Object *editable, int pos);
int  etk_editable_cursor_pos_get(Evas_Object *editable);
void etk_editable_cursor_move_to_start(Evas_Object *editable);
void etk_editable_cursor_move_to_end(Evas_Object *editable);
void etk_editable_cursor_move_left(Evas_Object *editable);
void etk_editable_cursor_move_right(Evas_Object *editable);
void etk_editable_cursor_show(Evas_Object *editable);
void etk_editable_cursor_hide(Evas_Object *editable);

void etk_editable_selection_pos_set(Evas_Object *editable, int pos);
int  etk_editable_selection_pos_get(Evas_Object *editable);
void etk_editable_selection_move_to_start(Evas_Object *editable);
void etk_editable_selection_move_to_end(Evas_Object *editable);
void etk_editable_selection_move_left(Evas_Object *editable);
void etk_editable_selection_move_right(Evas_Object *editable);
void etk_editable_selection_show(Evas_Object *editable);
void etk_editable_selection_hide(Evas_Object *editable);
void etk_editable_select_all(Evas_Object *editable);
void etk_editable_unselect_all(Evas_Object *editable);

int  etk_editable_pos_get_from_coords(Evas_Object *editable, int x, int y);
void etk_editable_char_size_get(Evas_Object *editable, int *w, int *h);

/** @} */

#endif
