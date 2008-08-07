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

#ifndef _EDJE_EDITOR_GROUP_H_
#define _EDJE_EDITOR_GROUP_H_


Etk_Widget *UI_GroupNameEntry;
Etk_Widget *UI_GroupNameEntryImage;
Etk_Widget *UI_GroupMinWSpinner;
Etk_Widget *UI_GroupMinHSpinner;
Etk_Widget *UI_GroupMaxWSpinner;
Etk_Widget *UI_GroupMaxHSpinner;
Etk_Widget *UI_CurrentGroupSizeLabel;


Etk_Widget* group_frame_create(void);
void        group_frame_update(void);


Etk_Bool _group_NamesEntry_text_changed_cb (Etk_Object *object, void *data);
Etk_Bool _group_spinners_value_changed_cb  (Etk_Range *range, double value, void *data);
Etk_Bool _group_NameEntry_key_down_cb      (Etk_Object *object, Etk_Event_Key_Down *event, void *data);
Etk_Bool _group_NameEntryImage_clicked_cb  (Etk_Object *object, void *data);


#endif
