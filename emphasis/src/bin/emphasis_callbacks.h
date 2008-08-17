/** @file emphasis_callbacks.h */
#ifndef EMPHASIS_CALLBACKS_H_
#define EMPHASIS_CALLBACKS_H_

/**
 * @defgroup callbacks
 * @{
 */
Etk_Bool cb_quit(Etk_Object *object, void *data);
Etk_Bool cb_media_quit(Etk_Object *object, void *data);
Etk_Bool cb_window_hide(Etk_Object *object, void *data);
Etk_Bool cb_pack_quit(Etk_Object *object, void *data);

Etk_Bool cb_button_stop_clicked(Etk_Object *object, void *data);
Etk_Bool cb_button_prev_clicked(Etk_Object *object, void *data);
Etk_Bool cb_button_play_clicked(Etk_Object *object, void *data);
Etk_Bool cb_button_next_clicked(Etk_Object *object, void *data);
Etk_Bool cb_toggle_random(Etk_Object *object, void *data);
Etk_Bool cb_toggle_repeat(Etk_Object *object, void *data);
Etk_Bool cb_toggle_full(Etk_Object *object, void *data);
Etk_Bool cb_toggle_media(Etk_Object *object, void *data);
Etk_Bool cb_seek_time(Etk_Object *object, Etk_Event_Mouse_Down *event,
                  void *data);
Etk_Bool cb_vol_image_clicked(Etk_Object *object, Etk_Event_Mouse_Down *event, 
                          void *data);
Etk_Bool cb_vol_slider_value_changed(Etk_Object *object, double value,
                                 void *data);

Etk_Bool cb_tree_artist_selected(Etk_Object *object, Etk_Tree_Row *row, void *data);
Etk_Bool cb_tree_album_selected(Etk_Object *object, Etk_Tree_Row *row, void *data);

Etk_Bool cb_drag_artist(Etk_Object *object, void *data);
Etk_Bool cb_drag_album(Etk_Object *object, void *data);
Etk_Bool cb_drag_track(Etk_Object *object, void *data);
Etk_Bool cb_drop_song(Etk_Object *object, void *event, void *data);

Etk_Bool cb_tree_mlib_clicked(Etk_Object *object, Etk_Tree_Row *row,
                          Etk_Event_Mouse_Down *event, void *data);
Etk_Bool cb_tree_pls_clicked(Etk_Object *object, Etk_Tree_Row *row,
                         Etk_Event_Mouse_Down *event, void *data);

Etk_Bool cb_emphasis_bindings_key(Etk_Object *object, Etk_Event_Key_Down *event, 
                              void *data);
Etk_Bool cb_mlib_bindings_key(Etk_Object *object, Etk_Event_Key_Down *event,
                          void *data);


Etk_Bool cb_pls_contextual_menu(Etk_Object *object, Etk_Event_Mouse_Down *event, 
                            void *data);
Etk_Bool cb_playlist_clear(Etk_Object *object, Etk_Event_Mouse_Down *event,
                       void *data);
Etk_Bool cb_pls_bindings_key(Etk_Object *object, Etk_Event_Key_Down *event,
                         void *data);
Etk_Bool cb_playlist_delete(Etk_Object *object, void *data);
Etk_Bool cb_playlist_shuffle(Etk_Object *object, void *data);
Etk_Bool cb_database_update(Etk_Object *object, void *data);

Etk_Bool cb_small_resize(Etk_Object *object, void *data);

Etk_Bool cb_switch_small(Etk_Object *object, void *data);
Etk_Bool cb_switch_full(Etk_Object *object, void *data);

Etk_Bool cb_media_button_lib_clicked(Etk_Object *object, void *data); 
Etk_Bool cb_media_button_search_clicked(Etk_Object *object, void *data); 
Etk_Bool cb_media_button_playlists_clicked(Etk_Object *object, void *data); 
Etk_Bool cb_media_button_stats_clicked(Etk_Object *object, void *data);
Etk_Bool cb_config_show(Etk_Object *object, void *data);
Etk_Bool cb_config_ok(Etk_Object *object, void *data);
Etk_Bool cb_config_hide(Etk_Object *object, void *data);

Etk_Bool cb_media_pls_list_row_clicked(Etk_Object *object, Etk_Tree_Row *row, 
                                   Etk_Event_Mouse_Down *event, void *data);
Etk_Bool cb_media_pls_save_clicked(Etk_Object *object, void *data);
Etk_Bool cb_media_pls_load_clicked(Etk_Object *object, void *data);
Etk_Bool cb_media_pls_del_clicked(Etk_Object *object, void *data);
Etk_Bool cb_media_pls_save_key_down(Etk_Object *object, 
                                Etk_Event_Key_Down *event, void *data);


Etk_Bool cb_media_search_btn_add_clicked(Etk_Object *object, void *data);
Etk_Bool cb_media_search_btn_replace_clicked(Etk_Object *object, void *data);
Etk_Bool cb_media_search_btn_del_search_clicked(Etk_Object *object, void *data);

Etk_Bool cb_media_search_btn_remove_search_clicked(Etk_Object *object, void *data);
Etk_Bool cb_media_search_entry_text_changed(Etk_Object *object,
                                        Etk_Event_Key_Down *event,
                                        void *data);
Etk_Bool cb_media_search_btn_search_clicked(Etk_Object *object, void *data);
Etk_Bool cb_tiny_cover_wheel(Etk_Object *object, Etk_Event_Mouse_Wheel *event, void *data);
/** @} */
#endif /* EMPHASIS_CALLBACKS_H_ */
