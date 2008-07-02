#ifndef _EDJE_EDITOR_DIALOGS_H_
#define _EDJE_EDITOR_DIALOGS_H_


//Alert Dialog
Etk_Widget  *UI_AlertDialog;

Etk_Widget*  dialog_alert_create (void);
void         dialog_alert_show   (char* text);


//FileChooser Dialog
Etk_Widget   *UI_FileChooser;
Etk_Widget   *UI_FileChooserDialog;
Etk_Widget   *UI_FilechooserSaveButton;
Etk_Widget   *UI_FilechooserLoadButton;

Etk_Widget*  dialog_filechooser_create (void);
void         dialog_filechooser_show   (int FileChooserType);


//Color Picker Dialog
Etk_Widget  *UI_ColorWin;
Etk_Widget  *UI_ColorPicker;
int          current_color_object;

Etk_Widget*  dialog_colorpicker_create (void);


/***   Callbacks   ***/
Etk_Bool _dialog_colorpicker_change_cb   (Etk_Object *object, void *data);
Etk_Bool _dialog_filechooser_response_cb (Etk_Dialog *dialog, int response_id, void *data);
Etk_Bool _dialog_filechooser_selected_cb (Etk_Filechooser_Widget *filechooser);

#endif
