#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Evas_Engine_Software_X11.h>
#include "src/Estyle.h"
#include "estyle-config.h"

#define MAX_EVAS_COLORS (216)
#define MAX_FONT_CACHE (512 * 1024)
#define MAX_IMAGE_CACHE (1 * (1024 * 1024))
#define RENDER_ENGINE RENDER_METHOD_ALPHA_SOFTWARE

/* the obstacle's coords */
#define OBST_X 100.0
#define OBST_Y 100.0

char string1[] = "This text should\ntest the basic styles ";

char string2[] = "This is the alternate text to test ";
char *last = string1;

int obstacle_w = -1, obstacle_h = -1, obstacle_x, obstacle_y;

int layer = 1000;
int visible = 1;
int focused = 0;

static int e_idle(void *data);
static int ecore_window_expose(void *data, int type, void * ev);
static int ecore_mouse_down(void *data, int type, void * ev);
static int ecore_mouse_move(void *data, int type, void * ev);
static int ecore_mouse_in(void *data, int type, void * ev);
static int ecore_mouse_out(void *data, int type, void * ev);

void setup(void);

Evas *evas;
#if 0
int render_method = RENDER_ENGINE;
#endif
Evas_Object *cursor = NULL;
Evas_Object *e;

static int e_idle(void *data)
{
	evas_render(evas);
	return 1;
	data = NULL;
}

static int ecore_window_expose(void *data, int type, void * ev)
{
	Ecore_X_Event_Window_Damage *e;

	e = (Ecore_X_Event_Window_Damage *) ev;
	evas_damage_rectangle_add(evas, e->x, e->y, e->w, e->h);

	return 1;
	data = NULL;
	type = 0;
	ev = NULL;
}

static int ecore_mouse_in(void *data, int type, void * ev)
{
	focused = 1;

	return 1;
	data = NULL;
	type = 0;
	ev = NULL;
}

static int ecore_mouse_out(void *data, int type, void * ev)
{
	focused = 0;

	return 1;
	data = NULL;
	type = 0;
	ev = NULL;
}

static int ecore_mouse_down(void *data, int type, void * ev)
{
	int index;
	Evas_Coord x, y, w, h;

	Ecore_X_Event_Mouse_Button_Down *eemd =
	    (Ecore_X_Event_Mouse_Button_Down *) ev;

	if (eemd->button == 1) {
		if (last == string1) {
			estyle_set_style(e, "raised");
			estyle_set_font(e, "morpheus", 14);
			estyle_set_text(e, string2);
			last = string2;
		} else {
			estyle_set_font(e, "nationff", 14);
			estyle_set_style(e, "shadow");
			estyle_set_text(e, string1);
			last = string1;
		}
		index =
		    estyle_text_at_position(e, eemd->x, eemd->y, &x, &y,
					    &w, &h);
		if (index < 0)
			printf("Click occurred outside of estyle\n");
		else {
			printf("Clicked letter %c at %g, %g size %g x %g\n",
				last[index], x, y, w, h);
			evas_object_move(cursor, x, y);
			evas_object_resize(cursor, w, h);
		}

	} else if (eemd->button == 2) {
		layer = -layer;
		estyle_set_layer(e, layer);
	} else {
		if (visible) {
			estyle_hide(e);
			visible = 0;
		} else {
			estyle_show(e);
			visible = 1;
		}
	}

	return 1;
	data = NULL;
	type = 0;
	ev = NULL;
}

/*
 * Follow the mouse around the window
 */
static int ecore_mouse_move(void *data, int type, void * ev)
{
	return 1;
	data = NULL;
	type = 0;
	ev = NULL;
}



void setup(void)
{
	Window win, ewin;
	Display *disp = ecore_x_display_get();

	ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DAMAGE,
			ecore_window_expose, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_DOWN,
			ecore_mouse_down, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, ecore_mouse_move,
			NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_IN, ecore_mouse_in, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_OUT, ecore_mouse_out, NULL);
	ecore_idle_enterer_add(e_idle, NULL);

#if 0
	evas = evas_new_all(ecore_display_get(), win, 0, 0, 400, 400,
			    render_method, MAX_EVAS_COLORS, MAX_FONT_CACHE,
			    MAX_IMAGE_CACHE, PACKAGE_DATA_DIR "/fnt");
#else
	evas=evas_new();
