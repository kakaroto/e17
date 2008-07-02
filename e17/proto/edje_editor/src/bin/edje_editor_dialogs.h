#ifndef _EDJE_EDITOR_DIALOGS_H_
#define _EDJE_EDITOR_DIALOGS_H_


Etk_Widget *UI_AlertDialog;

Etk_Widget *UI_FileChooser;
Etk_Widget *UI_FileChooserDialog;
Etk_Widget *UI_FilechooserSaveButton;
Etk_Widget *UI_FilechooserLoadButton;

Etk_Widget *UI_ColorWin;
Etk_Widget *UI_ColorPicker;
int current_color_object;


Etk_Widget* create_alert_dialog(void);
void ShowAlert(char* text);

Etk_Widget* create_filechooser_dialog(void);
void ShowFilechooser(int FileChooserType);

Etk_Widget *create_colorpicker(void);


Etk_Bool on_FileChooserDialog_response     (Etk_Dialog *dialog, int response_id, void *data);
Etk_Bool on_FileChooser_selected           (Etk_Filechooser_Widget *filechooser);
Etk_Bool on_AlertDialog_response           (Etk_Dialog *dialog, int response_id, void *data);

Etk_Bool on_ColorDialog_change             (Etk_Object *object, void *data);



#endif
