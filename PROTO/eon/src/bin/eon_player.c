#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdarg.h>

#include "Eon.h"

/*
 * The player is an application that parses external files with eon scenes
 *
 * TODO make eon type system to be defined at eon_init() to avoid strcmp => eon_foo
 * attributes of type VALUE are relative to another attribute, how to handle it?
 *
 */
char *engine = "sdl";
char *image_dir = "./";

void help(void)
{
	printf("eon_player [OPTIONS] FILE\n");
	printf("-h This help\n");
	printf("-e Engine\n");
	printf("-W Width\n");
	printf("-H Height\n");
	printf("-I Image directory\n");
	printf("FILE Eon XML file\n");
}

static void _pause_click_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Document *doc;
	Eon_Canvas *c = data;

	doc = eon_canvas_document_get(c);
	eon_document_pause(doc);
}

static void _play_click_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Eon_Document *doc;
	Eon_Canvas *c = data;

	doc = eon_canvas_document_get(c);
	eon_document_play(doc);
}

void ui_setup(Eon_Canvas *c)
{
	Eon_Rect *r;

	r = eon_rect_new(c);
	eon_rect_x_set(r, 0);
	eon_rect_y_set(r, 0);
	eon_rect_w_rel_set(r, 100);
	eon_rect_h_rel_set(r, 5);
	eon_rect_color_set(r, 0xff000000);
	eon_rect_show(r);
	ekeko_event_listener_add((Ekeko_Object *)r, EKEKO_EVENT_UI_MOUSE_DOWN, _pause_click_cb, EINA_FALSE, c);

	r = eon_rect_new(c);
	eon_rect_x_set(r, 0);
	eon_rect_y_rel_set(r, 95);
	eon_rect_w_rel_set(r, 100);
	eon_rect_h_rel_set(r, 5);
	eon_rect_color_set(r, 0xff000000);
	eon_rect_show(r);
	ekeko_event_listener_add((Ekeko_Object *)r, EKEKO_EVENT_UI_MOUSE_DOWN, _play_click_cb, EINA_FALSE, c);
}

int main(int argc, char **argv)
{
	Eon_Document *doc;
	Eon_External *ext;
	Eon_Canvas *canvas;
	int w = 320;
	int h = 240;

	char *short_options = "I:e:hW:H:";
	struct option long_options[] = {
		{"image", 1, 0, 'I'},
		{"engine", 1, 0, 'e'},
		{"help", 0, 0, 'h'},
		{"width", 1, 0, 'W'},
		{"height", 1, 0, 'H'},
		{0, 0, 0, 0}
	};
	int option;
	char c;
	char *file = argv[argc - 1];

	while ((c = getopt_long(argc, argv, short_options, long_options,
			&option)) != -1)
	{
		switch (c)
		{
			case 'h':
			help();
			return 0;

			case 'W':
			w = atoi(optarg);
			break;

			case 'I':
			image_dir = strdup(optarg);
			break;

			case 'H':
			h = atoi(optarg);
			break;

			default:
			help();
			return 0;
			break;
		}
	}
	/* create the context */
	eon_init();
	doc = eon_document_new(engine, w, h, NULL);
	/* create the canvas */
	canvas = eon_document_canvas_get(doc);
	/* create the external object */
	ext = eon_external_new(canvas);
	eon_external_file_set(ext, file);
	/* create the ui */
	ui_setup(canvas);
	eon_loop();
	eon_shutdown();

	return 0;
}
