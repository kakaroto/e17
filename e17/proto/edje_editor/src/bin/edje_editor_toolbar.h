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

#ifndef _EDJE_EDITOR_TOOLBAR_H_
#define _EDJE_EDITOR_TOOLBAR_H_


/* toolbar objects */

Etk_Widget *UI_Toolbar;
Etk_Widget *UI_AddMenu;
Etk_Widget *UI_RemoveMenu;
Etk_Widget *UI_OptionsMenu;

Etk_Widget *UI_PlayButton;
Etk_Widget *UI_PlayImage;
Etk_Widget *UI_PauseImage;
Etk_Widget *UI_AddStateButton;
Etk_Widget *UI_RemoveStateButton;
Etk_Widget *UI_RemovePartButton;
Etk_Widget *UI_RemoveProgramButton;


Etk_Widget* toolbar_create             (Etk_Toolbar_Orientation o);
void        toolbar_play_button_toggle (int set);

Etk_Bool _toolbar_add_menu_item_activated_cb    (Etk_Object *object, void *data);
Etk_Bool _toolbar_remove_menu_item_activated_cb (Etk_Object *object, void *data);



#endif
