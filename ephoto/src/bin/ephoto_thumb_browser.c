#include "ephoto.h"

/*Callbacks*/
static void _ephoto_slider_changed(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_thumber_connected(void *data, Ethumb_Client *client, Eina_Bool success);
static void _ephoto_thumbnail_generated(void *data, Ethumb_Client *client, int id,
		                        const char *file, const char *key, 
					const char *thumb_path, const char *thumb_key, 
					Eina_Bool success);
static char *_ephoto_get_label(const void *data, Evas_Object *obj, const char *part);
static Evas_Object *_ephoto_get_icon(const void *data, Evas_Object *obj, const char *part);
static Eina_Bool _ephoto_get_state(const void *data, Evas_Object *obj, const char *part);
static void _ephoto_grid_del(const void *data, Evas_Object *obj);

/*Inline Variables*/
static Elm_Gengrid_Item_Class eg;
static Ethumb_Client *ec;
static int cur_val;
static Ecore_Thread *thread = NULL;

/*Create the thumbnail browser object*/
void
ephoto_create_thumb_browser(void)
{
	Evas_Object *o;
	char buf[PATH_MAX];

	elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/themes/default/ephoto.edj");

	ec = ethumb_client_connect(_ephoto_thumber_connected, NULL, NULL);

	getcwd(buf, PATH_MAX);

	em->toolbar = elm_toolbar_add(em->win);
	elm_toolbar_icon_size_set(em->toolbar, 32);
	elm_toolbar_homogenous_set(em->toolbar, EINA_TRUE);
	evas_object_size_hint_weight_set(em->toolbar, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(em->toolbar, EVAS_HINT_FILL, 0.5);
	elm_box_pack_end(em->box, em->toolbar);
	evas_object_show(em->toolbar);

	o = elm_icon_add(em->win);
	elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/change_directory.png", NULL);
	elm_toolbar_item_add(em->toolbar, o, "Change Directory", NULL, NULL);

	o = elm_icon_add(em->win);
	elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/filter.png", NULL);
        elm_toolbar_item_add(em->toolbar, o, "Filter", NULL, NULL);

	o = elm_icon_add(em->win);
	elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/view_presentation.png", NULL);
        elm_toolbar_item_add(em->toolbar, o, "View Presentation", NULL, NULL);

	o = elm_icon_add(em->win);
	elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(em->toolbar, o, "Play Slideshow", NULL, NULL);

	em->thumb_browser = elm_gengrid_add(em->win);
	elm_gengrid_align_set(em->thumb_browser, 0.5, 0.6);
	elm_gengrid_item_size_set(em->thumb_browser, 208, 146);
	elm_gengrid_horizontal_set(em->thumb_browser, EINA_TRUE);
	evas_object_size_hint_align_set(em->thumb_browser, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(em->thumb_browser, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_fill_set(em->thumb_browser, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_style_set(em->thumb_browser, "ephoto");
	elm_box_pack_end(em->box, em->thumb_browser);
	evas_object_show(em->thumb_browser);

	em->thbox = elm_box_add(em->win);
        elm_win_resize_object_add(em->win, em->thbox);
	elm_box_horizontal_set(em->thbox, EINA_TRUE);
        evas_object_size_hint_weight_set(em->thbox, EVAS_HINT_EXPAND, EVAS_HINT_FILL);
        evas_object_size_hint_fill_set(em->thbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_pack_end(em->box, em->thbox);
        evas_object_show(em->thbox);

	em->dir_label = elm_label_add(em->win);
	elm_label_label_set(em->dir_label, buf);
	evas_object_size_hint_weight_set(em->dir_label, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(em->dir_label, 0.01, 0.5);
	elm_box_pack_end(em->thbox, em->dir_label);
	evas_object_show(em->dir_label);

	em->thumb_slider = elm_slider_add(em->win);
	elm_slider_label_set(em->thumb_slider, "Thumb Size:");
	elm_slider_span_size_set(em->thumb_slider, 100);
	elm_slider_min_max_set(em->thumb_slider, 0, 100);
	elm_slider_value_set(em->thumb_slider, 50);
	elm_box_pack_end(em->thbox, em->thumb_slider);
	evas_object_show(em->thumb_slider);
	evas_object_smart_callback_add(em->thumb_slider, "changed", 
					_ephoto_slider_changed, NULL);
	cur_val = 50;

	eg.item_style = "ephoto";
	eg.func.label_get = _ephoto_get_label;
	eg.func.icon_get = _ephoto_get_icon;
	eg.func.state_get = _ephoto_get_state;
	eg.func.del = _ephoto_grid_del;
}

/*Show the thumbnail browser*/
void
ephoto_show_thumb_browser(void)
{
	evas_object_show(em->toolbar);
	evas_object_show(em->thumb_browser);
	evas_object_show(em->dir_label);
	evas_object_show(em->thumb_slider);
	evas_object_show(em->thbox);
}

/*Hide the thumbnail browser*/
void
ephoto_hide_thumb_browser(void)
{
	evas_object_hide(em->toolbar);
	evas_object_hide(em->thumb_browser);
	evas_object_hide(em->dir_label);
	evas_object_hide(em->thumb_slider);
	evas_object_hide(em->thbox);
}

/*Destroy the thumbnail browser*/
void
ephoto_delete_thumb_browser(void)
{
	evas_object_del(em->toolbar);
	evas_object_del(em->thumb_browser);
	evas_object_del(em->dir_label);
	evas_object_del(em->thumb_slider);
	evas_object_del(em->thbox);
	ethumb_client_disconnect(ec);
}

/* Use ecore thread facility to avoid lock completly */

/* List image in a directory from another thread */
static void
_ephoto_access_disk(Ecore_Thread *thread, void *data)
{
	Eina_Iterator *it = data;
	const char *file;
	const char *type;

	EINA_ITERATOR_FOREACH(it, file)
	{
		if (ecore_thread_check(thread)) break;

		type = efreet_mime_type_get((const char *)file);

		fprintf(stderr, "[%s] => [%s]\n", file, type);
		if (!strncmp(type, "image", 5))
			if (ecore_thread_notify(thread, file))
				continue ;
		eina_stringshare_del(file);
	}
}

static void
_ephoto_populate_end(void *data)
{
	Eina_Iterator *it = data;

	eina_iterator_free(it);

	thread = NULL;
}

/* Build the interface component after detection from listing thread */
static void
_ephoto_populate_notify(Ecore_Thread *thread, void *msg_data, void *data)
{
	const char *thumb;
	char *path = msg_data;

	em->images = eina_list_append(em->images, path);
	ethumb_client_file_set(ec, path, NULL);
	if (!ethumb_client_thumb_exists(ec))
	{
		ethumb_client_generate(ec, _ephoto_thumbnail_generated, NULL, NULL);
	}
	else
	{
		ethumb_client_thumb_path_get(ec, &thumb, NULL);
		_ephoto_thumbnail_generated(NULL, ec, 0, path, NULL,
					    thumb, NULL, EINA_TRUE);

	}
}

/* Start a thread to list images in a directory without locking the interface */
void
ephoto_populate_thumbnails(void)
{
	Eina_Iterator *it;
	char *dir, cwd[PATH_MAX];

	dir = getcwd(cwd, PATH_MAX);
	if (!dir) return ;

	it = eina_file_ls(dir);
	if (!it) return ;

	thread = ecore_long_run(_ephoto_access_disk,
				_ephoto_populate_notify,
				_ephoto_populate_end,
				_ephoto_populate_end,
				it,
				EINA_FALSE);
}

/*Change the thumbnail size*/
static void
_ephoto_slider_changed(void *data, Evas_Object *obj, void *event)
{

}

/*Callback when the client is connected*/
static void _ephoto_thumber_connected(void *data, Ethumb_Client *client, Eina_Bool success)
{
	if (success == EINA_TRUE)
	{
		ethumb_client_fdo_set(ec, ETHUMB_THUMB_LARGE);
	        ethumb_client_format_set(ec, ETHUMB_THUMB_FDO);
	        ethumb_client_aspect_set(ec, ETHUMB_THUMB_KEEP_ASPECT);
		ephoto_populate_thumbnails();
	}
	else
	{
		printf("Could not connect to ethumb!\n");
	}
}

/*Callback when the thumbnail has been generated!*/
static void 
_ephoto_thumbnail_generated(void *data, Ethumb_Client *client, int id,
			const char *file, const char *key, 
			const char *thumb_path, const char *thumb_key, 
			Eina_Bool success)
{
	Evas_Object *o;

	o = ephoto_thumb_add();
	evas_object_image_size_set(o, 176, 117);
	ephoto_thumb_fill_inside_set(o, 0);
	ephoto_thumb_file_set(o, thumb_path, 176, 117);
	evas_object_data_set(o, "file", strdup(file));

	if (success)
	{
		elm_gengrid_item_append(em->thumb_browser, &eg, o, NULL, NULL);
	}
}

/*Get the label for the icon in the grid*/
static char *
_ephoto_get_label(const void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *o;
	char *file, *bname;

	o = (Evas_Object *)data;

	file = evas_object_data_get(o, "file");
	bname = basename(file);

	return strdup(bname);
}

/*Get the image for the icon in the grid*/
static Evas_Object *
_ephoto_get_icon(const void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *edje, *thumb, *o;

	o = (Evas_Object *)data;

	if (!strcmp(part, "elm.swallow.icon"))
	{
		thumb = edje_object_add(evas_object_evas_get(em->thumb_browser));
		edje_object_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
				"/ephoto/thumb/shadow");
		evas_object_show(thumb);
		evas_object_resize(thumb, 198, 136);
		evas_object_size_hint_aspect_set(thumb, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
		evas_object_size_hint_align_set(thumb, 0.5, 0.5);
		evas_object_size_hint_fill_set(thumb, 1.0, 1.0);
		evas_object_size_hint_min_set(thumb, 198, 136);
		evas_object_size_hint_max_set(thumb, 198, 136);
	
		edje = edje_object_add(evas_object_evas_get(em->thumb_browser));
		edje_object_file_set(edje, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
								"/ephoto/thumb/image");
		evas_object_show(edje);
	
		edje_object_part_swallow(edje, "ephoto.swallow.content", o);
		edje_object_part_swallow(thumb, "ephoto.swallow.content", edje);
	
		evas_object_resize(o, 176, 117);
		evas_object_size_hint_min_set(o, 176, 117);
		evas_object_size_hint_max_set(o, 176, 117);
		evas_object_resize(edje, 176, 117);
		evas_object_size_hint_min_set(edje, 176, 117);
		evas_object_size_hint_max_set(edje, 176, 117);

		return thumb;
	}

	return NULL;
}

/*Get the state of the icon in the grid!*/
static Eina_Bool
_ephoto_get_state(const void *data, Evas_Object *obj, const char *part)
{
	return EINA_FALSE;
}

/*Delete the grid*/
static void
_ephoto_grid_del(const void *data, Evas_Object *obj)
{
	return;
}

