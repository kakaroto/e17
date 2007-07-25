#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

Evas_List * themes;

Evas_Object * container_outer;
Evas_Object * container;

Evas_Object * b_default,
	    * b_apply,
	    * b_exit;

Evas_Object * preview;
Evas_Object * preview_clip;
Evas_Object * background;

Evas_Object * last_button;

char * default_theme;
char * selected_theme;

typedef struct {
	Evas_Object * thumb;
	Evas_Object * button;
	char * theme;
	char * label;
	char * thumbpath;
} Etheme_Object;


static void _preview_theme(void *data, Evas_Object *obj, const char *emission, const char *source);

Etheme_Object * etheme_new(const char * t) {

	Etheme_Object * o;
	o = (Etheme_Object *) malloc(sizeof(Etheme_Object));

	o->theme = (char *) strdup(t);
	o->thumbpath = e_preview_thumb_file_get(t);


	int size = strlen(t) - 4; /* without .edj */

	o->label = malloc(sizeof(char) * size);
	o->label = strncpy(o->label, t, size);
	o->label[size] = 0;
		
	o->button = edje_object_add(evas);

	edje_object_file_set(o->button, default_theme, "fileman/icon_normal");
	evas_object_resize(o->button, 100, 120);
	edje_object_part_text_set(o->button, "icon_title" , o->label);
		
	esmart_container_element_append(container, o->button);
	
	if (e_preview_thumb_check(t))
		e_preview_thumb_generate(t);
		
	o->thumb = evas_object_image_add(evas);
	evas_object_image_size_set(o->thumb, 640, 480);
	evas_object_image_fill_set(o->thumb, 0, 0, 85, 85);
	e_preview_thumb_image(o->thumbpath, o->thumb);

	edje_object_part_swallow(o->button, "icon_swallow", o->thumb);

	evas_object_show(o->button);

	edje_object_signal_callback_add(o->button, "mouse,up,1", "icon_event",
				_preview_theme, o);
		
	
	return o;

}

void etheme_free(Etheme_Object * o) {

	if (!o) return;

	if (o->theme) free(o->theme);
	if (o->thumbpath) free(o->thumbpath);
	if (o->label) free(o->label);
	
	evas_object_del(o->thumb);
	evas_object_del(o->button);

	free(o);
}


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
		
	evas_object_resize(container_outer, barwidth, h - 100);

	int length = (int) esmart_container_elements_length_get(container);

	double size;
	size = (double)(h - 100) / (double)length;

	edje_object_part_drag_size_set(container_outer, "vbar_bar", 1.0, size);

	if (length > h - 100)
		edje_object_signal_emit(container_outer, "vbar", "show");
	else
		edje_object_signal_emit(container_outer, "vbar", "hide");

	evas_object_move(b_default, 0, h - 100 + 10);
	evas_object_move(b_apply, 0, h - 100 + 40);
	evas_object_move(b_exit, 0, h - 100 + 60);

	
}

