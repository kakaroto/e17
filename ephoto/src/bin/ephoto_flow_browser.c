#include "ephoto.h"

/*Callbacks*/
static void _ephoto_go_back(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_first(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_last(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_next(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_previous(void *data, Evas_Object *obj, void *event_info);

/*Inline variables*/
static Eina_List *iter;
static Evas_Object *image, *toolbar;

/*Create the flow browser*/
void ephoto_create_flow_browser(void)
{
	Evas_Object *o;

	em->flow_browser = elm_box_add(em->win);
	elm_win_resize_object_add(em->win, em->flow_browser);
	evas_object_size_hint_weight_set(em->flow_browser, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(em->flow_browser, EVAS_HINT_FILL, EVAS_HINT_FILL);

	image = elm_photocam_add(em->win);
	elm_photocam_zoom_mode_set(image, ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);
	evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(image, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_pack_end(em->flow_browser, image);

	toolbar = elm_toolbar_add(em->win);
        elm_toolbar_icon_size_set(toolbar, 24);
        elm_toolbar_homogenous_set(toolbar, EINA_TRUE);
        evas_object_size_hint_weight_set(toolbar, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, 0.5);
        elm_box_pack_end(em->flow_browser, toolbar);
        evas_object_show(toolbar);

	o = elm_icon_add(em->win);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/go_back.png", NULL);
        elm_toolbar_item_add(toolbar, o, "Go Back", _ephoto_go_back, NULL);

        o = elm_icon_add(em->win);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/first.png", NULL);
        elm_toolbar_item_add(toolbar, o, "First", _ephoto_go_first, NULL);

	o = elm_icon_add(em->win);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/previous.png", NULL);
        elm_toolbar_item_add(toolbar, o, "Previous", _ephoto_go_previous, NULL);

	o = elm_icon_add(em->win);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/next.png", NULL);
        elm_toolbar_item_add(toolbar, o, "Next", _ephoto_go_next, NULL);

        o = elm_icon_add(em->win);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/last.png", NULL);
        elm_toolbar_item_add(toolbar, o, "Last", _ephoto_go_last, NULL);

	o = elm_icon_add(em->win);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(toolbar, o, "Slideshow", NULL, NULL);
}

/*Show the flow browser*/
void ephoto_show_flow_browser(const char *current_image)
{
	iter = eina_list_data_find_list(em->images, current_image);
	if (iter == NULL)
		iter = eina_list_nth_list(em->images, 0);

	elm_photocam_file_set(image, current_image);

	evas_object_show(image);
	evas_object_show(toolbar);
	evas_object_show(em->flow_browser);
}

/*Hide the flow browser*/
void ephoto_hide_flow_browser(void)
{
	evas_object_hide(image);
	evas_object_hide(toolbar);
	evas_object_hide(em->flow_browser);
}

/*Delete the flow browser*/
void ephoto_delete_flow_browser(void)
{
	Eina_List *items;

	items = elm_toolbar_item_get_all(toolbar);
        while (items)
        {
                evas_object_del(eina_list_data_get(items));
                items = eina_list_next(items);
        }

	evas_object_del(image);
	evas_object_del(toolbar);
	evas_object_del(em->flow_browser);
}

/*Go back to the thumbnail viewer*/
static void _ephoto_go_back(void *data, Evas_Object *obj, void *event_info)
{
	ephoto_hide_flow_browser();
	ephoto_show_thumb_browser();

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the very first image in the list*/
static void _ephoto_go_first(void *data, Evas_Object *obj, void *event_info)
{
	iter = eina_list_nth_list(em->images, 0);

        elm_photocam_file_set(image, eina_list_data_get(iter));

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the very last image in the list*/
static void _ephoto_go_last(void *data, Evas_Object *obj, void *event_info)
{
	iter = eina_list_last(em->images);

	elm_photocam_file_set(image, eina_list_data_get(iter));

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the next image in the list*/
static void _ephoto_go_next(void *data, Evas_Object *obj, void *event_info)
{
	iter = iter->next;
	if (!iter)
		iter = eina_list_nth_list(em->images, 0);

	elm_photocam_file_set(image, eina_list_data_get(iter));

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the previous image in the list*/
static void _ephoto_go_previous(void *data, Evas_Object *obj, void *event_info)
{
	iter = iter->prev;
	if (!iter)
		iter = eina_list_last(em->images);

	elm_photocam_file_set(image, eina_list_data_get(iter));

	elm_toolbar_item_unselect_all(toolbar);
}

