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

#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/*** Local Objects ***/
Etk_Widget *UI_DataWin;
Etk_Widget *UI_DataList;
Etk_Widget *UI_DataNameEntry;
Etk_Widget *UI_DataValueEntry;
Etk_Widget *UI_DataApplyButton;


/***   Local Callbacks   ***/
Etk_Bool _data_list_row_selected_cb(Etk_Tree *tree, Etk_Tree_Row *row, void *data);
Etk_Bool _data_apply_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _data_add_button_click_cb(Etk_Button *button, void *data);
Etk_Bool _data_del_button_click_cb(Etk_Button *button, void *data);


/***   Implementation   ***/
Etk_Widget*
data_window_create(void)
{
   Etk_Widget *hbox, *vbox, *hbox2, *vbox2, *vbox3;
   Etk_Tree_Col *col;
   Etk_Widget *button;
   Etk_Widget *frame;
   Etk_Widget *label;

   //DataWin
   UI_DataWin = etk_window_new();
   etk_window_title_set(ETK_WINDOW(UI_DataWin), "Data Browser");
   etk_window_resize(ETK_WINDOW(UI_DataWin), 400, 370);
   etk_signal_connect("delete-event", ETK_OBJECT(UI_DataWin),
                      ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_container_border_width_set(ETK_CONTAINER(UI_DataWin), 5);

   vbox = etk_vbox_new(ETK_FALSE, 5);
   etk_container_add(ETK_CONTAINER(UI_DataWin), vbox);

   //Description label
   label = etk_label_new("Data stored inside the edje file that can be accessed by the application.");
   etk_object_properties_set(ETK_OBJECT(label), "xalign", 0.5, NULL);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 7);

   hbox = etk_hbox_new(ETK_TRUE, 5);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(hbox), vbox2, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //UI_DataList
   UI_DataList = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(UI_DataList), ETK_TREE_MODE_LIST);
   etk_widget_size_request_set(UI_DataList, 225, 200);
   etk_tree_headers_visible_set(ETK_TREE(UI_DataList), ETK_TRUE);
   etk_tree_column_separators_visible_set(ETK_TREE(UI_DataList), ETK_TRUE);
   etk_tree_alternating_row_colors_set(ETK_TREE(UI_DataList), ETK_TRUE);
   col = etk_tree_col_new(ETK_TREE(UI_DataList), "Name", 100, 0.0);
   etk_tree_col_model_add(col, etk_tree_model_text_new());
   col = etk_tree_col_new(ETK_TREE(UI_DataList), "Value", 120, 0.0);
   etk_tree_col_model_add(col, etk_tree_model_text_new());
   etk_tree_build(ETK_TREE(UI_DataList));
   etk_box_append(ETK_BOX(vbox2), UI_DataList, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_signal_connect("row-selected", ETK_OBJECT(UI_DataList),
                      ETK_CALLBACK(_data_list_row_selected_cb), NULL);

   hbox2 = etk_hbox_new(ETK_TRUE, 0);
   etk_box_append(ETK_BOX(vbox2), hbox2, ETK_BOX_START, ETK_BOX_NONE, 0);

   //AddDataButton
   button = etk_button_new_from_stock(ETK_STOCK_LIST_ADD);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
                      ETK_CALLBACK(_data_add_button_click_cb), NULL);
   etk_box_append(ETK_BOX(hbox2), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //DelDataButton
   button = etk_button_new_from_stock(ETK_STOCK_LIST_REMOVE);
   etk_button_style_set(ETK_BUTTON(button), ETK_BUTTON_ICON);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
                      ETK_CALLBACK(_data_del_button_click_cb), NULL);
   etk_box_append(ETK_BOX(hbox2), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   vbox2 = etk_vbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(hbox), vbox2, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Data Editor
   frame = etk_frame_new("Data Editor");
   etk_box_append(ETK_BOX(vbox2), frame, ETK_BOX_START, ETK_BOX_FILL, 0);

   vbox3 = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox3);

   //Name Entry
   label = etk_label_new("<b>Name</b>");
   etk_box_append(ETK_BOX(vbox3), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   UI_DataNameEntry = etk_entry_new();
   etk_widget_disabled_set(UI_DataNameEntry, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox3), UI_DataNameEntry,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Value Entry
   label = etk_label_new("<b>Value</b>");
   etk_box_append(ETK_BOX(vbox3), label, ETK_BOX_START, ETK_BOX_NONE, 0);

   UI_DataValueEntry = etk_entry_new();
   etk_box_append(ETK_BOX(vbox3), UI_DataValueEntry,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   //Apply button
   UI_DataApplyButton =etk_button_new_from_stock(ETK_STOCK_DIALOG_APPLY);
   etk_signal_connect("clicked", ETK_OBJECT(UI_DataApplyButton),
                      ETK_CALLBACK(_data_apply_button_click_cb), NULL);
   etk_box_append(ETK_BOX(vbox3), UI_DataApplyButton,
                  ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   return UI_DataWin;
}

void
data_window_populate(void)
{
   Evas_List *l, *datas;
   Etk_Tree_Col *col1, *col2;

   col1 = etk_tree_nth_col_get(ETK_TREE(UI_DataList), 0);
   col2 = etk_tree_nth_col_get(ETK_TREE(UI_DataList), 1);

   etk_tree_freeze(ETK_TREE(UI_DataList));
   etk_tree_clear(ETK_TREE(UI_DataList));

   datas = edje_edit_data_list_get(edje_o);
   for(l = datas; l; l = l->next)
   {
      const char *val;

      val = edje_edit_data_value_get(edje_o, l->data);
      etk_tree_row_append(ETK_TREE(UI_DataList), NULL,
                    col1, l->data,
                    col2, val,
                    NULL);
      edje_edit_string_free(val);
   }
   edje_edit_string_list_free(datas);

   etk_tree_thaw(ETK_TREE(UI_DataList));
   
   etk_widget_disabled_set(UI_DataApplyButton, ETK_TRUE);
   etk_entry_text_set(ETK_ENTRY(UI_DataNameEntry), "");
   etk_entry_text_set(ETK_ENTRY(UI_DataValueEntry), "");
}

void
data_window_show(void)
{
   data_window_populate();
   etk_widget_show_all(UI_DataWin);
}

/* Data Windows Callbacks */
Etk_Bool
_data_list_row_selected_cb(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
{
   Etk_Tree_Col *col1, *col2;
   const char *name, *value;

   col1 = etk_tree_nth_col_get(ETK_TREE(UI_DataList), 0);
   col2 = etk_tree_nth_col_get(ETK_TREE(UI_DataList), 1);

   etk_tree_row_fields_get(row, col1, &name, NULL);
   etk_tree_row_fields_get(row, col2, &value, NULL);

   etk_entry_text_set(ETK_ENTRY(UI_DataNameEntry), name);
   etk_entry_text_set(ETK_ENTRY(UI_DataValueEntry), value);

   etk_widget_disabled_set(UI_DataApplyButton, ETK_FALSE);

   return ETK_TRUE;
}

Etk_Bool
_data_apply_button_click_cb(Etk_Button *button, void *data)
{
   Etk_Tree_Col *col2;
   Etk_Tree_Row *row;
   const char *name, *value;
   
   name = etk_entry_text_get(ETK_ENTRY(UI_DataNameEntry));
   value = etk_entry_text_get(ETK_ENTRY(UI_DataValueEntry));
   edje_edit_data_value_set(edje_o, name, value);

   row = etk_tree_selected_row_get(ETK_TREE(UI_DataList));
   col2 = etk_tree_nth_col_get(ETK_TREE(UI_DataList), 1);
   etk_tree_row_fields_set(row, ETK_FALSE, col2, value, NULL);
   
   return ETK_TRUE;
}

Etk_Bool
_data_add_button_click_cb(Etk_Button *button, void *data)
{
   char buf[64];
   int i = 0;
   Etk_Tree_Row *last_row;
   
   do snprintf(buf, sizeof(buf), "New data %d", ++i);
   while (!edje_edit_data_add(edje_o, buf, "New value") && i < 100);
      
   data_window_populate();
   
   last_row = etk_tree_last_row_get(ETK_TREE(UI_DataList));
   etk_tree_row_select(last_row);
   etk_tree_row_scroll_to(last_row, 1);
   
   return ETK_TRUE;
}

Etk_Bool
_data_del_button_click_cb(Etk_Button *button, void *data)
{
   const char *name;
   Etk_Tree_Row *row, *next_row;

   name = etk_entry_text_get(ETK_ENTRY(UI_DataNameEntry));
   if (!name) return ETK_TRUE;

   row = etk_tree_selected_row_get(ETK_TREE(UI_DataList));
   next_row = etk_tree_row_next_get(row);

   edje_edit_data_del(edje_o, name);
   etk_tree_row_delete(row);

   if (!next_row) next_row = etk_tree_last_row_get(ETK_TREE(UI_DataList));
   etk_tree_row_select(next_row);

   return ETK_TRUE;
}