static void _preview_theme(void *data, Evas_Object *obj, const char *emission, const char *source) {

	Etheme_Object * o;
	o = (Etheme_Object *) data;

	if (!o)
		selected_theme = (char *)strdup("default.edj");
	else
		selected_theme = o->theme;

	e_preview_theme_set(preview, selected_theme);

	if (last_button)
		edje_object_signal_emit(last_button, "unclicked", "");
	if (o) {
		edje_object_signal_emit(o->button, "clicked", "");
		last_button = o->button;
	}
	
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

static void _scrolldrag(void *data, Evas_Object *o, const char *emission, const char *source) { 
	        
	double y;
	edje_object_part_drag_value_get(container_outer, "vbar_bar", NULL,  &y);
	esmart_container_scroll_percent_set(container, y);

}

void get_theme_list() {

	char * path;
	path = malloc(sizeof(char) * PATH_MAX);
	snprintf(path, PATH_MAX, "%s/.e/e/themes", getenv("HOME"));
	
	Ecore_List *list;
	list = ecore_file_ls(path);
	
	ecore_list_first_goto(list);
	char * data;
	themes = NULL;
	while ((data = (char *)ecore_list_next(list)))
		if (strstr(data, ".edj") != NULL) {
			char * file = (char *) strdup(data);
			Etheme_Object * o;
			o = etheme_new(file);
			themes = evas_list_append(themes, o);
			free(file);
		}
	
	ecore_list_destroy(list);
	free(path);
	
}

void create_buttons() {

	
	container_outer = edje_object_add(evas);
	edje_object_file_set(container_outer, default_theme, "widgets/scrollframe");
	evas_object_move(container_outer, 0, 0);
	edje_object_signal_emit(container_outer, "hbar", "hide");
	evas_object_resize(container_outer, barwidth - 19, container_height);
	evas_object_show(container_outer);

	edje_object_signal_callback_add(container_outer, "drag*", "vbar_bar", _scrolldrag, NULL);
	
	container = esmart_container_new(evas);
	edje_object_part_swallow(container_outer, "item", container);
	esmart_container_direction_set(container, CONTAINER_DIRECTION_VERTICAL);
	
	get_theme_list();

#define ADD_BUTTON(A, B, C)\
	A = edje_object_add(evas);\
	edje_object_file_set(A, default_theme, "widgets/button");\
	evas_object_resize(A, 120, 24);\
	edje_object_part_text_set(A, "label", C);\
	evas_object_show(A);
	
	ADD_BUTTON(b_default, 40, "Default Theme");
	evas_object_move(b_default, 5, container_height + 10);
	edje_object_signal_callback_add(b_default, "click", "", _preview_theme, NULL);
	 
	ADD_BUTTON(b_apply, 60, "Apply");
	evas_object_move(b_apply, 5, container_height + 40);
	edje_object_signal_callback_add(b_apply, "click", "", _set_theme, NULL);
	
	ADD_BUTTON(b_exit, 80, "Exit");
	evas_object_move(b_exit, 5, container_height + 60);
	edje_object_signal_callback_add(b_exit, "click", "", _ethemes_exit, NULL);
	

#undef ADD_BUTTON
}

static int ethemes_init(void *data, int type, void *ev) {

	E_Response_Theme_Get *e;
	e = ev;
	
	default_theme = (char *) strdup(E17PREFIX "/share/enlightenment/data/themes/default.edj");

	edje_object_file_set(background, default_theme, "widgets/menu/default/background");

	selected_theme = (char *)strdup(e->file);
	e_preview_theme_set(preview, selected_theme);
	ecore_evas_title_set(ee, "eThemes");
	create_buttons();

	_resize_cb(ee);
	
	return 1;
}


int main(int argc, char **argv) {

	ecore_init();
	ecore_evas_init();
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

	e_preview_thumb_init();

	preview = e_preview_new(evas);
	
	preview_clip = evas_object_rectangle_add(evas);
	evas_object_move(preview_clip, barwidth, 0);
	evas_object_resize(preview_clip, WIDTH - barwidth, HEIGHT);
	evas_object_show(preview_clip);
	evas_object_clip_set (preview, preview_clip);

	last_button = NULL;

	ecore_evas_callback_resize_set(ee, _resize_cb);
	        
	ecore_event_handler_add(E_RESPONSE_THEME_GET, ethemes_init, NULL);
	e_lib_theme_get("theme");
			
	ecore_main_loop_begin();

	/* free ethemes */
	int n = evas_list_count(themes);
	for ( n -= 1; n>=0; n--) {
		Etheme_Object * o;
		o = evas_list_nth(themes, n);
		etheme_free(o);
	}
	evas_list_free(themes);

	/* free canvas objects */
	evas_object_del(container);
	evas_object_del(container_outer);
	evas_object_del(b_apply);
	evas_object_del(b_default);
	evas_object_del(b_exit);
	evas_object_del(background);
	evas_object_del(preview);
	evas_object_del(preview_clip);
	
	if (selected_theme) free(selected_theme);
	if (default_theme) free(default_theme);
	
	edje_shutdown();
	ecore_shutdown();

	return 0;
}


