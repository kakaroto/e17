#include "Etox_test.h"

/* globals */
Evas_Object *clip_test;
Evas_Object *o_next_box;
Evas_Object *o_txt_next_box;
Evas_Object *o_prev_box;
Evas_Object *o_txt_prev_box;
Eina_List *pbuttons;
Evas_Object *o_bg;
Evas_Object *o_bg_etox;

Ecore_Evas *ee;
Evas *evas;
int render_method = 0;

Evas_Object *e_msg;
Evas_Object *e_test;

Eina_List *current_test = NULL;

int win_w = W, win_h = H;
int win_x = 0, win_y = 0;

double get_time(void)
{
	struct timeval timev;

	gettimeofday(&timev, NULL);
	return (double) timev.tv_sec +
	    (((double) timev.tv_usec) / 1000000);
}

void window_resize(Ecore_Evas *ee)
{
	Evas_Coord pw;
	int ex, ey, ew, eh;

	evas_output_size_get(evas, &win_w, &win_h);
	evas_object_resize(o_bg, win_w, win_h);
	evas_object_image_fill_set(o_bg, 0, 0, win_w, win_h);

	evas_object_geometry_get(o_panel, NULL, NULL, &pw, NULL);
	evas_object_resize(o_panel, pw, win_h);
	evas_object_image_fill_set(o_panel, 0, 0, pw, win_h);

	ey = win_h / 2;
	ew = win_w - pw - 20;
	eh = ey - 10;
	ex = pw + 10;
	ey += 10;

	evas_object_move(e_msg, ex, 10);
	evas_object_resize(e_msg, ew, eh);

	evas_object_move(o_bg_etox, ex, ey);
	evas_object_resize(o_bg_etox, ew, eh);

	evas_object_move(clip_test, ex, ey);
	evas_object_resize(clip_test, ew, eh);

	evas_object_move(e_test, ex, ey);
	evas_object_resize(e_test, ew, eh);

	return;
	ee = NULL;
}

int sig_exit(void *data, int type, void * ev)
{
	ecore_main_loop_quit();
	return 1;
	data = NULL;
	type = 0;
	ev = NULL;
}

int e_mouse_move(void *data, int type, void * ev)
{
	Ecore_X_Event_Mouse_Move *e;
	Evas_Engine_Info_Software_X11 *info;

	e = (Ecore_X_Event_Mouse_Move *) ev;
	info = (Evas_Engine_Info_Software_X11 *)evas_engine_info_get(evas);
	if (e->win != info->info.drawable)
		return 1;
	evas_event_feed_mouse_move(evas, e->x, e->y, ecore_x_current_time_get(), NULL);
	return 1;
	data = NULL;
	type = 0;
}

int e_mouse_down(void *data, int type, void * ev)
{
	Ecore_X_Event_Mouse_Button_Down *e;
	Evas_Engine_Info_Software_X11 *info;

	e = (Ecore_X_Event_Mouse_Button_Down *) ev;
	info = (Evas_Engine_Info_Software_X11 *)evas_engine_info_get(evas);
	if (e->win != info->info.drawable)
		return 1;
	evas_event_feed_mouse_down(evas, e->button, EVAS_BUTTON_NONE, ecore_x_current_time_get(), NULL);
	return 1;
	data = NULL;
	type = 0;
}

int e_mouse_up(void *data, int type, void * ev)
{
	Ecore_X_Event_Mouse_Button_Up *e;
	Evas_Engine_Info_Software_X11 *info;

	e = (Ecore_X_Event_Mouse_Button_Up *) ev;
	info = (Evas_Engine_Info_Software_X11 *)evas_engine_info_get(evas);
	if (e->win != info->info.drawable)
		return 1;
	evas_event_feed_mouse_up(evas, e->button, EVAS_BUTTON_NONE, ecore_x_current_time_get(), NULL);
	return 1;
	data = NULL;
	type = 0;
}

void
button_mouse_in(void *_data, Evas *_e, Evas_Object *_o,
		void *event_info)
{
	if ((_e = evas))
		evas_object_color_set(_o, 0, 0, 0, 255);

	return;
	_data = NULL;
	_e = NULL;
	_o = NULL;
	event_info = NULL;
}

void
button_mouse_out(void *_data, Evas *_e, Evas_Object *_o,
		void *event_info)
{
	if ((_e = evas))
		evas_object_color_set(_o, 0, 0, 0, 160);

	return;
	_data = NULL;
	_e = NULL;
	_o = NULL;
	event_info = NULL;
}

