/** @file etk_editable_text_object.h */
#ifndef _ETK_EDITABLE_TEXT_OBJECT_H_
#define _ETK_EDITABLE_TEXT_OBJECT_H_

#include <Evas.h>

Evas_Object *etk_editable_text_object_add(Evas *evas);

void etk_editable_text_object_text_set(Evas_Object *object, const char *text);
void etk_editable_text_object_insert(Evas_Object *object, const char *text);
const char *etk_editable_text_object_text_get(Evas_Object *object);

void etk_editable_text_object_delete_char_before(Evas_Object *object);
void etk_editable_text_object_delete_char_after(Evas_Object *object);

void etk_editable_text_object_cursor_move_at_start(Evas_Object *object);
void etk_editable_text_object_cursor_move_at_end(Evas_Object *object);
void etk_editable_text_object_cursor_move_left(Evas_Object *object);
void etk_editable_text_object_cursor_move_right(Evas_Object *object);

void etk_editable_text_object_cursor_show(Evas_Object *object);
void etk_editable_text_object_cursor_hide(Evas_Object *object);

#endif
