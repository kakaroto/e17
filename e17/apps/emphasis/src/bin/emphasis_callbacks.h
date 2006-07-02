/** @file emphasis_callbacks.h */
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

/**
 * @defgroup callbacks
 * @{
 */
void cb_quit(Etk_Object *object, void *data);

void cb_button_stop_clicked(Etk_Object *object, void *data);
void cb_button_prev_clicked(Etk_Object *object, void *data);
void cb_button_play_clicked(Etk_Object *object, void *data);
void cb_button_next_clicked(Etk_Object *object, void *data);
void cb_toggle_random(Etk_Object *object, void *data);
void cb_toggle_repeat(Etk_Object *object, void *data);
void cb_seek_time(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);

void cb_tree_artist_selected(Etk_Object *object, Etk_Tree_Row *row, void *data);
void cb_tree_album_selected(Etk_Object *object, Etk_Tree_Row *row, void *data);

void cb_drag_artist(Etk_Object *object, void *data);
void cb_drag_album(Etk_Object *object, void *data);
void cb_drag_track(Etk_Object *object, void *data);
void cb_drop_song(Etk_Object *object, void *event, void *data);

void cb_tree_mlib_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data);
void cb_tree_pls_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data);

void cb_emphasis_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data);
void cb_mlib_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data);
void cb_pls_contextual_menu(Etk_Object *object, Etk_Event_Mouse_Up_Down *event_info, void *data);
void cb_playlist_clear(Etk_Object *object,  Etk_Event_Mouse_Up_Down *event, void *data);
void cb_pls_bindings_key(Etk_Object *object, Etk_Event_Key_Up_Down *event, void *data);
void cb_playlist_delete(Etk_Object *object, void *data);

void cb_vol_image_clicked(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);
void cb_vol_slider_value_changed(Etk_Object *object, double value, void *data);
void cb_database_update(Etk_Object *object, Etk_Event_Mouse_Up_Down *event, void *data);

void cb_config_show(Etk_Object *object, void *data);
void cb_config_hide(Etk_Object *object, void *data);
void cb_config_write(Etk_Object *object, void *data);

void cb_switch_small(Etk_Object *object, void *data);
void cb_switch_full(Etk_Object *object, void *data);
/** @} */
#endif /*_CALLBACKS_H_*/
