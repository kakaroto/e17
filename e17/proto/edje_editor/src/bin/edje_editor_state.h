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


Etk_Widget* state_frame_create (void);
void        state_frame_update (void);


Etk_Bool _state_Entry_key_down_cb              (Etk_Object *object, Etk_Event_Key_Down *event, void *data);
Etk_Bool _state_EntryImage_clicked_cb          (Etk_Object *object, void *data);
Etk_Bool _state_AspectSpinner_value_changed_cb (Etk_Range *range, double value, void *data);
Etk_Bool _state_AspectComboBox_changed_cb      (Etk_Combobox *combobox, void *data);
Etk_Bool _state_MinMaxSpinner_value_changed_cb (Etk_Range *range, double value, void *data);


#endif
