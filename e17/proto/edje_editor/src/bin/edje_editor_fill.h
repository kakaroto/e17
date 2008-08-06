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

#ifndef _EDJE_EDITOR_FILL_H_
#define _EDJE_EDITOR_FILL_H_


Etk_Widget *UI_FillRelXSpinner;
Etk_Widget *UI_FillRelYSpinner;
Etk_Widget *UI_FillOffsetXSpinner;
Etk_Widget *UI_FillOffsetYSpinner;


Etk_Widget *UI_FillSizeRelXSpinner;
Etk_Widget *UI_FillSizeRelYSpinner;
Etk_Widget *UI_FillSizeOffsetXSpinner;
Etk_Widget *UI_FillSizeOffsetYSpinner;


Etk_Widget* fill_frame_create   (void);
void        fill_frame_update   (void);


Etk_Bool _fill_spinners_value_changed_cb(Etk_Range *range, double value, void *data);
Etk_Bool _fill_size_spinners_value_changed_cb(Etk_Range *range, double value, void *data);

#endif
