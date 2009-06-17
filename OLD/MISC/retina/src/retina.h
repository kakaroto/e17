/* retina headers */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Evas.h>

#include <Imlib2.h>

#include "../config.h"

/* global variables */
Ecore_Evas *retina_win;
Evas *retina_evas;
Evas_Object *retina_bg, *retina_img, *retina_catch;
Evas_Object *retina_diag_bg, *retina_help_img;
Evas_Object *retina_thumb_bg;

int help_state, diag_state;
int img_count, cur_sel; 
int start_time;
char *retina_image_list[500];

struct retina_image_t {
	char *filename;
	int orig_w;
	int orig_h;
	int zoom_w;
	int zoom_h;
} retina_image, *retina_image_p;

/* global functions */
/* r_gui.c */
int  retina_gui_create();
void retina_gui_set_title(char *title);
void retina_gui_render();

/* r_img.c */
int  retina_img_load(char *img);
void retina_img_load_next();
void retina_img_load_back();
void retina_img_drawchecks();
void retina_img_resize(int w, int h);
void retina_img_zoom_in();
void retina_img_zoom_out();
void retina_img_zoom_set();
void retina_img_show_help();
void retina_img_show_image_info();
void retina_img_diag_render();
int retina_img_fade_in(void *data);

/* r_cb.c */
void retina_cb_attach();
void retina_cb_delete_event();
void retina_cb_win_resize();
void retina_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
void retina_cb_key_down(void *data, Evas *e, Evas_Object*obj, void *event_info);

/* r_thumb.c */
void retina_thumbnail_browser_create();
void retina_thumbnail_browser_show();
void retina_thumbnail_generate(char *file);
void retina_thumbnail_draw(char *file);
