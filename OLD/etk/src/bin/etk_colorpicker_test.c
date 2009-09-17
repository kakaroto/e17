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

/* Creates the window for the color picker test */
void etk_test_colorpicker_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *cp;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Color Picker Test");
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   cp = etk_colorpicker_new();
   etk_colorpicker_use_alpha_set(ETK_COLORPICKER(cp), ETK_TRUE);
   etk_container_add(ETK_CONTAINER(win), cp);

   etk_widget_show_all(win);
}
