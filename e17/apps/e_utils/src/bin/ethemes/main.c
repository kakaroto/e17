#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Evas.h>
#include <Edje.h>
#include <E_Lib.h>

#include "E_Preview.h"

Ecore_Evas * ee;
Evas * evas;

char * selected_theme;
char * current_theme;

Evas_List * themes;

Evas_Object ** b_labels;
Evas_Object ** buttons;
Evas_Object * preview;
Evas_Object * preview_clip;
Evas_Object * background;


void _resize_cb(Ecore_Evas * ee) {

	int w, h;
	evas_output_size_get(evas, &w, &h);

	evas_object_move(preview, 120, 0);
	evas_object_resize(preview, w - 120 , h );
	evas_object_show(preview);

	evas_object_resize(preview_clip, w - 120 , h );

	evas_object_move(background, 0, 0);
	evas_object_resize(background, 120, h);
	evas_object_show(background);

}

static void _preview_theme(void *data, Evas *e, Evas_Object *o, void *ev) {

	selected_theme = (char *) data;

	e_preview_theme_set(preview, selected_theme);
	_resize_cb(ee);

}

static void __exit(void *data, Evas * e, Evas_Object * obj, void *ev) {

       ecore_main_loop_quit();

}

static void _set_theme(void *data, Evas * e, Evas_Object * obj, void *ev) {

	e_lib_theme_set("theme", (char *)strdup(selected_theme));

	e_lib_restart();
	e_lib_shutdown();

	ecore_main_loop_quit();

}

static void _rollover(void *d, Evas *e, Evas_Object *o, void *ev) {

	Evas_Object *b;
	b = (Evas_Object *) d;
	edje_object_signal_emit(b, "active", "");

}

static void _rollout(void *d, Evas *e, Evas_Object *o, void *ev) {

	Evas_Object *b;
	b = (Evas_Object *) d;
	edje_object_signal_emit(b, "passive", "");

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
	
	b_labels = calloc(n + 3, sizeof(Evas_Object *));
	buttons = calloc(n + 3, sizeof(Evas_Object *));
	

#define BUTTON_EVENTS(A, B, C) \
	evas_object_event_callback_add(buttons[A], EVAS_CALLBACK_MOUSE_IN, _rollover, b_labels[A]);\
	evas_object_event_callback_add(buttons[A], EVAS_CALLBACK_MOUSE_OUT, _rollout, b_labels[A]);\
	evas_object_event_callback_add(buttons[A], EVAS_CALLBACK_MOUSE_UP, B, C);

#define ADD_BUTTON(A, B, C)\
	b_labels[A] = edje_object_add(evas);\
	edje_object_file_set(b_labels[A], current_theme, "widgets/menu/default/label");\
	evas_object_move(b_labels[A], 10, B + (25 * A));\
	evas_object_resize(b_labels[A], 100, 24);\
	edje_object_part_text_set(b_labels[A], "label", C);\
	evas_object_show(b_labels[A]);\
	buttons[A] = evas_object_rectangle_add(evas);\
	evas_object_resize(buttons[A], 100, 24);\
	evas_object_move(buttons[A], 10, B + (25 * A));\
	evas_object_color_set(buttons[A], 0, 0, 0, 0);\
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
		
		ADD_BUTTON(i, 20, name);
		BUTTON_EVENTS(i, _preview_theme, evas_list_nth(themes, i));
		
	}
	
	ADD_BUTTON(n, 40, "Default Theme");
	BUTTON_EVENTS(n, _preview_theme, "default.edj");
	 
	ADD_BUTTON(n + 1, 60, "Apply");
	BUTTON_EVENTS(n + 1, _set_theme, NULL);
	
	ADD_BUTTON(n + 2, 80, "Exit");
	BUTTON_EVENTS(n + 2, __exit, NULL);
	

#undef BUTTON_EVENT
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
	ecore_evas_name_class_set(ee, "eThemes", "eThemes");
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

	ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 700, 500);
	ecore_evas_title_set(ee, "eThemes (connecting...)");
	ecore_evas_show(ee);

	evas = ecore_evas_get(ee);

	background = edje_object_add(evas);
	evas_object_move(background, 0,0);
	evas_object_resize(background, 120, 500);
	evas_object_show(background);

	preview = e_preview_new(evas);
	
	preview_clip = evas_object_rectangle_add(evas);
	evas_object_move(preview_clip, 120, 0);
	evas_object_resize(preview_clip, 680, 500);
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



