#ifndef _EDJE_EDITOR_GROUP_H_
#define _EDJE_EDITOR_GROUP_H_


/* group frame objects */
Etk_Widget *UI_GroupNameEntry;
Etk_Widget *UI_GroupNameEntryImage;
Etk_Widget *UI_GroupMinWSpinner;
Etk_Widget *UI_GroupMinHSpinner;
Etk_Widget *UI_GroupMaxWSpinner;
Etk_Widget *UI_GroupMaxHSpinner;
Etk_Widget *UI_CurrentGroupSizeLabel;


Etk_Widget* create_group_frame(void);
void UpdateGroupFrame(void);
Etk_Bool on_NamesEntry_text_changed        (Etk_Object *object, void *data);
Etk_Bool on_GroupSpinner_value_changed     (Etk_Range *range, double value, void *data);
Etk_Bool on_GroupNameEntry_key_down        (Etk_Object *object, Etk_Event_Key_Down *event, void *data);
Etk_Bool on_GroupNameEntryImage_mouse_clicked(Etk_Object *object, void *data);


#endif
