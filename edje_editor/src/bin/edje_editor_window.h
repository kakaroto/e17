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

#ifndef _EDJE_EDITOR_INTERFACE_H_
#define _EDJE_EDITOR_INTERFACE_H_


/* main windows objects */
Evas        *UI_evas;
Ecore_Evas  *UI_ecore_MainWin;
Evas_Object *edje_ui;


/* etk_embed objects */
Etk_Widget *UI_PartsTreeEmbed;
Etk_Widget *UI_GroupEmbed;
Etk_Widget *UI_PartEmbed;
Etk_Widget *UI_DescriptionEmbed;
Etk_Widget *UI_RectEmbed;
Etk_Widget *UI_TextEmbed;
Etk_Widget *UI_PositionEmbed;
Etk_Widget *UI_ProgramEmbed;
Etk_Widget *UI_ScriptEmbed;
Etk_Widget *UI_ImageEmbed;
Etk_Widget *UI_GradientEmbed;
Etk_Widget *UI_FillEmbed;
Etk_Widget *UI_ToolbarEmbed;


void        window_main_create         (void);
Etk_Widget* window_color_button_create (char* label_text, int color_button_enum,int w,int h, Evas *evas);
void        window_embed_position_set  (void *position_data, int *x, int *y);

void     _window_delete_cb           (Ecore_Evas *ee);
void     _window_resize_cb           (Ecore_Evas *ecore_evas);

void     _window_edit_obj_signal_cb  (void *data, Evas_Object *o, const char *sig, const char *src);
void     _window_edit_obj_click      (void *data, Evas *e, Evas_Object *obj, void *event_info);

Etk_Bool _window_all_button_click_cb (Etk_Button *button, void *data);
void     _window_logo_key_press      (void *data, Evas *e, Evas_Object *obj, void *event_info);
void     _window_color_canvas_click  (void *data, Evas *e, Evas_Object *obj, void *event_info);

void     _window_confirm_save        (Etk_Dialog *dialog, int response_id, void *data);


#endif
