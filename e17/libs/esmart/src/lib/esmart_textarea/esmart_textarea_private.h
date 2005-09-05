#include "Esmart_Textarea.h"
#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DATADIR PACKAGE_DATA_DIR"/"

/* textarea private core functions */
Esmart_Text_Area *_esmart_textarea_init(Evas_Object *o);
void              _esmart_textarea_focus_set(Esmart_Text_Area *t, Evas_Bool focus);
void              _esmart_textarea_bg_set(Esmart_Text_Area *t, Evas_Object *o);
Evas_Object      *_esmart_textarea_bg_get(Esmart_Text_Area *t);
void              _esmart_textarea_clear(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_pos_set(Esmart_Text_Area *t, int pos);
int               _esmart_textarea_cursor_pos_get(Esmart_Text_Area *t);
int               _esmart_textarea_length_get(Esmart_Text_Area *t);
int               _esmart_textarea_cursor_line_get(Esmart_Text_Area *t);
int               _esmart_textarea_lines_get(Esmart_Text_Area *t);
int               _esmart_textarea_line_start_pos_get(Esmart_Text_Area *t);
int               _esmart_textarea_line_end_pos_get(Esmart_Text_Area *t);
Evas_Bool         _esmart_textarea_line_get(Esmart_Text_Area *t, int line, Evas_Coord *lx, Evas_Coord *ly, Evas_Coord *lw, Evas_Coord *lh);
Evas_Bool         _esmart_textarea_char_pos_get(Esmart_Text_Area *t, int pos, Evas_Coord *lx, Evas_Coord *ly, Evas_Coord *lw, Evas_Coord *lh);
int               _esmart_textarea_char_coords_get(Esmart_Text_Area *t, Evas_Coord x, Evas_Coord y, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw,  Evas_Coord *ch);
void              _esmart_textarea_text_insert(Esmart_Text_Area *t, const char *text);
char             *_esmart_textarea_text_get(Esmart_Text_Area *t, int len);
void              _esmart_textarea_text_del(Esmart_Text_Area *t, int len);
void              _esmart_textarea_format_insert(Esmart_Text_Area *t, const char *format);
int               _esmart_textarea_format_next_pos_get(Esmart_Text_Area *t);
int               _esmart_textarea_format_next_count_get(Esmart_Text_Area *t);
const char       *_esmart_textarea_format_next_get(Esmart_Text_Area *t, int n);
void              _esmart_textarea_format_next_del(Esmart_Text_Area *t, int n);
int               _esmart_textarea_format_prev_pos_get(Esmart_Text_Area *t);
int               _esmart_textarea_format_prev_count_get(Esmart_Text_Area *t);
const char       *_esmart_textarea_format_prev_get(Esmart_Text_Area *t, int n);
void              _esmart_textarea_format_prev_del(Esmart_Text_Area *t, int n);
char             *_esmart_textarea_format_current_get(Esmart_Text_Area *t);
void              _esmart_textarea_format_size_get(Esmart_Text_Area *t, Evas_Coord *w, Evas_Coord *h);
void              _esmart_textarea_native_size_get(Esmart_Text_Area *t, Evas_Coord *w, Evas_Coord *h);
int               _esmart_textarea_native_lines_get(Esmart_Text_Area *t);

Esmart_Text_Area_Format *_esmart_textarea_format_get(Esmart_Text_Area *t);
    

/* textarea cursor movement and cursor related functions */
void              _esmart_textarea_cursor_goto_cursor(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_move_left(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_move_right(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_move_down(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_move_up(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_move_home(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_move_end(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_delete_right(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_delete_left(Esmart_Text_Area *t);
void              _esmart_textarea_cursor_set(Esmart_Text_Area *t, Evas_Object *c);

/* callback functions */
void              _esmart_textarea_cb_key_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
void              _esmart_textarea_cb_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
void              _esmart_textarea_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
void              _esmart_textarea_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
void              _esmart_textarea_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);

/* textarea smart functions */
Evas_Smart       *esmart_textarea_smart_get();
void              esmart_textarea_smart_add(Evas_Object *o);
void              esmart_textarea_smart_del(Evas_Object *o);
void              esmart_textarea_smart_layer_set(Evas_Object *o, int l);
void              esmart_textarea_smart_raise(Evas_Object *o);
void              esmart_textarea_smart_lower(Evas_Object *o);
void              esmart_textarea_smart_stack_above(Evas_Object *o, Evas_Object *above);
void              esmart_textarea_smart_stack_below(Evas_Object *o, Evas_Object *below);
void              esmart_textarea_smart_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
void              esmart_textarea_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
void              esmart_textarea_smart_show(Evas_Object *o);
void              esmart_textarea_smart_hide(Evas_Object *o);
void              esmart_textarea_smart_color_set(Evas_Object *o, int r, int g, int b, int a);
void              esmart_textarea_smart_clip_set(Evas_Object *o, Evas_Object *clip);
void              esmart_textarea_smart_clip_unset(Evas_Object *o);
