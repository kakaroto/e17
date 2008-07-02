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

Etk_Widget* create_position_frame(void);
void PopulateRelComboBoxes(void);
void UpdatePositionFrame(void);
void UpdateComboPositionFrame(void);


Etk_Bool on_RelOffsetSpinner_value_changed (Etk_Range *range, double value, void *data);
Etk_Bool on_RelSpinner_value_changed       (Etk_Range *range, double value, void *data);
Etk_Bool on_RelToComboBox_changed          (Etk_Combobox *combobox, void *data);


#endif
