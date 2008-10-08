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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_test.h"

#include <Evas.h>
#include <stdlib.h>

/* Creates the window for the mdi test */
void etk_test_mdi_window_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *mdi_area;
   Etk_Widget *mdi_window;
   Etk_Widget *down_image;
   Etk_Widget *next_image;
   Etk_Widget *prev_image;
   Etk_Widget *up_image;
   Etk_Widget *table;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_resize(ETK_WINDOW(win), 300, 300);
   etk_window_title_set(ETK_WINDOW(win), "Etk Mdi Window Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   down_image = etk_image_new_from_stock(ETK_STOCK_GO_DOWN, ETK_STOCK_BIG);
   next_image = etk_image_new_from_stock(ETK_STOCK_GO_NEXT, ETK_STOCK_BIG);
   prev_image = etk_image_new_from_stock(ETK_STOCK_GO_PREVIOUS, ETK_STOCK_BIG);
   up_image = etk_image_new_from_stock(ETK_STOCK_GO_UP, ETK_STOCK_BIG);

   table = etk_table_new(3, 3, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_container_add(ETK_CONTAINER(win), table);

   etk_table_attach(ETK_TABLE(table), down_image, 0, 2, 0, 0, ETK_TABLE_NONE, 0, 0);
   etk_table_attach(ETK_TABLE(table), next_image, 0, 0, 1, 1, ETK_TABLE_NONE, 0, 0);
   etk_table_attach(ETK_TABLE(table), prev_image, 2, 2, 1, 1, ETK_TABLE_NONE, 0, 0);
   etk_table_attach(ETK_TABLE(table), up_image, 0, 2, 2, 2, ETK_TABLE_NONE, 0, 0);

   mdi_area = etk_mdi_area_new();
   etk_table_attach_default(ETK_TABLE(table), mdi_area, 1, 1, 1, 1);

   mdi_window = etk_mdi_window_new();
   etk_mdi_window_title_set(ETK_MDI_WINDOW(mdi_window), "Mdi Window 1");
   etk_widget_size_request_set(ETK_WIDGET(mdi_window), 100, 100);
   etk_container_add(ETK_CONTAINER(mdi_area), mdi_window);

   mdi_window = etk_mdi_window_new();
   etk_mdi_window_title_set(ETK_MDI_WINDOW(mdi_window), "Mdi Window 2");
   etk_mdi_window_move(ETK_MDI_WINDOW(mdi_window), 100, 100);
   etk_widget_size_request_set(ETK_WIDGET(mdi_window), 100, 100);
   etk_container_add(ETK_CONTAINER(mdi_area), mdi_window);

   etk_widget_show_all(win);
}
