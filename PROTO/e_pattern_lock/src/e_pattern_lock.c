/*
 * vim:ts=4
 * 
 * Copyright © 2009-2010 Rui Miguel Silva Seabra <rms@1407.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <limits.h>
#include <stdlib.h>

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "Eina.h"
#include "Ecore_X.h"
#include "Elementary.h"

#include "e_pattern_lock.h"

Eina_Strbuf *password=NULL;
Eina_Bool mouse_down=EINA_FALSE;
char *stored_password="0x0,0x1,1x1,1x2,2x2";

void on_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	Evas_Object *object = (Evas_Object*)data;
	int col, row;
	char buf[4];

	if(password)
		eina_strbuf_reset(password);
	else
		password = eina_strbuf_new();

	col = atoi(edje_object_data_get(object, "col"));
	row = atoi(edje_object_data_get(object, "row"));

	snprintf(buf, 4, "%dx%d", col, row);
	eina_strbuf_append(password, buf);
	mouse_down=EINA_TRUE;

	printf(_("Mouse down on %dx%d\n"), col, row);
	printf(_("Password is '%s'\n"), eina_strbuf_string_get(password));
	edje_object_signal_emit(obj, "go_selected", NULL);
}

void on_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	Evas_Object *object = (Evas_Object*)data;
	int col, row;
	char buf[5];

	col = atoi(edje_object_data_get(object, "col"));
	row = atoi(edje_object_data_get(object, "row"));

	if(mouse_down == EINA_TRUE) {
		snprintf(buf, 5, ",%dx%d", col, row);
		eina_strbuf_append(password, buf);
		printf(_("Password is '%s'\n"), eina_strbuf_string_get(password));
	}
	printf(_("Mouse in on %dx%d\n"), col, row);
}

void on_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	Evas_Object *object = (Evas_Object*)data;
	int col, row;

	col = atoi(edje_object_data_get(object, "col"));
	row = atoi(edje_object_data_get(object, "row"));

	mouse_down=EINA_FALSE;

	printf(_("Mouse up on %dx%d\n"), col, row);
	printf(_("Final password is '%s'\n"), eina_strbuf_string_get(password));
	if(strcmp(stored_password, eina_strbuf_string_get(password)) == 0) {
		printf("Passwords match!\n");
		exit(0);
	}
}

#define set_event_cbs(col, row, part) \
		item = edje_object_part_table_child_get(edje, "table", col, row); \
		object = (Evas_Object*)edje_object_part_object_get(item, part); \
		evas_object_event_callback_add(object, EVAS_CALLBACK_MOUSE_IN, on_mouse_in, item); \
		evas_object_event_callback_add(object, EVAS_CALLBACK_MOUSE_DOWN, on_mouse_down, item); \
		evas_object_event_callback_add(object, EVAS_CALLBACK_MOUSE_UP, on_mouse_up, item);

EAPI int elm_main(int argc, char **argv) {
	Evas_Object *win=NULL, *bg=NULL, *pattern=NULL, *edje=NULL, *object=NULL, *item=NULL;
	Ecore_X_Window xwin, rootxwin;
	char theme[PATH_MAX];

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	win = elm_win_add(NULL, "e_pattern_lock", ELM_WIN_BASIC);
	elm_win_autodel_set(win, EINA_TRUE);
	elm_win_maximized_set(win, EINA_TRUE); // change to full screen and above everything for lockscreen effect
	evas_object_size_hint_min_set(win, 480, 480);
	evas_object_size_hint_max_set(win, 640, 640);
	elm_win_fullscreen_set(win, EINA_TRUE);
	elm_win_sticky_set(win, EINA_TRUE);
	elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_OFF);
	elm_win_layer_set(win, INT_MAX);

	xwin = elm_win_xwindow_get(win);
	rootxwin = ecore_x_window_root_get(xwin);
	ecore_x_pointer_grab(xwin);
	ecore_x_keyboard_grab(rootxwin);

	bg = elm_bg_add(win);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	elm_win_resize_object_add(win, bg);
	evas_object_show(bg);

	snprintf(theme, sizeof(theme), "%s/themes/default.edj", PKGDATADIR);
    elm_theme_extension_add(NULL, theme);
    elm_theme_overlay_add(NULL, theme);

	pattern = elm_layout_add(win);

		elm_layout_file_set(pattern, theme, "e_lock/table/default");
		evas_object_size_hint_weight_set(pattern, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(pattern, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_win_resize_object_add(win, pattern);

		edje = elm_layout_edje_get(pattern);

		set_event_cbs(0, 0, "top_left");    set_event_cbs(1, 0, "top");    set_event_cbs(2, 0, "top_right");
		set_event_cbs(0, 1, "left");        set_event_cbs(1, 1, "center"); set_event_cbs(2, 1, "right");
		set_event_cbs(0, 2, "bottom_left"); set_event_cbs(1, 2, "bottom"); set_event_cbs(2, 2, "bottom_right");

	evas_object_show(pattern);

	evas_object_show(win);

    elm_run();

    elm_shutdown();

	return(0);
}

ELM_MAIN()
