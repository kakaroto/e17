#ifndef _EDJE_EDITOR_SCRIPT_H_
#define _EDJE_EDITOR_SCRIPT_H_


/* script frame objects */
Etk_Widget *UI_ScriptBox;
Etk_Widget *UI_ScriptSaveButton;
Etk_Widget *UI_MsgSendTypeComboBox;
Etk_Widget *UI_IntFloatSpinner;
Etk_Widget *UI_MsgSendEntry;

Etk_Widget* script_frame_create (void);
void        script_frame_update (void);


Etk_Bool _script_MsgTypeComboBox_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool _script_MsgSend_pressed_cb(Etk_Button *button, void *data);
#endif
