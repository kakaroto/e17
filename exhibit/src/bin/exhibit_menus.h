/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_MENUS_H
#define _EX_MENUS_H

typedef enum _Ex_Menu_Item_Type
{
   EX_MENU_ITEM_NORMAL,
   EX_MENU_ITEM_CHECK,     
   EX_MENU_ITEM_SEPARATOR,
   EX_MENU_ITEM_RADIO
} Ex_Menu_Item_Type;

Etk_Widget *_ex_menu_item_new(Ex_Menu_Item_Type item_type, const char *label, int stock_id, Etk_Menu_Shell *menu_shell, Etk_Callback callback, void *data);

void       _ex_menu_build_run_menu(Etk_Widget *submenu);
int        _ex_menu_new_tab_cb(Etk_Object *obj, void *data);
int        _ex_menu_delete_tab_cb(Etk_Object *obj, void *data);
int        _ex_menu_save_image_cb(Etk_Object *obj, void *data);
int        _ex_menu_save_image_as_cb(Etk_Object *obj, void *data);
int        _ex_menu_rename_cb(Etk_Object *obj, void *data);
int        _ex_menu_delete_cb(Etk_Object *obj, void *data);
int        _ex_menu_move_cb(Etk_Object *obj, void *data);
int        _ex_menu_options_cb(Etk_Object *obj, void *data);
int        _ex_menu_quit_cb(Etk_Object *obj, void *data);
int        _ex_menu_run_in_cb(Etk_Object *obj, void *data);
int        _ex_menu_undo_cb(Etk_Object *obj, void *data);
int        _ex_menu_rot_clockwise_cb(Etk_Object *obj, void *data);
int        _ex_menu_rot_counter_clockwise_cb(Etk_Object *obj, void *data);
int        _ex_menu_flip_horizontal_cb(Etk_Object *obj, void *data);
int        _ex_menu_flip_vertical_cb(Etk_Object *obj, void *data);
int        _ex_menu_blur_cb(Etk_Object *obj, void *data);
int        _ex_menu_sharpen_cb(Etk_Object *obj, void *data);
int        _ex_menu_brighten_cb(Etk_Object *obj, void *data);
int        _ex_menu_darken_cb(Etk_Object *obj, void *data);    
int        _ex_menu_set_wallpaper_cb(Etk_Object *obj, void *data);
int        _ex_menu_zoom_in_cb(Etk_Object *obj, void *data);
int        _ex_menu_zoom_out_cb(Etk_Object *obj, void *data);
int        _ex_menu_zoom_one_to_one_cb(Etk_Object *obj, void *data);
int        _ex_menu_fit_to_window_cb(Etk_Object *obj, void *data);
int        _ex_menu_window_fullscreen_toggle_cb(Etk_Object *obj, void *data);
int        _ex_menu_toggle_slideshow_cb(Etk_Object *obj, void *data);
int        _ex_menu_refresh_cb(Etk_Object *obj, void *data);
int        _ex_menu_comments_cb(Etk_Object *obj, void *data);
int        _ex_menu_add_to_fav_cb(Etk_Object *obj, void *data);
int        _ex_menu_remove_from_fav_cb(Etk_Object *obj, void *data);    
int        _ex_menu_go_to_fav_cb(Etk_Object *obj, void *data);    
int        _ex_menu_about_cb(Etk_Object *obj, void *data);

#endif
