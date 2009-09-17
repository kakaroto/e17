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

/* Creates the window for the image test */
void etk_test_image_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *images[2];
   Etk_Widget *labels[2];
   Etk_Widget *table;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Image Test");
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   /* TODO: we need a more complete test-app for Etk_Image */
   /* Create two images with different "keep-aspect" property and pack theme into a table */
   images[0] = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png", NULL);
   etk_image_keep_aspect_set(ETK_IMAGE(images[0]), ETK_TRUE);
   images[1] = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png", NULL);
   etk_image_keep_aspect_set(ETK_IMAGE(images[1]), ETK_FALSE);

   labels[0] = etk_label_new("Keep aspect");
   labels[1] = etk_label_new("Don't keep aspect");
   table = etk_table_new(2, 2, ETK_TABLE_HHOMOGENEOUS);

   etk_table_attach_default(ETK_TABLE(table), images[0], 0, 0, 0, 0);
   etk_table_attach_default(ETK_TABLE(table), images[1], 1, 1, 0, 0);
   etk_table_attach(ETK_TABLE(table), labels[0], 0, 0, 1, 1, ETK_TABLE_HEXPAND, 2, 0);
   etk_table_attach(ETK_TABLE(table), labels[1], 1, 1, 1, 1, ETK_TABLE_HEXPAND, 2, 0);

   etk_container_add(ETK_CONTAINER(win), table);

   etk_widget_show_all(win);
}
