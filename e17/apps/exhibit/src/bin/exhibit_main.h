/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_MAIN_H
#define _EX_MAIN_H

void         _ex_main_statusbar_zoom_update(Exhibit *e);
void         _ex_main_populate_files(Exhibit *e, const char *selected_file);
int          _ex_main_window_slideshow_next(void *data);
void         _ex_main_window_slideshow_toggle(Exhibit *e);    
void         _ex_main_window_tab_append(Exhibit *e, Ex_Tab *tab);
void         _ex_main_window_tab_remove(Ex_Tab *tab);
void         _ex_main_dialog_show(char *text, Etk_Message_Dialog_Type type);
void         _ex_main_window_show(char *dir);
void         _ex_main_image_unset();
void         _ex_main_image_set(Exhibit *e, char *image);
void         _ex_main_button_fit_to_window_cb(Etk_Object *obj, void *data);
void         _ex_main_button_zoom_one_to_one_cb(Etk_Object *obj, void *data);
   
   

#endif
