/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

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
