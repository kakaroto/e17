#include "ephoto.h"

/*General Callbacks */
static void window_close(Ecore_Evas *ee);
static void window_resize(Ecore_Evas *ee);
static void window_shown(void *data, Evas *e, Evas_Object *obj, void *event_info);

/*Ephoto Main Global*/
Ephoto *em;

/*Create the main ephoto window*/
void create_main_window(void)
{
	em = calloc(1, sizeof(Ephoto));
	em->sel = NULL;

	em->ee = ecore_evas_software_x11_new(0, 0, 0, 0, 920, 530);
	ecore_evas_title_set(em->ee, "Ephoto");
	ecore_evas_name_class_set(em->ee, "Ephoto", "Ephoto");
	ecore_evas_callback_destroy_set(em->ee, window_close);
	ecore_evas_callback_resize_set(em->ee, window_resize);
	ecore_evas_show(em->ee);

	em->e = ecore_evas_get(em->ee);

	em->bg = edje_object_add(em->e);
	edje_object_file_set(em->bg, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", 
						"/ephoto/window/background");
	evas_object_resize(em->bg, 920, 530);
	evas_object_move(em->bg, 0, 0);
	evas_object_event_callback_add(em->bg, EVAS_CALLBACK_SHOW, window_shown, NULL);
	evas_object_show(em->bg);
}

static void
window_shown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	add_image_browser();
	add_flow_view();
	show_image_browser();
	evas_object_event_callback_del(em->bg, EVAS_CALLBACK_SHOW, window_shown);
}

/*The window has been closed, free and quit*/
static void window_close(Ecore_Evas *ee)
{
	evas_object_del(em->image_browser);
	evas_object_del(em->flow);
	if (em->images)
		eina_list_free(em->images);
	free(em);
	ecore_main_loop_quit();
}

/*The window has been resized, update everything*/
static void window_resize(Ecore_Evas *ee)
{
	int w, h;

	ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
	evas_object_resize(em->bg, w, h);
	evas_object_move(em->bg, 0, 0);
}

