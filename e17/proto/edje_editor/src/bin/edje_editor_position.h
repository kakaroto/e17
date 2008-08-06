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

#ifndef _EDJE_EDITOR_POSITION_H_
#define _EDJE_EDITOR_POSITION_H_


/* position frame objects */
Etk_Widget *UI_Rel1XSpinner;
Etk_Widget *UI_Rel1XOffsetSpinner;
Etk_Widget *UI_Rel1YSpinner;
Etk_Widget *UI_Rel1YOffsetSpinner;
Etk_Widget *UI_Rel2XSpinner;
Etk_Widget *UI_Rel2XOffsetSpinner;
Etk_Widget *UI_Rel2YSpinner;
Etk_Widget *UI_Rel2YOffsetSpinner;
Etk_Widget *UI_Rel1ToXComboBox;
Etk_Widget *UI_Rel1ToYComboBox;
Etk_Widget *UI_Rel2ToXComboBox;
Etk_Widget *UI_Rel2ToYComboBox;


Etk_Widget* position_frame_create(void);
void        position_frame_update(void);
void        position_comboboxes_populate(void);
void        position_comboboxes_update(void);


Etk_Bool _position_RelOffsetSpinners_value_changed_cb (Etk_Range *range, double value, void *data);
Etk_Bool _position_RelSpinners_value_changed_cb       (Etk_Range *range, double value, void *data);
Etk_Bool _position_RelToComboBoxes_changed_cb         (Etk_Combobox *combobox, void *data);


#endif
