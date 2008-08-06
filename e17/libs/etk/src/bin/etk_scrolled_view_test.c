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

#define BUTTON_WIDTH 150
#define BUTTON_HEIGHT 40
#define SPACING 5
#define NUM_COLS 5
#define NUM_ROWS 15

/* Creates the window for the scrolled view test */
void etk_test_scrolled_view_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *scrolled_view;
   Etk_Widget *canvas;
   Etk_Widget *button;
   int i, j;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Scrolled View Test");
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_widget_size_request_set(win, 180, 180);

   scrolled_view = etk_scrolled_view_new();
   etk_scrolled_view_dragable_set(ETK_SCROLLED_VIEW(scrolled_view), 1);
   etk_container_add(ETK_CONTAINER(win), scrolled_view);

   canvas = etk_canvas_new();
   etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(scrolled_view), canvas);

   for (i = 0; i < NUM_COLS; i++)
   {
      for (j = 0; j < NUM_ROWS; j++)
      {
         button = etk_button_new_with_label("Scrolled View Test");
         etk_widget_size_request_set(button, BUTTON_WIDTH, BUTTON_HEIGHT);
         etk_canvas_put(ETK_CANVAS(canvas), button, (BUTTON_WIDTH + SPACING) * i, (BUTTON_HEIGHT + SPACING) * j);
      }
   }

   etk_widget_show_all(win);
}
