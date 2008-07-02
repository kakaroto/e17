#ifndef _EDJE_EDITOR_STATE_H_
#define _EDJE_EDITOR_STATE_H_


/* state frame objects */
Etk_Widget *UI_StateEntry;
Etk_Widget *UI_StateEntryImage;
Etk_Widget *UI_AspectMinSpinner;
Etk_Widget *UI_AspectMaxSpinner;
Etk_Widget *UI_AspectComboBox;
Etk_Widget *UI_StateMinWSpinner;
Etk_Widget *UI_StateMinHSpinner;
Etk_Widget *UI_StateMaxWSpinner;
Etk_Widget *UI_StateMaxHSpinner;
Etk_Widget *UI_StateAlignVSpinner;
Etk_Widget *UI_StateAlignHSpinner;


Etk_Widget* create_description_frame(void);
void UpdateDescriptionFrame(void);


Etk_Bool on_StateEntry_key_down            (Etk_Object *object, Etk_Event_Key_Down *event, void *data);
Etk_Bool on_StateEntryImage_mouse_clicked  (Etk_Object *object, void *data);
Etk_Bool on_AspectSpinner_value_changed    (Etk_Range *range, double value, void *data);
Etk_Bool on_AspectComboBox_changed         (Etk_Combobox *combobox, void *data);
Etk_Bool on_StateMinMaxSpinner_value_changed(Etk_Range *range, double value, void *data);


#endif