#endif

	ewin = ecore_x_window_new(0,0,0,400,400); /* evas_get_window(evas); */

    evas_output_method_set(evas, evas_render_method_lookup("software_x11"));
    evas_output_size_set(evas, 400, 400);
    evas_output_viewport_set(evas, 0, 0, 400, 400);

    {
	Evas_Engine_Info_Software_X11 *einfo;
	
	einfo = (Evas_Engine_Info_Software_X11 *) evas_engine_info_get(evas);

	einfo->info.display = disp;
	einfo->info.visual = DefaultVisual(disp, DefaultScreen(disp));
	einfo->info.colormap = DefaultColormap(disp, DefaultScreen(disp));
	einfo->info.drawable = ewin;
	einfo->info.depth = DefaultDepth(disp, DefaultScreen(disp));
	einfo->info.rotation = 0;
	einfo->info.debug = 0;
	evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);
    }

	ecore_x_window_show(ewin);
	/* ecore_window_set_events(ewin, XEV_EXPOSE | XEV_BUTTON |
		 XEV_MOUSE_MOVE	| XEV_IN_OUT); */ 
	ecore_x_window_show(win);
}

int main(int argc, char *argv[])
{
	int i, w, h;
	Evas_Coord curs_x, curs_y, curs_w, curs_h;
	Evas_Object *clip_rect;
	Evas_Object *img_test;
	Evas_Object *bg, *et_bg, *obst;

	obstacle_x = OBST_X;
	obstacle_y = OBST_Y;


	for (i = 1; i < argc; i++) {
#if 0
		if (!strcmp(argv[i], "soft") ||
		    !strcmp(argv[i], "x11") || !strcmp(argv[i], "hard")) {
			if (!strcmp(argv[i], "soft"))
				render_method =
				    RENDER_METHOD_ALPHA_SOFTWARE;
			if (!strcmp(argv[i], "x11"))
				render_method =
				    RENDER_METHOD_BASIC_HARDWARE;
			if (!strcmp(argv[i], "hard"))
				render_method = RENDER_METHOD_3D_HARDWARE;
		} else {
#else
		  {
#endif
			if (obstacle_w < 0.0)
				obstacle_w = atoi(argv[i]);
			else if (obstacle_h < 0.0)
				obstacle_h = atoi(argv[i]);
		}
	}

	if (obstacle_w < 0.0)
		obstacle_w = 100.0;
	if (obstacle_h < 0.0)
		obstacle_h = 100.0;

	ecore_init();

	ecore_x_init(NULL);

	setup();

	/* add a background */
	bg = evas_object_rectangle_add(evas);
	evas_object_resize(bg, 400, 400);
	evas_object_move(bg, 0, 0);
	evas_object_layer_set(bg, -10000);
	evas_object_color_set(bg, 255, 255, 255, 255);
	evas_object_show(bg);

	/* add an estyle-background */
	et_bg = evas_object_rectangle_add(evas);
	evas_object_resize(et_bg, 380, 380);
	evas_object_move(et_bg, 10, 10);
	evas_object_layer_set(et_bg, -10000);
	evas_object_color_set(et_bg, 0, 0, 255, 50);
	evas_object_show(et_bg);

	/* draw obstacle-rect */
	obst = evas_object_rectangle_add(evas);
	evas_object_resize(obst, obstacle_w, obstacle_h);
	evas_object_move(obst, obstacle_x, obstacle_y);
	evas_object_color_set(obst, 255, 0, 0, 50);
	evas_object_show(obst);

	img_test = evas_object_image_add(evas);
	evas_object_image_file_set(img_test, "/usr/share/pixmaps/dia-diagram.png", NULL);
	evas_object_image_size_get(img_test, &w, &h);
	printf("Image of size %d x %d\n", w, h);
	evas_object_move(img_test, 10, 10);
	evas_object_image_fill_set(img_test, 0, 0, w, h);
	evas_object_resize(img_test, 5 * w, 5 * h);
	evas_object_layer_set(img_test, 10000);
	evas_object_show(img_test);

	/*
	 * Create a clip rectangle for bounding where the text is drawn
	 */
	clip_rect = evas_object_rectangle_add(evas);
	evas_object_move(clip_rect, 100, 100);
	evas_object_resize(clip_rect, 100, 100);
	evas_object_show(clip_rect);
	evas_object_color_set(clip_rect, 255, 255, 255, 255);

	/*
	 * Create an estyle.
	 */
	e = estyle_new(evas, string1, "soft_shadow");
	evas_object_move(e, 100, 100);
	evas_object_color_set(e, 128, 255, 255, 255);
	evas_object_clip_set(e, clip_rect);
	evas_object_show(e);
	estyle_text_at(e, 0, &curs_x, &curs_y, &curs_w, &curs_h);

	printf("size: %d\n",estyle_get_font_size(e));
	printf("name: %s\n",estyle_get_font(e));

	estyle_set_font(e,"andover",32);

	/* add a cursor */
	cursor = evas_object_rectangle_add(evas);
	evas_object_move(cursor, curs_x, curs_y);
	evas_object_resize(cursor, curs_w, curs_h);
	evas_object_layer_set(cursor, 10000);
	evas_object_color_set(cursor, 255, 255, 255, 128);
	evas_object_show(cursor);

	ecore_main_loop_begin();

	estyle_free(e);
	evas_free(evas);

	return 0;
}
