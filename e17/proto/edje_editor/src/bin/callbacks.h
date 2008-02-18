#ifndef _EDJE_EDITOR_CALLBACKS_H_
#define _EDJE_EDITOR_CALLBACKS_H_

/* Called when the window is destroyed */
Etk_Bool etk_main_quit_cb(void *data);
void ecore_delete_cb(Ecore_Evas *ee);
void ecore_resize_callback(Ecore_Evas *ecore_evas);

//void on_GroupComboBox_changed        (Etk_Combobox *combobox, void *data);
Etk_Bool on_GroupSpinner_value_changed     (Etk_Range *range, double value, void *data);
Etk_Bool on_canvas_geometry_changed        (Etk_Object *canvas, const char *property_name, void *data);
Etk_Bool on_PartsTree_row_selected         (Etk_Object *object, Etk_Tree_Row *row, void *data);
void     on_ColorCanvas_click              (void *data, Evas *e, Evas_Object *obj, void *event_info);
Etk_Bool on_ColorDialog_change             (Etk_Object *object, void *data);
Etk_Bool on_PartNameEntry_text_changed     (Etk_Object *object, void *data);
Etk_Bool on_PartEventsCheck_toggled        (Etk_Object *object, void *data);
Etk_Bool on_PartEventsRepeatCheck_toggled  (Etk_Object *object, void *data);
Etk_Bool on_StateEntry_text_changed        (Etk_Object *object, void *data);
Etk_Bool on_AllButton_click                (Etk_Button *button, void *data);
Etk_Bool on_AddMenu_item_activated         (Etk_Object *object, void *data);
Etk_Bool on_RemoveMenu_item_activated      (Etk_Object *object, void *data);
Etk_Bool on_RelToComboBox_changed          (Etk_Combobox *combobox, void *data);
Etk_Bool on_TextEntry_text_changed         (Etk_Object *object, void *data);
Etk_Bool on_FontAlignSpinner_value_changed (Etk_Range *range, double value, void *data);
Etk_Bool on_FileChooser_response           (Etk_Dialog *dialog, int response_id,void *data);
Etk_Bool on_FileChooser_selected           (Etk_Filechooser_Widget *filechooser);
Etk_Bool on_FileChooserDialog_response     (Etk_Dialog *dialog, int response_id, void *data);
Etk_Bool on_BorderSpinner_value_changed    (Etk_Range *range, double value, void *data);
Etk_Bool on_RelSpinner_value_changed       (Etk_Range *range, double value, void *data);
Etk_Bool on_RelOffsetSpinner_value_changed (Etk_Range *range, double value, void *data);
Etk_Bool on_FontComboBox_item_activated    (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool on_FontSizeSpinner_value_changed  (Etk_Range *range, double value, void *data);

Etk_Bool on_ColorCanvas_realize            (Etk_Widget *canvas, void *data);
Etk_Bool on_ImageAlphaSlider_value_changed (Etk_Object *object, double value, void *data);
Etk_Bool on_ImageComboBox_item_activated   (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool on_ImageTweenList_row_selected    (Etk_Object *object, Etk_Tree_Row *row, void *data);
Etk_Bool on_EffectComboBox_changed         (Etk_Combobox *combobox, void *data);
Etk_Bool on_AlertDialog_response           (Etk_Dialog *dialog, int response_id, void *data);
Etk_Bool on_GroupNameEntry_text_changed    (Etk_Object *object, void *data);
Etk_Bool on_AspectSpinner_value_changed    (Etk_Range *range, double value, void *data);
Etk_Bool on_AspectComboBox_changed         (Etk_Combobox *combobox, void *data);
Etk_Bool on_StateMinMaxSpinner_value_changed(Etk_Range *range, double value, void *data);
Etk_Bool on_ActionComboBox_changed         (Etk_Combobox *combobox, void *data);
Etk_Bool on_ProgramEntry_text_changed      (Etk_Object *object, void *data);
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


Etk_Bool on_GroupsComboBox_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
void signal_cb(void *data, Evas_Object *o, const char *sig, const char *src);

#endif
