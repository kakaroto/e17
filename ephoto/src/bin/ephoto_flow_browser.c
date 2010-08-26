#include "ephoto.h"

/*Callbacks*/
static void _ephoto_go_back(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_first(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_last(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_next(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_previous(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_go_slideshow(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_key_pressed(void *data, Evas *e, Evas_Object *obj, void *event_info);

/*Inline variables*/
static Eina_List *iter;
static Evas_Object *image, *image2, *toolbar;
static const char *cur_image;

static const char *toolbar_items[] = {
	"First",
	"Previous",
	"Next",
	"Last",
	"Slideshow"
};

/*Create the flow browser*/
void
ephoto_create_flow_browser(void)
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

	image2 = elm_image_add(em->win);
	elm_image_smooth_set(image2, EINA_TRUE);
        evas_object_size_hint_weight_set(image2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(image2, EVAS_HINT_FILL, EVAS_HINT_FILL);

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
        elm_toolbar_item_add(toolbar, o, "Slideshow", _ephoto_go_slideshow, NULL);
}

/*Show the flow browser*/
void
ephoto_show_flow_browser(const char *current_image)
{
	const char *file_type;
	Elm_Toolbar_Item *o;
	int i;

	if (current_image)
        {
		cur_image = current_image;

		evas_object_event_callback_add(em->flow_browser, EVAS_CALLBACK_KEY_UP,
						_ephoto_key_pressed, NULL);

		iter = eina_list_data_find_list(em->images, current_image);
		for (i = 0; i < (sizeof (toolbar_items) / sizeof (char*)); ++i)
		{
			o = elm_toolbar_item_find_by_label(toolbar, toolbar_items[i]);
			elm_toolbar_item_disabled_set(o, !iter ? EINA_TRUE : EINA_FALSE);
		}

		elm_box_unpack(em->flow_browser, image);
		elm_box_unpack(em->flow_browser, image2);
		elm_box_unpack(em->flow_browser, toolbar);

		file_type = strrchr(current_image, '.');
		if (strncasecmp(file_type, ".jpg", 4) && strncasecmp(file_type, ".jpeg", 5))
		{
			elm_image_file_set(image2, current_image, NULL);
			elm_box_pack_end(em->flow_browser, image2);
			evas_object_show(image2);
		}
		else
		{
			elm_photocam_file_set(image, current_image);
			elm_box_pack_end(em->flow_browser, image);
			evas_object_show(image);
		}
	}
	else
	{
		for (i = 0; i < (sizeof (toolbar_items) / sizeof (char*)); ++i)
                {
                        o = elm_toolbar_item_find_by_label(toolbar, toolbar_items[i]);
                        elm_toolbar_item_disabled_set(o, EINA_TRUE);
                }
	}
	elm_box_pack_end(em->flow_browser, toolbar);
	evas_object_show(toolbar);
	evas_object_show(em->flow_browser);
	evas_object_focus_set(em->flow_browser, 1);
}

/*Hide the flow browser*/
void 
ephoto_hide_flow_browser(void)
{
	evas_object_hide(image);
	evas_object_hide(image2);
	evas_object_hide(toolbar);
	evas_object_hide(em->flow_browser);
}

/*Delete the flow browser*/
void 
ephoto_delete_flow_browser(void)
{
	Eina_List *items;

	items = elm_toolbar_item_get_all(toolbar);
        while (items)
        {
                evas_object_del(eina_list_data_get(items));
                items = eina_list_next(items);
        }

	evas_object_del(image);
	evas_object_del(image2);
	evas_object_del(toolbar);
	evas_object_del(em->flow_browser);
}

/*A key has been pressed*/
static const struct
{
	const char *name;
	void (*func)(void *data, Evas_Object *obj, void *event_info);
} keys[] = {
	{ "Left", _ephoto_go_previous },
	{ "Right", _ephoto_go_next },
	{ "space", _ephoto_go_next },
	{ "Escape", _ephoto_go_back },
	{ NULL, NULL }
};

static void
_ephoto_key_pressed(void *data, Evas *e, Evas_Object *obj, void *event_data)
{
	Evas_Event_Key_Up *eku;
	int i;

	eku = (Evas_Event_Key_Up *)event_data;
	for (i = 0; keys[i].name; ++i)
		if (!strcmp(eku->keyname, keys[i].name))
			keys[i].func(NULL, NULL, NULL);
	printf("%s\n", eku->keyname);
}

/*Go back to the thumbnail viewer*/
static void 
_ephoto_go_back(void *data, Evas_Object *obj, void *event_info)
{
	ephoto_hide_flow_browser();
	ephoto_show_thumb_browser();

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the very first image in the list*/
static void 
_ephoto_go_first(void *data, Evas_Object *obj, void *event_info)
{
	const char *file_type;

	iter = eina_list_nth_list(em->images, 0);

	elm_box_unpack(em->flow_browser, image);
	elm_box_unpack(em->flow_browser, image2);
	elm_box_unpack(em->flow_browser, toolbar);
	evas_object_hide(image);
        evas_object_hide(image2);

        file_type = strrchr(eina_list_data_get(iter), '.');
        if (strncasecmp(file_type, ".jpg", 4) && strncasecmp(file_type, ".jpeg", 5))
        {
		elm_image_file_set(image2, eina_list_data_get(iter), NULL);
		elm_box_pack_end(em->flow_browser, image2);
		evas_object_show(image2);
        }
        else
        {
		elm_photocam_file_set(image, eina_list_data_get(iter));
		elm_box_pack_end(em->flow_browser, image);
		evas_object_show(image);
        }
	cur_image = eina_list_data_get(iter);

        elm_box_pack_end(em->flow_browser, toolbar);

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the very last image in the list*/
static void 
_ephoto_go_last(void *data, Evas_Object *obj, void *event_info)
{
	const char *file_type;

	iter = eina_list_last(em->images);

	elm_box_unpack(em->flow_browser, image);
	elm_box_unpack(em->flow_browser, image2);
	elm_box_unpack(em->flow_browser, toolbar);
	evas_object_hide(image);
        evas_object_hide(image2);

        file_type = strrchr(eina_list_data_get(iter), '.');
        if (strncasecmp(file_type, ".jpg", 4) && strncasecmp(file_type, ".jpeg", 5))
        {
		elm_image_file_set(image2, eina_list_data_get(iter), NULL);
		elm_box_pack_end(em->flow_browser, image2);
		evas_object_show(image2);
        }
        else
        {
		elm_photocam_file_set(image, eina_list_data_get(iter));
		elm_box_pack_end(em->flow_browser, image);
		evas_object_show(image);
        }
	cur_image = eina_list_data_get(iter);

        elm_box_pack_end(em->flow_browser, toolbar);

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the next image in the list*/
static void 
_ephoto_go_next(void *data, Evas_Object *obj, void *event_info)
{
	const char *file_type;

	iter = iter->next;
	if (!iter)
		iter = eina_list_nth_list(em->images, 0);

	elm_box_unpack(em->flow_browser, image);
	elm_box_unpack(em->flow_browser, image2);
	elm_box_unpack(em->flow_browser, toolbar);
	evas_object_hide(image);
        evas_object_hide(image2);

        file_type = strrchr(eina_list_data_get(iter), '.');
        if (strncasecmp(file_type, ".jpg", 4) && strncasecmp(file_type, ".jpeg", 5))
        {
		elm_image_file_set(image2, eina_list_data_get(iter), NULL);
		elm_box_pack_end(em->flow_browser, image2);
		evas_object_show(image2);
        }
        else
        {
		elm_photocam_file_set(image, eina_list_data_get(iter));
		elm_box_pack_end(em->flow_browser, image);
		evas_object_show(image);
        }
	cur_image = eina_list_data_get(iter);

        elm_box_pack_end(em->flow_browser, toolbar);	

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the previous image in the list*/
static void 
_ephoto_go_previous(void *data, Evas_Object *obj, void *event_info)
{
	const char *file_type;

	iter = iter->prev;
	if (!iter)
		iter = eina_list_last(em->images);

	elm_box_unpack(em->flow_browser, image);
	elm_box_unpack(em->flow_browser, image2);
	elm_box_unpack(em->flow_browser, toolbar);
	evas_object_hide(image);
	evas_object_hide(image2);

        file_type = strrchr(eina_list_data_get(iter), '.');
        if (strncasecmp(file_type, ".jpg", 4) && strncasecmp(file_type, ".jpeg", 5))
        {
		elm_image_file_set(image2, eina_list_data_get(iter), NULL);
		elm_box_pack_end(em->flow_browser, image2);
		evas_object_show(image2);
        }
        else
        {
		elm_photocam_file_set(image, eina_list_data_get(iter));
		elm_box_pack_end(em->flow_browser, image);
		evas_object_show(image);
        }
	cur_image = eina_list_data_get(iter);

        elm_box_pack_end(em->flow_browser, toolbar);

	elm_toolbar_item_unselect_all(toolbar);
}

/*Go to the slideshow*/
static void
_ephoto_go_slideshow(void *data, Evas_Object *obj, void *event_info)
{
	ephoto_hide_flow_browser();
	ephoto_show_slideshow(1, cur_image);
	elm_toolbar_item_unselect_all(toolbar);
}

