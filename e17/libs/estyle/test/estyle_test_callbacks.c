#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Evas.h>
#include <Ecore.h>
#include "src/Estyle.h"
#include "estyle-config.h"

#define MAX_EVAS_COLORS (216)
#define MAX_FONT_CACHE (512 * 1024)
#define MAX_IMAGE_CACHE (1 * (1024 * 1024))
#define RENDER_ENGINE RENDER_METHOD_ALPHA_SOFTWARE

/* the obstacle's coords */
#define OBST_X 100.0
#define OBST_Y 100.0

char string1[] = "This text should test callbacks";

int obstacle_w = -1, obstacle_h = -1, obstacle_x, obstacle_y;

int layer = 1000;
int visible = 1;
int focused = 0;

static void e_idle(void *data);
static void ecore_window_expose(Ecore_Event * ev);
static void ecore_mouse_down(Ecore_Event * ev);
static void ecore_mouse_move(Ecore_Event * ev);
static void ecore_mouse_in(Ecore_Event * ev);
static void ecore_mouse_out(Ecore_Event * ev);

void setup(void);

Evas evas;
Evas_Render_Method render_method = RENDER_ENGINE;
Evas_Object cursor = NULL;
Estyle *e;

static void e_idle(void *data)
{
	evas_render(evas);
	return;
	data = NULL;
}

static void ecore_window_expose(Ecore_Event * ev)
{
	Ecore_Event_Window_Expose *e;

	e = (Ecore_Event_Window_Expose *) ev->event;
	evas_update_rect(evas, e->x, e->y, e->w, e->h);
}

static void ecore_mouse_in(Ecore_Event * ev)
{
	focused = 1;
}

static void ecore_mouse_out(Ecore_Event * ev)
{
	focused = 0;
}

static void ecore_mouse_down(Ecore_Event * ev)
{
	Ecore_Event_Mouse_Down *e;

	e = (Ecore_Event_Mouse_Down *) ev->event;
	if ((e->win != evas_get_window(evas)))
		return;
	evas_event_button_down(evas, e->x, e->y, e->button);
}

/*
 * Follow the mouse around the window
 */
static void ecore_mouse_move(Ecore_Event * ev)
{
	Ecore_Event_Mouse_Move *eemm =
	    (Ecore_Event_Mouse_Move *) ev->event;
	if (focused)
		estyle_move(e, eemm->x, eemm->y);
}

void mouse_move(void *_data, Estyle * _es, int _b, int _x, int _y)
{
	printf("Mouse movement detected\n");

	_es = NULL;
	_data = NULL;
	_b = 0;
	_x = 0;
	_y = 0;
}

void mouse_down(void *_data, Estyle * _es, int _b, int _x, int _y)
{
	printf("Clicked button %d at %d, %d\n", _b, _x, _y);
	evas_move(evas, cursor, _x, _y);

	if (_b == 2)
		estyle_callback_add(e, CALLBACK_MOUSE_MOVE, mouse_move,
				    NULL);
	if (_b == 3)
		estyle_callback_del(_es, CALLBACK_MOUSE_MOVE);

	_data = NULL;
}

void setup(void)
{
	Window win, ewin;

	ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_EXPOSE,
				       ecore_window_expose);
	ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_DOWN,
				       ecore_mouse_down);
	ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_MOVE,
				       ecore_mouse_move);
	ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_IN,
				       ecore_mouse_in);
	ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_OUT,
				       ecore_mouse_out);
	ecore_event_filter_idle_handler_add(e_idle, NULL);
	win = ecore_window_new(0, 0, 0, 400, 400);

	evas = evas_new_all(ecore_display_get(), win, 0, 0, 400, 400,
			    render_method, MAX_EVAS_COLORS, MAX_FONT_CACHE,
			    MAX_IMAGE_CACHE, PACKAGE_DATA_DIR "/fnt");

	ewin = evas_get_window(evas);

	ecore_window_show(ewin);
	ecore_window_set_events(ewin, XEV_EXPOSE | XEV_BUTTON | XEV_MOUSE_MOVE	/* |
										   XEV_IN_OUT */ );
	ecore_window_show(win);
}

int main(int argc, char *argv[])
{
	int i;
	int curs_x, curs_y, curs_w, curs_h;
	Evas_Object clip_rect;
	Evas_Object bg, et_bg, obst;

	obstacle_x = OBST_X;
	obstacle_y = OBST_Y;


	for (i = 1; i < argc; i++) {
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

	ecore_display_init(NULL);
	ecore_event_signal_init();
	ecore_event_filter_init();
	ecore_event_x_init();

	setup();

	/* add a background */
	bg = evas_add_rectangle(evas);
	evas_resize(evas, bg, 400, 400);
	evas_move(evas, bg, 0, 0);
	evas_set_layer(evas, bg, -10000);
	evas_set_color(evas, bg, 255, 255, 255, 255);
	evas_show(evas, bg);

	/* add an estyle-background */
	et_bg = evas_add_rectangle(evas);
	evas_resize(evas, et_bg, 380, 380);
	evas_move(evas, et_bg, 10, 10);
	evas_set_layer(evas, et_bg, -10000);
	evas_set_color(evas, et_bg, 0, 0, 255, 50);
	evas_show(evas, et_bg);

	/* draw obstacle-rect */
	obst = evas_add_rectangle(evas);
	evas_resize(evas, obst, obstacle_w, obstacle_h);
	evas_move(evas, obst, obstacle_x, obstacle_y);
	evas_set_color(evas, obst, 255, 0, 0, 50);
	evas_show(evas, obst);

	/*
	 * Create a clip rectangle for bounding where the text is drawn
	 */
	clip_rect = evas_add_rectangle(evas);
	evas_move(evas, clip_rect, 100, 100);
	evas_resize(evas, clip_rect, 200, 200);
	evas_show(evas, clip_rect);
	evas_set_color(evas, clip_rect, 255, 255, 255, 255);

	/*
	 * Create an estyle.
	 */
	e = estyle_new(evas, string1, "raised");
	estyle_move(e, 100, 100);
	estyle_set_color(e, 128, 255, 255, 255);
	estyle_set_clip(e, clip_rect);
	estyle_set_font(e, "nationff", 14);
	estyle_set_style(e, "shadow");
	estyle_show(e);
	estyle_text_at(e, 0, &curs_x, &curs_y, &curs_w, &curs_h);

	/* add a cursor */
	cursor = evas_add_rectangle(evas);
	evas_move(evas, cursor, curs_x, curs_y);
	evas_resize(evas, cursor, curs_w, curs_h);
	evas_set_layer(evas, cursor, 10000);
	evas_set_color(evas, cursor, 255, 255, 255, 128);
	evas_show(evas, cursor);

	/* Callbacks */
	estyle_callback_add(e, CALLBACK_MOUSE_DOWN, mouse_down, NULL);
	ecore_event_loop();

	estyle_free(e);
	evas_free(evas);

	return 0;
}
