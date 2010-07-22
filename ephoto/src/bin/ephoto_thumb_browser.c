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
static void _ephoto_thumb_clicked_job(void *data);
static void _ephoto_thumb_clicked(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_view_large(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_change_directory(void *data, Evas_Object *obj, void *event_info);

/*Inline Variables*/
static Elm_Gengrid_Item_Class eg;
static Ethumb_Client *ec;
static const char *current_directory;
static int cur_val;
static Eio_List *list = NULL;
static Evas_Object *toolbar, *dir_label, *thumb_slider, *thbox;

typedef struct _Ephoto_Thumb_Data Ephoto_Thumb_Data;
struct _Ephoto_Thumb_Data
{
	const char *thumb_path;
	const char *file;
};

/*Create the thumbnail browser object*/
void
ephoto_create_thumb_browser(const char *directory)
{
	Evas_Object *o;
	char buf[PATH_MAX];

	elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/themes/default/ephoto.edj");

	ec = ethumb_client_connect(_ephoto_thumber_connected, NULL, NULL);

	if (!directory)
	{
		getcwd(buf, PATH_MAX);
		current_directory = eina_stringshare_add(buf);
	}
	else
	{
		current_directory = eina_stringshare_add(directory);
	}
	toolbar = elm_toolbar_add(em->win);
	elm_toolbar_icon_size_set(toolbar, 24);
	elm_toolbar_homogenous_set(toolbar, EINA_TRUE);
	evas_object_size_hint_weight_set(toolbar, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, 0.5);
	elm_box_pack_end(em->box, toolbar);

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
	evas_object_smart_callback_add(em->thumb_browser, "clicked", _ephoto_thumb_clicked, NULL);
	elm_box_pack_end(em->box, em->thumb_browser);

	thbox = elm_box_add(em->win);
	elm_box_horizontal_set(thbox, EINA_TRUE);
        evas_object_size_hint_weight_set(thbox, EVAS_HINT_EXPAND, EVAS_HINT_FILL);
        evas_object_size_hint_fill_set(thbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_pack_end(em->box, thbox);

	dir_label = elm_label_add(em->win);
	elm_label_label_set(dir_label, buf);
	evas_object_size_hint_weight_set(dir_label, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(dir_label, 0.01, 0.5);
	elm_box_pack_end(thbox, dir_label);

	thumb_slider = elm_slider_add(em->win);
	elm_slider_label_set(thumb_slider, "Thumb Size:");
	elm_slider_span_size_set(thumb_slider, 100);
	elm_slider_min_max_set(thumb_slider, 0, 100);
	elm_slider_value_set(thumb_slider, 50);
	elm_box_pack_end(thbox, thumb_slider);
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
	const Eina_List *items, *l, *iter;
	Elm_Gengrid_Item *item;
	Ephoto_Thumb_Data *etd;

	items = elm_toolbar_item_get_all(toolbar);
	while (items)
	{
		evas_object_del(eina_list_data_get(items));
		items = eina_list_next(items);
	}
	l = elm_gengrid_items_get(em->thumb_browser);
        EINA_LIST_FOREACH(l, iter, item)
        {
        	etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get(item);
                eina_stringshare_del(etd->thumb_path);
                eina_stringshare_del(etd->file);
                free(etd);
        }
	eina_stringshare_del(current_directory);
	evas_object_del(toolbar);
	evas_object_del(em->thumb_browser);
	evas_object_del(dir_label);
	evas_object_del(thumb_slider);
	evas_object_del(thbox);
	ethumb_client_disconnect(ec);
}

/* Use ecore thread facility to avoid lock completly */

/* Check image type from another thread */
static Eina_Bool
_ephoto_populate_filter(const char *file, void *data)
{
	const char *type;

	if (!(type = efreet_mime_type_get((const char *)file)))
		return EINA_FALSE;

	if (!strncmp(type, "image", 5))
		return EINA_TRUE;

	return EINA_FALSE;
}

/*Done populating images*/
static void
_ephoto_populate_end(void *data)
{
	list = NULL;

	efreet_mime_shutdown();
}

/* Build the interface component after detection from main thread */
static void
_ephoto_populate_main(const char *file, void *data)
{
	const char *thumb;

	file = eina_stringshare_ref(file);

	em->images = eina_list_append(em->images, file);
	ethumb_client_file_set(ec, file, NULL);
	if (!ethumb_client_thumb_exists(ec))
	{
		ethumb_client_generate(ec, _ephoto_thumbnail_generated, NULL, NULL);
	}
	else
	{
		ethumb_client_thumb_path_get(ec, &thumb, NULL);
		_ephoto_thumbnail_generated(NULL, ec, 0, file, NULL,
					    thumb, NULL, EINA_TRUE);
	}
}

/* Start a thread to list images in a directory without locking the interface */
void
ephoto_populate_thumbnails(void)
{
	if (!current_directory) return ;

	if (!efreet_mime_init())
		fprintf(stderr, "Could not init efreet_mime!\n");

	list = eio_file_ls(current_directory,
			   _ephoto_populate_filter,
			   _ephoto_populate_main,
			   _ephoto_populate_end,
			   NULL);
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
		Ephoto_Thumb_Data *etd;

		etd = calloc(1, sizeof(etd));
		etd->thumb_path = eina_stringshare_add(thumb_path);
		etd->file = eina_stringshare_add(file);

		elm_gengrid_item_append(em->thumb_browser, &eg, etd, NULL, NULL);
	}
}

/*Get the label for the icon in the grid*/
static char *
_ephoto_get_label(const void *data, Evas_Object *obj, const char *part)
{
	const char *bname;
	Ephoto_Thumb_Data *etd;

	etd = (Ephoto_Thumb_Data *)data;

	bname = basename(etd->file);

	return strdup(bname);
}

/*Get the image for the icon in the grid*/
static Evas_Object *
_ephoto_get_icon(const void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *thumb, *o;
	Ephoto_Thumb_Data *etd;

	etd = (Ephoto_Thumb_Data *)data;

	if (!strcmp(part, "elm.swallow.icon"))
	{
		thumb = elm_layout_add(em->win);
		elm_layout_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
				"/ephoto/thumb");
		evas_object_size_hint_weight_set(thumb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_show(thumb);

		o = elm_bg_add(em->win);
		elm_bg_file_set(o, etd->thumb_path, NULL);
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

/*Thumb clicked job*/
static void
_ephoto_thumb_clicked_job(void *data)
{
	const char *file;

	file = data;

	ephoto_hide_thumb_browser();
        ephoto_show_flow_browser(file);
}

/*Check to see if the thumbnail was double clicked*/
static void 
_ephoto_thumb_clicked(void *data, Evas_Object *obj, void *event_info)
{
	const Eina_List *selected;
	Ephoto_Thumb_Data *etd;
	Evas_Object *o;

	selected = elm_gengrid_selected_items_get(em->thumb_browser);
	o = eina_list_data_get(selected);
	etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get((Elm_Gengrid_Item *)o);
	ecore_job_add(_ephoto_thumb_clicked_job, etd->file);
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
	const Eina_List *l, *iter;
	Evas_Object *win;
	Elm_Gengrid_Item *item;
	Ephoto_Thumb_Data *etd;
	const char *directory;

	win = (Evas_Object *)data;

	directory = elm_fileselector_selected_get(obj);

	if ((directory) && (eina_stringshare_replace(&current_directory, directory)))
	{
		l = elm_gengrid_items_get(em->thumb_browser);
                EINA_LIST_FOREACH(l, iter, item)
                {
                        etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get(item);
                        eina_stringshare_del(etd->thumb_path);
                        eina_stringshare_del(etd->file);
                        free(etd);
                }
		elm_gengrid_clear(em->thumb_browser);
		eina_list_free(em->images);
		em->images = NULL;
		ephoto_populate_thumbnails();
		elm_label_label_set(dir_label, current_directory);
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
	Ephoto_Thumb_Data *etd;

	ephoto_hide_thumb_browser();

	selected = elm_gengrid_selected_items_get(em->thumb_browser);

	if (eina_list_data_get(selected))
	{
		o = eina_list_data_get(selected);
		etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get((Elm_Gengrid_Item *)o);
		ephoto_show_flow_browser(etd->file);
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

