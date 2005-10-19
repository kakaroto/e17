#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Esmart/Esmart_Container.h>
#include <Evas.h>
#include <Edje.h>
#include <E_Lib.h>

#include "E_Preview.h"

Ecore_Evas * ee;
Evas * evas;

const int barwidth = 130;
const int WIDTH = 700;
const int HEIGHT = 500;
const int container_height = 400;

char * selected_theme;
char * current_theme;

Evas_List * themes;

Evas_Object * label;

Evas_Object * container_outer;
Evas_Object * container;
Evas_Object * scrollbar;

Evas_Object ** buttons;
Evas_Object * preview;
Evas_Object * preview_clip;
Evas_Object * background;


void _resize_cb(Ecore_Evas * ee) {

	int w, h;
	evas_output_size_get(evas, &w, &h);

	evas_object_move(preview, barwidth, 0);
	evas_object_resize(preview, w - barwidth , h );
	evas_object_show(preview);

	evas_object_resize(preview_clip, w - barwidth , h );

	evas_object_move(background, 0, 0);
	evas_object_resize(background, barwidth, h);
	evas_object_show(background);

	int length = (int) esmart_container_elements_length_get(container);
	if (length > h - 100) {
		evas_object_resize(container_outer, barwidth - 19, h - 100);
		evas_object_show(scrollbar);
	} else {
		evas_object_resize(container_outer, barwidth, h - 100);
		evas_object_hide(scrollbar);
	}

	int n = evas_list_count(themes);
	evas_object_move(buttons[n], 0, h - 100 + 10);
	evas_object_move(buttons[n+1], 0, h - 100 + 40);
	evas_object_move(buttons[n+2], 0, h - 100 + 60);
	evas_object_resize(scrollbar, 16, h - 100);

	
}

static void _preview_theme(void *data, Evas_Object *o, const char *emission, const char *source) {

	selected_theme = (char *) data;

	e_preview_theme_set(preview, selected_theme);
	_resize_cb(ee);

}

static void _ethemes_exit(void *data, Evas_Object *o, const char *emission, const char *source) {

       ecore_main_loop_quit();

}

static void _set_theme(void *data, Evas_Object *o, const char *emission, const char *source) {

	e_lib_theme_set("theme", (char *)strdup(selected_theme));

	e_lib_restart();
	e_lib_shutdown();

	ecore_main_loop_quit();

}

static void _scrollup(void *data, Evas_Object *o, const char *emission, const char *source) { 
	esmart_container_scroll_start(container, 1);
}

static void _scrolldown(void *data, Evas_Object *o, const char *emission, const char *source) { 
	esmart_container_scroll_start(container, -1);
}

static void _scrolldrag(void *data, Evas_Object *o, const char *emission, const char *source) { 
	        
	double x, y;
	edje_object_part_drag_value_get(scrollbar, "drag", &x, &y);
	esmart_container_scroll_percent_set(container, y);

}

static void _scrollstop(void *data, Evas_Object *o, const char *emission, const char *source) { 
	esmart_container_scroll_stop(container);
	double s;
	s = esmart_container_scroll_percent_get(container);
	edje_object_part_drag_value_set(scrollbar, "drag", 1, s);

}

void read_theme_list() {

	char * path;
	path = malloc(sizeof(char) * PATH_MAX);
	snprintf(path, PATH_MAX, "%s/.e/e/themes", getenv("HOME"));
	
	Ecore_List *list;
	list = ecore_file_ls(path);
	
	ecore_list_goto_first(list);
	char * data;
	themes = NULL;
	while (data = (char *)ecore_list_next(list))
		if (strstr(data, ".edj") != NULL) {
			char * file = (char *) strdup(data);
			themes = evas_list_append(themes, file);
		}
	
	ecore_list_destroy(list);
	free(path);
	
}

