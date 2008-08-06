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

#include <Evas.h>
#include <Edje.h>

void etk_test_evas_object_window_create(void *data)
{
	Evas *evas;
	Evas_Object *edje_object;
	Evas_Object *evas_object;
	static Etk_Widget *window = NULL;
	Etk_Widget *vbox;
	Etk_Widget *frame;
	Etk_Widget *etk_evas_object;
	int w, h;

	if (window)
	{
		etk_widget_show_all(window);
		return;
	}

	window = etk_window_new();
  etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(window), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
	etk_window_title_set(ETK_WINDOW(window), "Etk-Evas Object Test");

	evas = etk_toplevel_evas_get(ETK_TOPLEVEL(window));

	vbox = etk_vbox_new(ETK_FALSE, 5);

	/* Add an edje object */
	frame = etk_frame_new("Edje Object");
	edje_object = edje_object_add(evas);
	edje_object_file_set(edje_object, PACKAGE_DATA_DIR"/test/clock.edj", "icon"); 
	edje_object_size_min_get(edje_object, &w, &h);
	edje_extern_object_min_size_set(edje_object, w, h);
	evas_object_resize(edje_object, w, h);
	etk_evas_object = etk_evas_object_new();
	etk_evas_object_set_object(ETK_EVAS_OBJECT(etk_evas_object), edje_object);
	etk_container_add(ETK_CONTAINER(frame), etk_evas_object);
	etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	/* Add an evas rectangle */
	frame = etk_frame_new("Evas Object");
	evas_object = evas_object_rectangle_add(evas);
	evas_object_resize(evas_object, 150, 150);
	evas_object_color_set(evas_object, 150, 100, 100, 255);
	etk_evas_object = etk_evas_object_new();
	etk_evas_object_set_object(ETK_EVAS_OBJECT(etk_evas_object), evas_object);
	etk_container_add(ETK_CONTAINER(frame), etk_evas_object);
	etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_END, ETK_BOX_EXPAND_FILL, 0);

	etk_container_add(ETK_CONTAINER(window), vbox);
	etk_widget_show_all(window);
}

