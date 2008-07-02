#ifndef _EDJE_EDITOR_PROGRAM_H_
#define _EDJE_EDITOR_PROGRAM_H_



/* program frame objects */
Etk_Widget *UI_ProgramEntry;
Etk_Widget *UI_ProgramEntryImage;
Etk_Widget *UI_RunProgButton;
Etk_Widget *UI_SignalEntry;
Etk_Widget *UI_ActionComboBox;
Etk_Widget *UI_TransiComboBox;
Etk_Widget *UI_TransiLabel;
Etk_Widget *UI_DurationSpinner;
Etk_Widget *UI_AfterEntry;
Etk_Widget *UI_SourceEntry;
Etk_Widget *UI_TargetEntry;
Etk_Widget *UI_TargetLabel;
Etk_Widget *UI_Param1Label;
Etk_Widget *UI_Param1Entry;
Etk_Widget *UI_Param1Spinner;
Etk_Widget *UI_Param1Label;
Etk_Widget *UI_Param2Entry;
Etk_Widget *UI_Param2Spinner;
Etk_Widget *UI_Param2Label;
Etk_Widget *UI_DelayFromSpinner;
Etk_Widget *UI_DelayRangeSpinner;

Etk_Widget* create_program_frame(void);
void UpdateProgFrame(void);


Etk_Bool on_ActionComboBox_changed         (Etk_Combobox *combobox, void *data);
Etk_Bool on_ProgramEntryImage_mouse_clicked(Etk_Object *object, void *data);
Etk_Bool on_ProgramEntry_key_down          (Etk_Object *object, Etk_Event_Key_Down *event, void *data);
Etk_Bool on_SourceEntry_text_changed       (Etk_Object *object, void *data);
Etk_Bool on_SourceEntry_item_changed       (Etk_Combobox_Entry *combo, void *data);
Etk_Bool on_SignalEntry_text_changed       (Etk_Object *object, void *data);
Etk_Bool on_SignalEntry_item_changed       (Etk_Combobox_Entry *combo, void *data);
Etk_Bool on_DelaySpinners_value_changed    (Etk_Range *range, double value, void *data);
Etk_Bool on_TargetEntry_text_changed       (Etk_Object *object, void *data);
Etk_Bool on_Param1Entry_text_changed       (Etk_Object *object, void *data);
Etk_Bool on_Param2Entry_text_changed       (Etk_Object *object, void *data);
Etk_Bool on_Param1Spinner_value_changed    (Etk_Range *range, double value, void *data);
Etk_Bool on_Param2Spinner_value_changed    (Etk_Range *range, double value, void *data);
Etk_Bool on_TransitionComboBox_changed     (Etk_Combobox *combobox, void *data);
Etk_Bool on_DurationSpinner_value_changed  (Etk_Range *range, double value, void *data);
Etk_Bool on_AfterEntry_text_changed        (Etk_Object *object, void *data);


#endif
