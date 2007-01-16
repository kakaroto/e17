/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_MAIN_H
#define _EX_MAIN_H

void         _ex_main_statusbar_zoom_update(Exhibit *e);
void         _ex_main_populate_files(const char *selected_file, Ex_Tree_Update update);
void         _ex_main_itree_add(const char *file, const char *selected_file);
int          _ex_main_window_slideshow_next(void *data);
void         _ex_main_window_slideshow_toggle();    
void         _ex_main_window_fullscreen_toggle();    
void         _ex_main_window_tab_append(Ex_Tab *tab);
void         _ex_main_window_tab_remove(Ex_Tab *tab);
void         _ex_main_dialog_show(char *text, Etk_Message_Dialog_Type type);
void         _ex_main_window_show(char *dir, int fullscreen);
void         _ex_main_image_unset();
void         _ex_main_image_set(Exhibit *e, char *image);
void         _ex_main_button_fit_to_window_cb(Etk_Object *obj, void *data);
void         _ex_main_button_zoom_one_to_one_cb(Etk_Object *obj, void *data);

/* remove this when tree2 defines it */
Etk_Tree2_Row *etk_tree2_selected_row_get(Etk_Tree2 *tree);
Evas_List *etk_tree2_selected_rows_get(Etk_Tree2 *tree);
int etk_tree2_num_rows_get(Etk_Tree2 *tree);
int etk_tree2_row_num_get(Etk_Tree2 *tree, Etk_Tree2_Row *row);
#endif
