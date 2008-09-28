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

#include <string.h>
#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/*** Local Objects ***/
Etk_Widget *UI_ColorsWin;
Etk_Widget *UI_ColorsList;
Etk_Widget *UI_ColorsNameEntry;
Etk_Widget *UI_Color1;
Etk_Widget *UI_Color2;
Etk_Widget *UI_Color3;
Etk_Widget *UI_ColorsApplyButton;

/***   Local Callbacks   ***/
Etk_Bool _colors_list_row_selected_cb(Etk_Tree *tree, Etk_Tree_Row *row, void *data);
Etk_Bool _colors_apply_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _colors_add_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _colors_del_button_click_cb(Etk_Button *button, void *data);


/***   Implementation   ***/
Etk_Widget*
colors_window_create(void)
{
   Etk_Widget *hbox, *vbox, *hbox2, *vbox2, *vbox3;
   Etk_Tree_Col *col;
   Etk_Widget *button;
   Etk_Widget *frame;
   Etk_Widget *label;

   //DataWin
   UI_ColorsWin = etk_window_new();
   etk_window_title_set(ETK_WINDOW(UI_ColorsWin), "Color Classes Browser");
   etk_window_resize(ETK_WINDOW(UI_ColorsWin), 400, 370);
   etk_signal_connect("delete-event", ETK_OBJECT(UI_ColorsWin),
                      ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_container_border_width_set(ETK_CONTAINER(UI_ColorsWin), 5);

   vbox = etk_vbox_new(ETK_FALSE, 5);
   etk_container_add(ETK_CONTAINER(UI_ColorsWin), vbox);

   //Description label
   label = etk_label_new("Color classes...some description....");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 7);

   hbox = etk_hbox_new(ETK_TRUE, 5);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(hbox), vbox2, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_ColorsList
   UI_ColorsList = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(UI_ColorsList), ETK_TREE_MODE_LIST);
   etk_widget_size_request_set(UI_ColorsList, 225, 200);
   etk_tree_headers_visible_set(ETK_TREE(UI_ColorsList), ETK_TRUE);
   etk_tree_column_separators_visible_set(ETK_TREE(UI_ColorsList), ETK_TRUE);
   etk_tree_alternating_row_colors_set(ETK_TREE(UI_ColorsList), ETK_TRUE);
   col = etk_tree_col_new(ETK_TREE(UI_ColorsList), "Name", 100, 0.0);
   etk_tree_col_model_add(col, etk_tree_model_text_new());
   //~ col = etk_tree_col_new(ETK_TREE(UI_ColorsList), "Value", 120, 0.0);
   //~ etk_tree_col_model_add(col, etk_tree_model_text_new());
   etk_tree_build(ETK_TREE(UI_ColorsList));
   etk_box_append(ETK_BOX(vbox2), UI_ColorsList, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_signal_connect("row-selected", ETK_OBJECT(UI_ColorsList),
                      ETK_CALLBACK(_colors_list_row_selected_cb), NULL);

   hbox2 = etk_hbox_new(ETK_TRUE, 0);
   etk_box_append(ETK_BOX(vbox2), hbox2, ETK_BOX_START, ETK_BOX_NONE, 0);

   //AddColorButton
   button = etk_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
                      ETK_CALLBACK(_colors_add_button_click_cb), NULL);
   etk_box_append(ETK_BOX(hbox2), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //DelColorButton
   button = etk_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
                      ETK_CALLBACK(_colors_del_button_click_cb), NULL);
   etk_box_append(ETK_BOX(hbox2), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(hbox), vbox2, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Color Classes Editor
   frame = etk_frame_new("Color Class Editor");
   etk_box_append(ETK_BOX(vbox2), frame, ETK_BOX_START, ETK_BOX_FILL, 0);

   vbox3 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox3);

   //Name Entry
   label = etk_label_new("<b>Name</b>");
   etk_box_append(ETK_BOX(vbox3), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   UI_ColorsNameEntry = etk_entry_new();
   etk_box_append(ETK_BOX(vbox3), UI_ColorsNameEntry,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Color1 rect
   UI_Color1 = window_color_button_create("Normal color", COLOR_OBJECT_CC1, 50, 30,
                                          etk_widget_toplevel_evas_get(vbox3));
   etk_box_append(ETK_BOX(vbox3), UI_Color1, ETK_BOX_START,
                  ETK_BOX_EXPAND_FILL, 0);
   
   //Color2 rect
   UI_Color2 = window_color_button_create("Second color", COLOR_OBJECT_CC2, 50, 30,
                                          etk_widget_toplevel_evas_get(vbox3));
   etk_box_append(ETK_BOX(vbox3), UI_Color2, ETK_BOX_START,
                  ETK_BOX_EXPAND_FILL, 0);
   
   //Color3 rect
   UI_Color3 = window_color_button_create("Third color", COLOR_OBJECT_CC3, 50, 30,
                                          etk_widget_toplevel_evas_get(vbox3));
   etk_box_append(ETK_BOX(vbox3), UI_Color3, ETK_BOX_START,
                  ETK_BOX_EXPAND_FILL, 0);
   
   //Apply button
   UI_ColorsApplyButton =etk_button_new_from_stock(ETK_STOCK_DIALOG_APPLY);
   etk_signal_connect("clicked", ETK_OBJECT(UI_ColorsApplyButton),
                      ETK_CALLBACK(_colors_apply_button_click_cb), NULL);
   etk_box_append(ETK_BOX(vbox3), UI_ColorsApplyButton,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 15);

   return UI_ColorsWin;
}

void
colors_window_populate(void)
{
   Evas_List *l, *classes;
   Etk_Tree_Col *col1;

   col1 = etk_tree_nth_col_get(ETK_TREE(UI_ColorsList), 0);

   etk_tree_freeze(ETK_TREE(UI_ColorsList));
   etk_tree_clear(ETK_TREE(UI_ColorsList));

   classes = edje_edit_color_classes_list_get(edje_o);
   for(l = classes; l; l = l->next)
      etk_tree_row_append(ETK_TREE(UI_ColorsList), NULL,
                          col1, l->data,
                          NULL);

   edje_edit_string_list_free(classes);

   etk_tree_thaw(ETK_TREE(UI_ColorsList));
   
   etk_widget_disabled_set(UI_ColorsApplyButton, ETK_TRUE);
   etk_entry_text_set(ETK_ENTRY(UI_ColorsNameEntry), "");
}

void
colors_window_show(void)
{
   colors_window_populate();
   etk_widget_show_all(UI_ColorsWin);
}

/* Colors Windows Callbacks */
Etk_Bool
_colors_list_row_selected_cb(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
{
   Etk_Tree_Col *col1;
   Etk_Color c1, c2, c3;
   const char *name;

   col1 = etk_tree_nth_col_get(ETK_TREE(UI_ColorsList), 0);

   etk_tree_row_fields_get(row, col1, &name, NULL);
   etk_entry_text_set(ETK_ENTRY(UI_ColorsNameEntry), name);
   
   edje_edit_color_class_colors_get(edje_o, name, &c1.r, &c1.g, &c1.b, &c1.a,
                                                  &c2.r, &c2.g, &c2.b, &c2.a,
                                                  &c3.r, &c3.g, &c3.b, &c3.a);
   evas_object_color_set(ColorClassC1, c1.r, c1.g, c1.b, c1.a);
   evas_object_color_set(ColorClassC2, c2.r, c2.g, c2.b, c2.a);
   evas_object_color_set(ColorClassC3, c3.r, c3.g, c3.b, c3.a);
  
   etk_widget_disabled_set(UI_ColorsApplyButton, ETK_FALSE);

   return ETK_TRUE;
}

Etk_Bool
_colors_apply_button_click_cb(Etk_Button *button, void *data)
{
   Etk_Tree_Col *col1;
   Etk_Tree_Row *row;
   Etk_Color c1, c2, c3;
   const char *name, *new_name;

   new_name = etk_entry_text_get(ETK_ENTRY(UI_ColorsNameEntry));
   row = etk_tree_selected_row_get(ETK_TREE(UI_ColorsList));
   col1 = etk_tree_nth_col_get(ETK_TREE(UI_ColorsList), 0);
   etk_tree_row_fields_get(row, col1, &name, NULL);
   
   if (strcmp(name, new_name))
     {
        edje_edit_color_class_name_set(edje_o, name, new_name);
        etk_tree_row_fields_set(row, ETK_FALSE, col1, new_name, NULL);
     }
   
   evas_object_color_get(ColorClassC1, &c1.r, &c1.g, &c1.b, &c1.a);
   evas_object_color_get(ColorClassC2, &c2.r, &c2.g, &c2.b, &c2.a);
   evas_object_color_get(ColorClassC3, &c3.r, &c3.g, &c3.b, &c3.a);
   
   edje_edit_color_class_colors_set(edje_o, new_name,
                                    c1.r, c1.g, c1.b, c1.a,
                                    c2.r, c2.g, c2.b, c2.a,
                                    c3.r, c3.g, c3.b, c3.a);
   
   return ETK_TRUE;
}

Etk_Bool
_colors_add_button_click_cb(Etk_Button *button, void *data)
{
   char buf[64];
   int i = 0;
   Etk_Tree_Row *last_row;
   
   do snprintf(buf, sizeof(buf), "New ColorClass %d", ++i);
   while (!edje_edit_color_class_add(edje_o, buf) && i < 100);
      
   colors_window_populate();
   
   last_row = etk_tree_last_row_get(ETK_TREE(UI_ColorsList));
   etk_tree_row_select(last_row);
   etk_tree_row_scroll_to(last_row, 1);
   
   return ETK_TRUE;
}

Etk_Bool
_colors_del_button_click_cb(Etk_Button *button, void *data)
{
   const char *name;
   Etk_Tree_Row *row, *next_row;

   name = etk_entry_text_get(ETK_ENTRY(UI_ColorsNameEntry));
   if (!name) return ETK_TRUE;

   row = etk_tree_selected_row_get(ETK_TREE(UI_ColorsList));
   next_row = etk_tree_row_next_get(row);

   edje_edit_color_class_del(edje_o, name);
   etk_tree_row_delete(row);

   if (!next_row) next_row = etk_tree_last_row_get(ETK_TREE(UI_ColorsList));
   etk_tree_row_select(next_row);

   return ETK_TRUE;
}
