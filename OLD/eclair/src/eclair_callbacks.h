#ifndef _ECLAIR_CALLBACKS_H_
#define _ECLAIR_CALLBACKS_H_

#include <Evas.h>
#include <Edje.h>

int eclair_exit_cb(void *data, int type, void *event);
void eclair_key_press_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
void eclair_message_cb(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg);
int eclair_mouse_up_cb(void *data, int type, void *event);

int eclair_dnd_position_cb(void *data, int type, void *event);
int eclair_dnd_drop_cb(void *data, int type, void *event);
int eclair_dnd_selection_cb(void *data, int type, void *event);

void eclair_open_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_play_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_pause_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_stop_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_prev_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_next_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_repeat_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_shuffle_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_playlist_load_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_playlist_save_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_remove_all_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_remove_selected_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_remove_unselected_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_select_all_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_select_none_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_select_inverse_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_progress_bar_drag_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_volume_bar_drag_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);

#endif