void create_buttons() {

	int n = evas_list_count(themes);
	
	buttons = calloc(n + 3, sizeof(Evas_Object *));
	
	container_outer = edje_object_add(evas);
	edje_object_file_set(container_outer, current_theme, "modules/ibar/main");
	evas_object_move(container_outer, 0, 0);
	evas_object_resize(container_outer, barwidth - 19, container_height);
	evas_object_show(container_outer);

	scrollbar = edje_object_add(evas);
	edje_object_file_set(scrollbar, current_theme, "widgets/vscrollbar");
	evas_object_move(scrollbar, barwidth - 19, 0);
	evas_object_resize(scrollbar, 16, container_height);
	evas_object_show(scrollbar);

	edje_object_signal_callback_add(scrollbar, "scroll_top_start", "", _scrollup, NULL);
	edje_object_signal_callback_add(scrollbar, "scroll_bottom_start", "", _scrolldown, NULL);
	edje_object_signal_callback_add(scrollbar, "scroll_top_stop", "", _scrollstop, NULL);
	edje_object_signal_callback_add(scrollbar, "scroll_bottom_stop", "", _scrollstop, NULL);
	edje_object_signal_callback_add(scrollbar, "drag", "*", _scrolldrag, NULL);
	
	container = esmart_container_new(evas);
	edje_object_part_swallow(container_outer, "items", container);
	esmart_container_direction_set(container, CONTAINER_DIRECTION_VERTICAL);
	

#define BUTTON_EVENTS(A, B, C)\
	edje_object_signal_callback_add(buttons[A], "click", "", B, C);
	
#define ADD_BUTTON(A, B, C, D)\
	buttons[A] = edje_object_add(evas);\
	edje_object_file_set(buttons[A], current_theme, "widgets/button");\
	evas_object_resize(buttons[A], D, 24);\
	edje_object_part_text_set(buttons[A], "label", C);\
	evas_object_show(buttons[A]);
	
	int i;
	for (i=0; i<n; i++) {
			
		char * n;
		n = evas_list_nth(themes, i);
		char * name;
		int size = strlen(n) - 4; /* without .edj */
		name = malloc(sizeof(char) * size);
		name = strncpy(name, n, size);
		name[size] = 0;
		
		ADD_BUTTON(i, 20, name, 100);
		esmart_container_element_append(container, buttons[i]);
		BUTTON_EVENTS(i, _preview_theme, evas_list_nth(themes, i));
		
	}
	
	ADD_BUTTON(n, 40, "Default Theme", 120);
	evas_object_move(buttons[n], 5, container_height + 10);
	BUTTON_EVENTS(n, _preview_theme, "default.edj");
	 
	ADD_BUTTON(n + 1, 60, "Apply", 120);
	evas_object_move(buttons[n+1], 5, container_height + 40);
	BUTTON_EVENTS(n + 1, _set_theme, NULL);
	
	ADD_BUTTON(n + 2, 80, "Exit", 120);
	evas_object_move(buttons[n+2], 5, container_height + 60);
	BUTTON_EVENTS(n + 2, _ethemes_exit, NULL);
	

#undef BUTTON_EVENTS
#undef ADD_BUTTON
}

static int ethemes_init(void *data, int type, void *ev) {

	E_Response_Theme_Get *e;
	e = ev;
	
/* for now, I'll only use the default e17 theme cause it's the only one that 
 * is guaranteed to work.
 
	current_theme = malloc(sizeof(char) * PATH_MAX);
	snprintf(current_theme, PATH_MAX, "%s/.e/e/themes/%s", getenv("HOME"),
			e->file);
*/
	
	current_theme = E17PREFIX "/share/enlightenment/data/themes/default.edj";

	edje_object_file_set(background, current_theme, "widgets/menu/default/background");

	selected_theme = (char *)strdup(e->file);
	e_preview_theme_set(preview, selected_theme);
	ecore_evas_title_set(ee, "eThemes");
	create_buttons();

	_resize_cb(ee);
	
	return 1;
}


int main(int argc, char **argv) {

	ecore_init();
	edje_init();
	 
	char * display = getenv("DISPLAY");
	if (! display) 
		display = (char *)strdup(":0.0");
	
	if (!e_lib_init(display)) {
		fprintf(stderr, "Cannot connect to a running e17\n");
		edje_shutdown();
		ecore_shutdown();
		return 1;
	}       

	ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, WIDTH, HEIGHT);
	ecore_evas_title_set(ee, "eThemes (connecting...)");
	ecore_evas_name_class_set(ee, "eThemes", "eThemes");
	ecore_evas_show(ee);

	evas = ecore_evas_get(ee);

	background = edje_object_add(evas);
	evas_object_move(background, 0,0);
	evas_object_resize(background, barwidth, HEIGHT);
	evas_object_show(background);

	preview = e_preview_new(evas);
	
	preview_clip = evas_object_rectangle_add(evas);
	evas_object_move(preview_clip, barwidth, 0);
	evas_object_resize(preview_clip, WIDTH - barwidth, HEIGHT);
	evas_object_show(preview_clip);
	evas_object_clip_set (preview, preview_clip);
	
	read_theme_list();

	ecore_evas_callback_resize_set(ee, _resize_cb);
	        
	ecore_event_handler_add(E_RESPONSE_THEME_GET, ethemes_init, NULL);
	e_lib_theme_get("theme");
			
	ecore_main_loop_begin();


	edje_shutdown();
	ecore_shutdown();

	return 0;
}



