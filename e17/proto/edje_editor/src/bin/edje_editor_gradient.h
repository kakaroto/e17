#ifndef _EDJE_EDITOR_OBJ_GRADIENT_H_
#define _EDJE_EDITOR_OBJ_GRADIENT_H_



Etk_Widget *UI_GradientTypeComboBox;
Etk_Widget *UI_GradientSpectraComboBox;
Etk_Widget *UI_GradientRel1XSpinner;
Etk_Widget *UI_GradientRel1XOffsetSpinner;
Etk_Widget *UI_GradientRel1YSpinner;
Etk_Widget *UI_GradientRel1YOffsetSpinner;
Etk_Widget *UI_GradientRel2XSpinner;
Etk_Widget *UI_GradientRel2XOffsetSpinner;
Etk_Widget *UI_GradientRel2YSpinner;
Etk_Widget *UI_GradientRel2YOffsetSpinner;


Etk_Widget* gradient_frame_create(void);
void        gradient_frame_update(void);
void        gradient_spectra_combo_populate(void);

Etk_Bool _gradient_type_combo_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool _gradient_spectra_combo_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);

#endif
