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

#ifndef _EDJE_EDITOR_OBJ_GRADIENT_H_
#define _EDJE_EDITOR_OBJ_GRADIENT_H_



Etk_Widget *UI_GradientTypeComboBox;
Etk_Widget *UI_GradientSpectraComboBox;
Etk_Widget *UI_GradientAlphaSlider;
Etk_Widget *UI_GradientRel1XSpinner;
Etk_Widget *UI_GradientRel1XOffsetSpinner;
Etk_Widget *UI_GradientRel1YSpinner;
Etk_Widget *UI_GradientRel1YOffsetSpinner;
Etk_Widget *UI_GradientRel2XSpinner;
Etk_Widget *UI_GradientRel2XOffsetSpinner;
Etk_Widget *UI_GradientRel2YSpinner;
Etk_Widget *UI_GradientRel2YOffsetSpinner;


Etk_Widget* gradient_frame_create(void);
void        gradient_frame_update(void);
void        gradient_spectra_combo_populate(void);

Etk_Bool _gradient_type_combo_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool _gradient_spectra_combo_activated_cb(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool _gradient_RelSpinners_value_changed_cb(Etk_Range *range, double value, void *data);

#endif
