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

#ifndef _EDJE_EDITOR_TEXT_H_
#define _EDJE_EDITOR_TEXT_H_


/* text frame objects */
Etk_Widget *UI_TextEntry;
Etk_Widget *UI_FontComboBox;
Etk_Widget *UI_FontSizeSpinner;
Etk_Widget *UI_EffectComboBox;
Etk_Widget *UI_FontAddButton;
Etk_Widget *UI_FontAlignVSpinner;
Etk_Widget *UI_FontAlignHSpinner;

Evas_Object *TextColorObject;
Evas_Object *ShadowColorObject;
Evas_Object *OutlineColorObject;


Etk_Widget* text_frame_create        (Evas *evas);
void        text_frame_update        (void);
void        text_font_combo_populate (void);


Etk_Bool _text_FontComboBox_item_activated_cb    (Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool _text_EffectComboBox_changed_cb         (Etk_Combobox *combobox, void *data);
Etk_Bool _text_FontSizeSpinner_value_changed_cb  (Etk_Range *range, double value, void *data);
Etk_Bool _text_Entry_text_changed_cb             (Etk_Object *object, void *data);
Etk_Bool _text_FontAlignSpinner_value_changed_cb (Etk_Range *range, double value, void *data);


#endif