void
button_mouse_down(void *_data, Evas *_e, Evas_Object *_o,
		void *event_info)
{
	Eina_List *l = _data;
	void (*run) ();

	etox_clear(e_msg);
	etox_clear(e_test);

	if (l) {
		evas_object_hide(o_prev_box);
		evas_object_hide(o_txt_prev_box);

		if (l->next) {
			evas_object_show(o_next_box);
			evas_object_show(o_txt_next_box);
		} else {
			evas_object_hide(o_next_box);
			evas_object_hide(o_txt_next_box);
		}

		current_test = l;
		run = l->data;
		run();
	}

	return;
	_data = NULL;
	_e = NULL;
	_o = NULL;
	event_info = NULL;
}

void
next_test(void *_data, Evas *_e, Evas_Object *_o,
		void *event_info)
{
	void (*run) ();

	if (!current_test || !current_test->next)
		return;

	/*
	 * Step to the next test, hide the "Next" button if appropriate, and
	 * run the current test.
	 */
	current_test = current_test->next;
	if (!current_test->next) {
		evas_object_hide(o_next_box);
		evas_object_hide(o_txt_next_box);
	}

	evas_object_show(o_prev_box);
	evas_object_show(o_txt_prev_box);

	evas_object_event_callback_add(o_txt_next_box, EVAS_CALLBACK_MOUSE_IN,
			  button_mouse_in, NULL);
	evas_object_event_callback_add(o_txt_next_box, EVAS_CALLBACK_MOUSE_OUT,
			  button_mouse_out, NULL);

	run = current_test->data;
	run();

	return;
	_data = NULL;
	_e = NULL;
	_o = NULL;
	event_info = NULL;
}

void
prev_test(void *_data, Evas *_e, Evas_Object *_o,
		void *event_info)
{
	void (*run) ();

	if (!current_test || !current_test->prev)
		return;

	/*
	 * Step to the next test, hide the "Next" button if appropriate, and
	 * run the current test.
	 */
	current_test = current_test->prev;
	if (!current_test->prev) {
		evas_object_hide(o_prev_box);
		evas_object_hide(o_txt_prev_box);
	}

	evas_object_show(o_next_box);
	evas_object_show(o_txt_next_box);

	evas_object_event_callback_add(o_txt_prev_box, EVAS_CALLBACK_MOUSE_IN,
			  button_mouse_in, NULL);
	evas_object_event_callback_add(o_txt_prev_box, EVAS_CALLBACK_MOUSE_OUT,
			  button_mouse_out, NULL);

	run = current_test->data;
	run();

	return;
	_data = NULL;
	_e = NULL;
	_o = NULL;
	event_info = NULL;
}

