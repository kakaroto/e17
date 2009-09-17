/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "etk_test.h"
#include "config.h"

/**************************
 *
 * Creation of the test-app window
 *
 **************************/

/* Creates the window for the table test */
void etk_test_table_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox, *hbox;
   Etk_Widget *table;
   Etk_Widget *image;
   Etk_Widget *buttons[5];
   Etk_Widget *labels[8];
   Etk_Widget *entries[6];
   int i;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Table Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   /* First we create the widgets to pack into the table */
   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png", NULL);

   buttons[0] = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   buttons[1] = etk_check_button_new();
   buttons[2] = etk_check_button_new();
   buttons[3] = etk_button_new_from_stock(ETK_STOCK_DIALOG_CANCEL);
   buttons[4] = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);

   labels[0] = etk_label_new("App Name");
   labels[1] = etk_label_new("Generic Info");
   labels[2] = etk_label_new("Comments");
   labels[3] = etk_label_new("Executable");
   labels[4] = etk_label_new("Window Name");
   labels[5] = etk_label_new("Window Class");
   labels[6] = etk_label_new("Startup Notify");
   labels[7] = etk_label_new("Wait Exit");

   for (i = 0; i < 6; i++)
      entries[i] = etk_entry_new();


   /* And then, we create the table and we pack the widgets into it */
   table = etk_table_new(2, 10, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_table_attach(ETK_TABLE(table), image, 0, 0, 0, 0, ETK_TABLE_NONE, 0, 0);
   etk_table_attach(ETK_TABLE(table), buttons[0], 1, 1, 0, 0, ETK_TABLE_HEXPAND, 0, 0);

   for (i = 0; i < 6; i++)
   {
      etk_table_attach(ETK_TABLE(table), labels[i], 0, 0, 2 + i, 2 + i, ETK_TABLE_HFILL, 0, 0);
      etk_table_attach_default(ETK_TABLE(table), entries[i], 1, 1, 2 + i, 2 + i);
   }

   etk_table_attach(ETK_TABLE(table), labels[6], 0, 0, 8, 8, ETK_TABLE_HFILL, 0, 0);
   etk_table_attach_default(ETK_TABLE(table), buttons[1], 1, 1, 8, 8);
   etk_table_attach(ETK_TABLE(table), labels[7], 0, 0, 9, 9, ETK_TABLE_HFILL, 0, 0);
   etk_table_attach_default(ETK_TABLE(table), buttons[2], 1, 1, 9, 9);


   vbox = etk_vbox_new(ETK_FALSE, 0);
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   etk_box_append(ETK_BOX(vbox), table, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(hbox), buttons[3], ETK_BOX_END, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(hbox), buttons[4], ETK_BOX_END, ETK_BOX_NONE, 0);

   etk_widget_show_all(win);
}
