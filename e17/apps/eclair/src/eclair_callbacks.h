#ifndef _ECLAIR_CALLBACKS_H_
#define _ECLAIR_CALLBACKS_H_

#include "eclair_private.h"

int eclair_exit_cb(void *data, int type, void *event);
void eclair_key_press_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
void eclair_message_cb(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg);
int eclair_mouse_up_cb(void *data, int type, void *event);

int eclair_dnd_position_cb(void *data, int type, void *event);
int eclair_dnd_drop_cb(void *data, int type, void *event);
int eclair_dnd_selection_cb(void *data, int type, void *event);

void eclair_video_window_resize_cb(Ecore_Evas *window);
void eclair_video_window_close_cb(Ecore_Evas *window);
void eclair_video_frame_decode_cb(void *data, Evas_Object *obj, void *event_info);
void eclair_video_playback_finished_cb(void *data, Evas_Object *obj, void *event_info);
void eclair_video_audio_level_change_cb(void *data, Evas_Object *obj, void *event_info);
void eclair_video_frame_resize_change_cb(void *data, Evas_Object *obj, void *event_info);

void eclair_open_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_play_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_pause_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_stop_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_prev_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_next_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
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

void eclair_playlist_container_entry_down_cb(void *data, Evas *evas, Evas_Object *entry, void *event_info);
void eclair_playlist_container_scrollbar_drag_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_playlist_container_wheel_cb(void *data, Evas *evas, Evas_Object *playlist_container, void *event_info);
void eclair_playlist_container_scroll_percent_changed(void *data, Evas_Object *obj, void *event_info);
void eclair_playlist_container_scroll_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);

void eclair_window_resize_start_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_window_resize_stop_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_window_mouse_move_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
void eclair_window_resize_cb(Ecore_Evas *ecore_window);
void eclair_window_open_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_window_close_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);
void eclair_window_close_request_cb(Ecore_Evas *ecore_window);
void eclair_window_minimize_cb(void *data, Evas_Object *edje_object, const char *emission, const char *source);

#endif
