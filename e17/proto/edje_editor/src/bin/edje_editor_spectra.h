#ifndef _EDJE_EDITOR_SPECTRA_H_
#define _EDJE_EDITOR_SPECTRA_H_


Etk_Widget *UI_SpectrumWin;
Etk_Widget *UI_SpectrumList;
Etk_Widget* UI_SpectraWidget;


Etk_Widget* spectra_window_create   (void);
void        spectra_window_show     (void);
void        spectra_window_populate (void);


Etk_Bool _spectra_list_row_selected_cb(Etk_Tree *tree, Etk_Tree_Row *row, void *data);
Etk_Bool _spectra_add_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _spectra_del_button_click_cb(Etk_Button *button, void *data);


#endif
