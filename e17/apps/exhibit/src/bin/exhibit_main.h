#ifndef _EX_MAIN_H
#define _EX_MAIN_H

void         _ex_main_statusbar_zoom_update(Exhibit *e);
void         _ex_main_button_zoom_in_cb(Etk_Object *obj, void *data);
void         _ex_main_button_zoom_out_cb(Etk_Object *obj, void *data);
void         _ex_main_button_zoom_one_to_one_cb(Etk_Object *obj, void *data);
void         _ex_main_button_fit_to_window_cb(Etk_Object *obj, void *data);
void         _ex_main_itree_item_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
void         _ex_main_itree_key_down_cb(Etk_Object *object, void *event, void *data);
void         _ex_main_dtree_item_clicked_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
void         _ex_main_populate_files(Exhibit *e);
void         _ex_main_entry_dir_key_down_cb(Etk_Object *object, void *event, void *data);
void         _ex_main_goto_dir_clicked_cb(Etk_Object *object, void *data);
Etk_Bool     _ex_main_window_deleted_cb(void *data);
void         _ex_main_window_show(char *dir);

#endif
