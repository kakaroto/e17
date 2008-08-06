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

#include "Ecore.h"

static Ecore_Timer *_etk_test_progress_bar_timer = NULL;
static Ecore_Timer *_etk_test_progress_bar_timer2 = NULL;

static int _etk_test_progress_bar_update(void *data);
static int _etk_test_progress_bar_update2(void *data);

/* Creates the window for the progress bar test */
void etk_test_progress_bar_window_create(void *data)
{
   static Etk_Widget *win;
   Etk_Widget *vbox;
   Etk_Widget *pbar;
   Etk_Widget *pbar2;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Progress Bar Test");
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vbox = etk_vbox_new(ETK_TRUE, 5);
   etk_container_add(ETK_CONTAINER(win), vbox);

   pbar = etk_progress_bar_new_with_text("0% done");
   etk_box_append(ETK_BOX(vbox), pbar, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   pbar2 = etk_progress_bar_new_with_text("Loading...");
   etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(pbar2), 0.015);
   etk_box_append(ETK_BOX(vbox), pbar2, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   etk_widget_show_all(win);

   _etk_test_progress_bar_timer = ecore_timer_add(0.05, _etk_test_progress_bar_update, pbar);
   _etk_test_progress_bar_timer2 = ecore_timer_add(0.025, _etk_test_progress_bar_update2, pbar2);

   etk_signal_connect_swapped_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(pbar), ETK_CALLBACK(ecore_timer_del), _etk_test_progress_bar_timer);
   etk_signal_connect_swapped_by_code(ETK_OBJECT_DESTROYED_SIGNAL, ETK_OBJECT(pbar2), ETK_CALLBACK(ecore_timer_del), _etk_test_progress_bar_timer2);
}

/* Updates the first progress bar */
int _etk_test_progress_bar_update(void *data)
{
   Etk_Widget *pbar;
   double fraction;
   char text[32];

   pbar = data;

   fraction = etk_progress_bar_fraction_get(ETK_PROGRESS_BAR(pbar));
   fraction += 0.01;

   if (fraction > 1.0)
      fraction = 0.0;

   snprintf(text, sizeof(text), "%d%% done", (int)(fraction * 100.0));
   etk_progress_bar_text_set(ETK_PROGRESS_BAR(pbar), text);
   etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(pbar), fraction);

   return 1;
}

/* Updates the second progress bar */
int _etk_test_progress_bar_update2(void *data)
{
   Etk_Widget *pbar;

   pbar = data;
   etk_progress_bar_pulse(ETK_PROGRESS_BAR(pbar));

   return 1;
}
