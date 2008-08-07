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

#ifndef _EDJE_EDITOR_SPECTRA_H_
#define _EDJE_EDITOR_SPECTRA_H_


Etk_Widget *UI_SpectrumWin;
Etk_Widget *UI_SpectrumList;
Etk_Widget *UI_SpectraWidget;


Etk_Widget* spectra_window_create   (void);
void        spectra_window_show     (void);
void        spectra_window_populate (void);
Etk_Widget* spectra_thumb_create    (Etk_Widget *parent, const char *spectra);

void //TODO this should be done as a new Etk_Event with a callback
spectra_window_gradient_changed(Etk_Widget *spectra);
void //TODO this should be done as a new Etk_Event with a callback
spectra_window_gradient_name_changed(Etk_Widget *spectra);

Etk_Bool _spectra_window_hide_cb(Etk_Object *window,void *data);
Etk_Bool _spectra_list_row_selected_cb(Etk_Tree *tree, Etk_Tree_Row *row, void *data);
Etk_Bool _spectra_add_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _spectra_del_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _spectra_thumb_destroy(Etk_Object *object, void *data);


#endif
