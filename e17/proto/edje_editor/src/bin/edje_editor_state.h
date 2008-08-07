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
