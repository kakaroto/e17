#ifndef _EDJE_EDITOR_POSITION_H_
#define _EDJE_EDITOR_POSITION_H_


/* position frame objects */
Etk_Widget *UI_Rel1XSpinner;
Etk_Widget *UI_Rel1XOffsetSpinner;
Etk_Widget *UI_Rel1YSpinner;
Etk_Widget *UI_Rel1YOffsetSpinner;
Etk_Widget *UI_Rel2XSpinner;
Etk_Widget *UI_Rel2XOffsetSpinner;
Etk_Widget *UI_Rel2YSpinner;
Etk_Widget *UI_Rel2YOffsetSpinner;
Etk_Widget *UI_Rel1ToXComboBox;
Etk_Widget *UI_Rel1ToYComboBox;
Etk_Widget *UI_Rel2ToXComboBox;
Etk_Widget *UI_Rel2ToYComboBox;


Etk_Widget* position_frame_create(void);
void        position_frame_update(void);
void        position_comboboxes_populate(void);
void        position_comboboxes_update(void);


Etk_Bool _position_RelOffsetSpinners_value_changed_cb (Etk_Range *range, double value, void *data);
Etk_Bool _position_RelSpinners_value_changed_cb       (Etk_Range *range, double value, void *data);
Etk_Bool _position_RelToComboBoxes_changed_cb         (Etk_Combobox *combobox, void *data);


#endif
