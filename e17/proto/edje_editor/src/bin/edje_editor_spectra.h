#ifndef _EDJE_EDITOR_SPECTRA_H_
#define _EDJE_EDITOR_SPECTRA_H_


Etk_Widget *UI_SpectrumWin;
Etk_Widget *UI_SpectrumList;
Etk_Widget *UI_SpectraWidget;


Etk_Widget* spectra_window_create   (void);
void        spectra_window_show     (void);
void        spectra_window_populate (void);
Etk_Widget* spectra_thumb_create    (Etk_Widget *parent, const char *spectra);

void //TODO this should be done as a new Etk_Event with a callback
spectra_window_gradient_changed(Etk_Widget *spectra);
void //TODO this should be done as a new Etk_Event with a callback
spectra_window_gradient_name_changed(Etk_Widget *spectra);

Etk_Bool _spectra_window_hide_cb(Etk_Object *window,void *data);
Etk_Bool _spectra_list_row_selected_cb(Etk_Tree *tree, Etk_Tree_Row *row, void *data);
Etk_Bool _spectra_add_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _spectra_del_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _spectra_thumb_destroy(Etk_Object *object, void *data);


#endif
