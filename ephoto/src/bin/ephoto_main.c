#include "ephoto.h"

/*Ephoto Main Global*/
Ephoto *em;

/*Inline Callbacks*/
static void _ephoto_delete_main_window(void *data, Evas_Object *obj, void *event_info);

/*Create the main ephoto window*/
void 
ephoto_create_main_window(const char *directory, const char *image)
{
	em = calloc(1, sizeof(Ephoto));

	/*Setup the main window*/
	em->win = elm_win_add(NULL, "ephoto", ELM_WIN_BASIC);
	elm_win_title_set(em->win, "Ephoto");
	evas_object_resize(em->win, 840, 530);
	evas_object_show(em->win);

	/*Callback to close the main window*/
	evas_object_smart_callback_add(em->win, "delete,request", 
					_ephoto_delete_main_window, NULL);

	/*Grab the main evas canvas we are working with*/
	em->e = evas_object_evas_get(em->win);

	/*Set a custom background for the window*/
	em->bg = elm_bg_add(em->win);
	elm_win_resize_object_add(em->win, em->bg);
	evas_object_size_hint_weight_set(em->bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(em->bg);

	em->box = elm_box_add(em->win);
	elm_win_resize_object_add(em->win, em->box);
	evas_object_size_hint_weight_set(em->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_fill_set(em->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(em->box);	

	ephoto_create_thumb_browser(directory);
	ephoto_create_flow_browser();
	if (image)
	{
		ephoto_show_flow_browser(image);
		eina_stringshare_del(image);
	}
	else
		ephoto_show_thumb_browser();
}

/*Delete the main ephoto window*/
static void
_ephoto_delete_main_window(void *data, Evas_Object *obj, void *event_info)
{
	ephoto_delete_thumb_browser();
	ephoto_delete_flow_browser();
	evas_object_del(em->box);
	evas_object_del(em->bg);
	if (em->images)
		eina_list_free(em->images);
	free(em);
	elm_exit();
}

