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

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "Elementary.h"

#include "e_pattern_lock.h"

void pat_mouse_down_1(void *data, Evas_Object *obj, const char *emission, const char *source) {
	printf(_("Mouse down\n"));
}

#define mouse_down(foo) edje_object_signal_callback_add(edje, "mouse,down,1", foo, pat_mouse_down_1, NULL)

EAPI int elm_main(int argc, char **argv) {
	Evas_Object *win=NULL, *bg=NULL, *pattern=NULL, *edje=NULL;
	char theme[PATH_MAX];

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	win = elm_win_add(NULL, "e_pattern_lock", ELM_WIN_BASIC);
		elm_win_autodel_set(win, EINA_TRUE);
		evas_object_size_hint_min_set(win, 480, 480);
		evas_object_size_hint_max_set(win, 640, 640);

	bg = elm_bg_add(win);
		evas_object_size_hint_weight_set(bg, 1.0, 1.0);
		elm_win_resize_object_add(win, bg);
	evas_object_show(bg);

	snprintf(theme, sizeof(theme), "%s/themes/default.edj", PKGDATADIR);
    elm_theme_extension_add(NULL, theme);

	pattern = elm_layout_add(win);
		elm_layout_file_set(pattern, theme, "e_lock/table/default");
		evas_object_size_hint_weight_set(pattern, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_win_resize_object_add(win, pattern);
	evas_object_show(pattern);

	edje = elm_layout_edje_get(pattern);
	mouse_down("top_left"); mouse_down("top"); mouse_down("top_right");
	mouse_down("left"); mouse_down("center"); mouse_down("right");
	mouse_down("bottom_left"); mouse_down("bottom"); mouse_down("bottom_right");


	evas_object_show(win);

    elm_run();

    elm_shutdown();

	return(0);
}

ELM_MAIN()
