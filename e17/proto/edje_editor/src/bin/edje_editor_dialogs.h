/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

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
