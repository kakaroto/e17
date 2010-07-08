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
static void _ephoto_thumb_clicked(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_view_large(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_change_directory(void *data, Evas_Object *obj, void *event_info);

/*Inline Variables*/
static Elm_Gengrid_Item_Class eg;
static Ethumb_Client *ec;
static char *current_directory;
static int cur_val;
static Ecore_Thread *thread = NULL;
static Evas_Object *toolbar, *dir_label, *thumb_slider, *thbox;

/*Create the thumbnail browser object*/
void
ephoto_create_thumb_browser(void)
{
	Evas_Object *o;
	char buf[PATH_MAX];

	elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/themes/default/ephoto.edj");

	ec = ethumb_client_connect(_ephoto_thumber_connected, NULL, NULL);

	getcwd(buf, PATH_MAX);
	current_directory = strdup(buf);

	toolbar = elm_toolbar_add(em->win);
	elm_toolbar_icon_size_set(toolbar, 24);
	elm_toolbar_homogenous_set(toolbar, EINA_TRUE);
	evas_object_size_hint_weight_set(toolbar, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, 0.5);
	elm_box_pack_end(em->box, toolbar);
	evas_object_show(toolbar);

	o = elm_icon_add(em->win);
	elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/change_directory.png", NULL);
	elm_toolbar_item_add(toolbar, o, "Change Directory", _ephoto_change_directory, NULL);

	o = elm_icon_add(em->win);
	elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/filter.png", NULL);
        elm_toolbar_item_add(toolbar, o, "Filter", NULL, NULL);

	o = elm_icon_add(em->win);
	elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/view_presentation.png", NULL);
        elm_toolbar_item_add(toolbar, o, "View Large", _ephoto_view_large, NULL);

	o = elm_icon_add(em->win);
	elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(toolbar, o, "Play Slideshow", NULL, NULL);

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

	thbox = elm_box_add(em->win);
	elm_box_horizontal_set(thbox, EINA_TRUE);
        evas_object_size_hint_weight_set(thbox, EVAS_HINT_EXPAND, EVAS_HINT_FILL);
        evas_object_size_hint_fill_set(thbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_pack_end(em->box, thbox);
        evas_object_show(thbox);

	dir_label = elm_label_add(em->win);
	elm_label_label_set(dir_label, buf);
	evas_object_size_hint_weight_set(dir_label, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(dir_label, 0.01, 0.5);
	elm_box_pack_end(thbox, dir_label);
	evas_object_show(dir_label);

	thumb_slider = elm_slider_add(em->win);
	elm_slider_label_set(thumb_slider, "Thumb Size:");
	elm_slider_span_size_set(thumb_slider, 100);
	elm_slider_min_max_set(thumb_slider, 0, 100);
	elm_slider_value_set(thumb_slider, 50);
	elm_box_pack_end(thbox, thumb_slider);
	evas_object_show(thumb_slider);
	evas_object_smart_callback_add(thumb_slider, "changed", 
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
	evas_object_show(toolbar);
	evas_object_show(em->thumb_browser);
	evas_object_show(dir_label);
	evas_object_show(thumb_slider);
	evas_object_show(thbox);
}

/*Hide the thumbnail browser*/
void
ephoto_hide_thumb_browser(void)
{
	evas_object_hide(toolbar);
	evas_object_hide(em->thumb_browser);
	evas_object_hide(dir_label);
	evas_object_hide(thumb_slider);
	evas_object_hide(thbox);
}

/*Destroy the thumbnail browser*/
void
ephoto_delete_thumb_browser(void)
{
	Eina_List *items;

	items = elm_toolbar_item_get_all(toolbar);
	while (items)
	{
		evas_object_del(eina_list_data_get(items));
		items = eina_list_next(items);
	}
	free(current_directory);
	evas_object_del(toolbar);
	evas_object_del(em->thumb_browser);
	evas_object_del(dir_label);
	evas_object_del(thumb_slider);
	evas_object_del(thbox);
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
	if (!efreet_mime_init())
		fprintf(stderr, "Could not init efreet_mime!\n");

	EINA_ITERATOR_FOREACH(it, file)
	{
		if (ecore_thread_check(thread)) break;

		if (!(type = efreet_mime_type_get((const char *)file)))
			continue;

		fprintf(stderr, "[%s] => [%s]\n", file, type);
		if (!strncmp(type, "image", 5))
			if (ecore_thread_notify(thread, file))
				continue ;
		eina_stringshare_del(file);
	}
	efreet_mime_shutdown();
}

/*Done populating images*/
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

	if (!current_directory) return ;

	it = eina_file_ls(current_directory);
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
	int w, h, val;

	val = elm_slider_value_get(thumb_slider);
	elm_gengrid_item_size_get(em->thumb_browser, &w, &h);
	if (val < cur_val)
	{
		w -= cur_val-val;
		h -= cur_val-val;
	}
	else if (val > cur_val)
	{
		w += val-cur_val;
		h += val-cur_val;
	}
	elm_gengrid_item_size_set(em->thumb_browser, w, h);
	cur_val = val;
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
	if (success)
	{
		eina_hash_add(em->thumbs_images, file, eina_stringshare_ref(thumb_path));
		elm_gengrid_item_append(em->thumb_browser, &eg, file, NULL, NULL);
	}
}

/*Get the label for the icon in the grid*/
static char *
_ephoto_get_label(const void *data, Evas_Object *obj, const char *part)
{
	char *file;
	const char *bname;

	file = (char *)data;

	bname = basename(file);

	return strdup(bname);
}

/*Get the image for the icon in the grid*/
static Evas_Object *
_ephoto_get_icon(const void *data, Evas_Object *obj, const char *part)
{
	char *file, *path;
	Evas_Object *thumb, *o;

	file = (char *)data;
	path = (char *)eina_hash_find(em->thumbs_images, file);
	
	if (!strcmp(part, "elm.swallow.icon"))
	{
		thumb = elm_layout_add(em->win);
		elm_layout_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
				"/ephoto/thumb");
		evas_object_size_hint_weight_set(thumb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_event_callback_add(thumb, EVAS_CALLBACK_MOUSE_DOWN,
						_ephoto_thumb_clicked, NULL);
		evas_object_show(thumb);

		o = elm_bg_add(em->win);
		elm_bg_file_set(o, path, NULL);
		evas_object_resize(o, 176, 117);

		elm_layout_content_set(thumb, "ephoto.swallow.content", o);

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

/*Check to see if the thumbnail was double clicked*/
static void 
_ephoto_thumb_clicked(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	char *file;
	const Eina_List *selected;
	Evas_Event_Mouse_Down *emd;
	Evas_Object *o;

	emd = event_info;
	if (emd->flags == EVAS_BUTTON_DOUBLE_CLICK || 
		emd->flags == EVAS_BUTTON_TRIPLE_CLICK)
	{
		selected = elm_gengrid_selected_items_get(em->thumb_browser);
		o = eina_list_data_get(selected);
		file = (char *)elm_gengrid_item_data_get((Elm_Gengrid_Item *)o);
		ephoto_hide_thumb_browser();
		ephoto_show_flow_browser(file);
	}
}

/*File Selector is show*/
static void
_ephoto_fileselector_shown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	elm_fileselector_path_set(obj, current_directory);
	evas_render(em->e);
}

/*Directory has been chosen*/
static void
_ephoto_directory_chosen(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *win;
	const char *directory;

	win = (Evas_Object *)data;

	directory = elm_fileselector_selected_get(obj);

	if (directory && strcmp(directory, current_directory))
	{
		elm_gengrid_clear(em->thumb_browser);
		free(current_directory);
		current_directory = strdup(directory);
		eina_list_free(em->images);
		em->images = NULL;
		ephoto_populate_thumbnails();
	}
	evas_object_del(obj);
	evas_object_del(win);
	elm_toolbar_item_unselect_all(toolbar);
}

/*Show the flow browser*/
static void 
_ephoto_view_large(void *data, Evas_Object *obj, void *event_info)
{
	const Eina_List *selected;
	Evas_Object *o;
	char *file;

	ephoto_hide_thumb_browser();

	selected = elm_gengrid_selected_items_get(em->thumb_browser);

	if (eina_list_data_get(selected))
	{
		o = eina_list_data_get(selected);
		file = (char *)elm_gengrid_item_data_get((Elm_Gengrid_Item *)o);
		ephoto_show_flow_browser(file);
	}
	else
	{
		ephoto_show_flow_browser(eina_list_data_get(em->images));
	}

	elm_toolbar_item_unselect_all(toolbar);
}

/*Change directory*/
static void
_ephoto_change_directory(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *win, *fsel;

	win = elm_win_inwin_add(em->win);
	
	fsel = elm_fileselector_add(em->win);
	elm_fileselector_folder_only_set(fsel, EINA_TRUE);
	elm_fileselector_expandable_set(fsel, EINA_TRUE);
	elm_fileselector_buttons_ok_cancel_set(fsel, EINA_TRUE);
	evas_object_event_callback_add(fsel, EVAS_CALLBACK_SHOW, 
					_ephoto_fileselector_shown, NULL);
	evas_object_smart_callback_add(fsel, "done", 
					_ephoto_directory_chosen, win);

	elm_win_inwin_content_set(win, fsel);
	elm_win_inwin_activate(win);

	evas_object_show(fsel);
	evas_object_show(win);
}