void setup(void)
{
	Evas_Coord pw;
	int ex, ey, ew, eh;
	Evas_Coord width, height;
	char msg[] =
	    "The Etox Test utility consists in a series "
	    "of test suites designed to exercise all of "
	    "the etox functions.\n"
	    "Informational messages will be displayed here, "
	    "the test text will be presented in the colored "
	    "rectangle below.\n"
	    "To start a test suite, select it from the "
	    "navigation panel on the left.\n";

	/* Create interface */

	/* Background */
	o_bg = evas_object_image_add(evas);
	evas_object_image_file_set(o_bg, IM "bg.png", IM "bg.png");
	evas_object_resize(o_bg, win_w, win_h);
	evas_object_image_fill_set(o_bg, 0, 0, win_w, win_h);
	evas_object_move(o_bg, 0, 0);
	evas_object_layer_set(o_bg, 0);
	evas_object_show(o_bg);

	/* Panel */
	setup_panel(evas);

	evas_object_geometry_get(o_panel, NULL, NULL, &pw, NULL);

	ey = win_h / 2;
	ew = win_w - pw - 20;
	eh = ey - 10;
	ex = pw + 10;
	ey += 10;

	/* Create message etox */
	e_msg = etox_new_all(evas, ex, 10, ew, eh , 255, ETOX_ALIGN_LEFT);
	etox_context_set_align(etox_get_context(e_msg), ETOX_ALIGN_LEFT);
	etox_context_set_font(etox_get_context(e_msg), "sinon", 14);
	etox_context_set_style(etox_get_context(e_msg), "shadow");
	etox_context_set_color(etox_get_context(e_msg), 225, 225, 225, 255);
	etox_set_soft_wrap(e_msg, 1);
	etox_set_text(e_msg, msg);
	etox_set_alpha(e_msg, 255);
	evas_object_layer_set(e_msg, 1000);
	evas_object_show(e_msg);

	/* Setup test etox */
	/* Setup test etox background */
	o_bg_etox = evas_object_rectangle_add(evas);
	evas_object_move(o_bg_etox, ex, ey);
	evas_object_resize(o_bg_etox, ew, eh);
	evas_object_color_set(o_bg_etox, 0, 100, 100, 100);
	evas_object_layer_set(o_bg_etox, 100);
	evas_object_show(o_bg_etox);

	/* Clip rectangle for bounding where the test text is drawn */
	clip_test = evas_object_rectangle_add(evas);
	evas_object_move(clip_test, ex, ey);
	evas_object_resize(clip_test, ew, eh);
	evas_object_show(clip_test);

	/* Create test etox */
	e_test = etox_new_all(evas, ex, ey, ew, eh, 255, ETOX_ALIGN_CENTER);
	etox_context_set_font(etox_get_context(e_test), "sinon", 14);
	etox_context_set_style(etox_get_context(e_test), "plain");
	etox_context_set_color(etox_get_context(e_test), 225, 225, 225, 255);
	etox_set_text(e_test, "");
	evas_object_clip_set(e_test, clip_test);
	etox_set_alpha(e_test, 255);
	evas_object_layer_set(e_test, 1000);

	/*
	 * Create the next test button
	 */
	o_next_box = evas_object_image_add(evas);
	evas_object_image_file_set(o_next_box,
			IM "panel_button1.png",
			IM "panel_button1.png");
	evas_object_image_border_set(o_next_box, 3, 3, 3, 3);
	evas_object_layer_set(o_next_box, 1000);
	evas_object_hide(o_next_box);

	o_txt_next_box = evas_object_text_add(evas);
	evas_object_text_font_set(o_txt_next_box, "andover", 24);
       	evas_object_text_text_set(o_txt_next_box, "Next");
	evas_object_color_set(o_txt_next_box, 0, 0, 0, 160);
	evas_object_layer_set(o_txt_next_box, 1000);
	evas_object_move(o_next_box, 516, 150);
	evas_object_move(o_txt_next_box, 519, 152);

	width = evas_object_text_horiz_advance_get(o_txt_next_box);
	height = evas_object_text_vert_advance_get(o_txt_next_box);

	width += 10;
	height += 5;

	evas_object_resize(o_next_box, width, height);
	evas_object_image_fill_set(o_next_box, 0, 0, width, height);
	evas_object_hide(o_txt_next_box);
	evas_object_event_callback_add(o_txt_next_box, EVAS_CALLBACK_MOUSE_DOWN,
			  next_test, NULL);

	/*
	 * Create the previous test button
	 */
	o_prev_box = evas_object_image_add(evas);
	evas_object_image_file_set(o_prev_box,
			IM "panel_button1.png",
			IM "panel_button1.png");
	evas_object_image_border_set(o_prev_box, 3, 3, 3, 3);
	evas_object_layer_set(o_prev_box, 1000);
	evas_object_hide(o_prev_box);

	o_txt_prev_box = evas_object_text_add(evas);
	evas_object_text_font_set(o_txt_prev_box, "andover", 24);
	evas_object_text_text_set(o_txt_prev_box, "Previous");
	evas_object_color_set(o_txt_prev_box, 0, 0, 0, 160);
	evas_object_layer_set(o_txt_prev_box, 1000);
	evas_object_move(o_prev_box, 416, 150);
	evas_object_move(o_txt_prev_box, 419, 152);

	width = evas_object_text_horiz_advance_get(o_txt_prev_box);
	height = evas_object_text_vert_advance_get(o_txt_prev_box);

	width += 10;
	height += 5;

	evas_object_resize(o_prev_box, width, height);
	evas_object_image_fill_set(o_prev_box, 0, 0, width, height);
	evas_object_hide(o_txt_prev_box);
	evas_object_event_callback_add(o_txt_prev_box, EVAS_CALLBACK_MOUSE_DOWN,
			  prev_test, NULL);
}

int main(int argc, const char **argv)
{
	Ecore_X_Window win;

	ecore_init();
	ecore_app_args_set(argc, argv);

	if (!ecore_evas_init())
		return -1;

	ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, win_w, win_h);
	if (!ee)
		return 1;
	win = ecore_evas_software_x11_window_get(ee);
	ecore_evas_title_set(ee, "Etox Selection Test");
	ecore_evas_show(ee);

	/* setup callbacks for events */
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE, e_mouse_move, NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_DOWN, e_mouse_down,
			NULL);
	ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP, e_mouse_up,
			NULL);
	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, sig_exit, NULL);

	evas = ecore_evas_get(ee);
	evas_font_path_append(evas, FONT_DIRECTORY);

	/* program does its data setup here */
	setup();

	ecore_evas_callback_resize_set(ee, window_resize);

	/* and now loop forever handling events */
	ecore_main_loop_begin();

	evas_object_del(e_msg);
	evas_object_del(e_test);

	while (pbuttons) {
		panel_button_free(pbuttons->data);
		pbuttons = eina_list_remove(pbuttons, pbuttons->data);
	}

	evas_free(evas);

	return 0;
	argc = 0;
	argv = NULL;
}
