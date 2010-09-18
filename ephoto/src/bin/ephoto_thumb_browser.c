#include "ephoto.h"

typedef struct _Ephoto_Thumb_Data Ephoto_Thumb_Data;
typedef struct _Ephoto_Thumb_Browser Ephoto_Thumb_Browser;

struct _Ephoto_Thumb_Data
{
    const char *thumb_path;
    const char *file;
};

struct _Ephoto_Thumb_Browser
{    
        Evas_Object *layout;
        Evas_Object *thumb_browser;
        Evas_Object *dir_label;
        Evas_Object *toolbar;
        Evas_Object *thumb_slider;
        Evas_Object *thbox;
        Evas_Object *fsel_win;
        Elm_Gengrid_Item_Class eg;
        Ethumb_Client *ec;
        const char *current_directory;
        Eio_File *list;
};

#define THUMB_RATIO (256 / 192)

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
static void _ephoto_view_slideshow(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
/*Create the thumbnail browser object*/
Evas_Object *
ephoto_create_thumb_browser(Evas_Object *parent, const char *directory)
{
	Evas_Object *o;
	char buf[PATH_MAX];
        Ephoto_Thumb_Browser *tb;

        tb = calloc(1, sizeof(Ephoto_Thumb_Browser));

	elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/themes/default/ephoto.edj");

	tb->ec = ethumb_client_connect(_ephoto_thumber_connected, tb, NULL);

	tb->layout = elm_layout_add(parent);
	elm_layout_file_set(tb->layout, 
			    PACKAGE_DATA_DIR "/themes/default/ephoto.edj", 
			    "ephoto/browser/layout");

	evas_object_size_hint_weight_set(tb->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_fill_set(tb->layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(tb->layout);

	if (!directory)
	{
		getcwd(buf, PATH_MAX);
		tb->current_directory = eina_stringshare_add(buf);
	}
	else
	{
		tb->current_directory = eina_stringshare_add(directory);
	}

        tb->thbox = elm_box_add(tb->layout);
        elm_box_horizontal_set(tb->thbox, EINA_TRUE);
        evas_object_size_hint_weight_set(tb->thbox, EVAS_HINT_EXPAND, EVAS_HINT_FILL);
        evas_object_size_hint_fill_set(tb->thbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_layout_content_set(tb->layout, "ephoto.location.swallow", tb->thbox);

        tb->dir_label = elm_label_add(tb->thbox);
        elm_label_label_set(tb->dir_label, tb->current_directory);
        evas_object_size_hint_weight_set(tb->dir_label, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(tb->dir_label, 0.01, 0.5);
        elm_box_pack_end(tb->thbox, tb->dir_label);

        tb->thumb_slider = elm_slider_add(tb->thbox);
        elm_slider_label_set(tb->thumb_slider, "Thumb Size:");
        elm_slider_span_size_set(tb->thumb_slider, 100);
        elm_slider_min_max_set(tb->thumb_slider, 80, 300);
        elm_slider_value_set(tb->thumb_slider, em->config->thumb_size);
        elm_box_pack_end(tb->thbox, tb->thumb_slider);
        evas_object_smart_callback_add(tb->thumb_slider, "changed",
                                        _ephoto_slider_changed, tb);
	evas_object_show(tb->thumb_slider);
	evas_object_show(tb->dir_label);

	tb->thumb_browser = elm_gengrid_add(tb->layout);
	elm_gengrid_align_set(tb->thumb_browser, 0.5, 0.5);
        elm_gengrid_bounce_set(tb->thumb_browser, EINA_TRUE, EINA_FALSE);
	elm_gengrid_item_size_set(tb->thumb_browser, em->config->thumb_size, em->config->thumb_size / THUMB_RATIO);
	elm_gengrid_horizontal_set(tb->thumb_browser, EINA_TRUE);
	evas_object_size_hint_align_set(tb->thumb_browser, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(tb->thumb_browser, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_fill_set(tb->thumb_browser, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_style_set(tb->thumb_browser, "ephoto");
	evas_object_smart_callback_add(tb->thumb_browser, "clicked", _ephoto_thumb_clicked, tb);
	elm_layout_content_set(tb->layout, "ephoto.thumb.swallow", tb->thumb_browser);

        tb->toolbar = elm_toolbar_add(tb->layout);
        elm_toolbar_icon_size_set(tb->toolbar, 24);
        elm_toolbar_homogenous_set(tb->toolbar, EINA_TRUE);
        elm_toolbar_scrollable_set(tb->toolbar, EINA_FALSE);

        evas_object_size_hint_weight_set(tb->toolbar, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(tb->toolbar, EVAS_HINT_FILL, 0.5);

	elm_layout_content_set(tb->layout, "ephoto.toolbar.swallow", tb->toolbar);

        o = elm_icon_add(tb->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/change_directory.png", NULL);
        elm_toolbar_item_add(tb->toolbar, o, "Change Directory", _ephoto_change_directory, tb);

        o = elm_icon_add(tb->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/filter.png", NULL);
        elm_toolbar_item_add(tb->toolbar, o, "Filter", NULL, NULL);

        o = elm_icon_add(tb->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/view_presentation.png", NULL);
        elm_toolbar_item_add(tb->toolbar, o, "View Large", _ephoto_view_large, tb);

        o = elm_icon_add(tb->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(tb->toolbar, o, "Play Slideshow", _ephoto_view_slideshow, tb);

	tb->eg.item_style = "ephoto";
	tb->eg.func.label_get = _ephoto_get_label;
	tb->eg.func.icon_get = _ephoto_get_icon;
	tb->eg.func.state_get = _ephoto_get_state;
	tb->eg.func.del = _ephoto_grid_del;
        
        evas_object_data_set(tb->layout, "thumb_browser", tb);
        evas_object_event_callback_add(tb->layout, EVAS_CALLBACK_DEL, _ephoto_del_cb, tb);
        evas_object_event_callback_add(tb->layout, EVAS_CALLBACK_SHOW, _ephoto_show_cb, tb);
	return tb->layout;
}

/*Show the thumbnail browser*/
static void
_ephoto_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)

{
        Ephoto_Thumb_Browser *tb = evas_object_data_get(obj, "thumb_browser");

	/* evas_object_show(tb->toolbar); */
	/* evas_object_show(tb->thumb_browser); */
	/* evas_object_show(tb->dir_label); */
	/* evas_object_show(tb->thumb_slider); */
	/* evas_object_show(tb->thbox); */

	if (tb->current_directory)
	{
		char *buffer;
		int length;

		length = strlen(tb->current_directory) + strlen("Ephoto - ") + 1;
		buffer = alloca(length);
		snprintf(buffer, length, "Ephoto - %s", tb->current_directory);
		elm_win_title_set(em->win, buffer);
	} else {
		elm_win_title_set(em->win, "Ephoto");
	}
}

/* Use ecore thread facility to avoid lock completly */

/* Check image type from another thread */
static Eina_Bool
_ephoto_populate_filter(const void *data, const char *file)
{
	const char *type;

	if (!(type = efreet_mime_type_get(file)))
		return EINA_FALSE;
	if (!strncmp(type, "image", 5))
		return EINA_TRUE;

	return EINA_FALSE;
}

/*Done populating images*/
static void
_ephoto_populate_end(const void *data)
{
        Ephoto_Thumb_Browser *tb = (Ephoto_Thumb_Browser *)data;

        tb->list = NULL;
        efreet_mime_shutdown();
}

static void
_ephoto_populate_error(int error, const void *data)
{
        Ephoto_Thumb_Browser *tb = (Ephoto_Thumb_Browser*)data;
	/* We don't handle error case in ephoto */
	tb->list = NULL;

	efreet_mime_shutdown();
}

/* Build the interface component after detection from main thread */
static void
_ephoto_populate_main(const void *data, const char *file)
{
        Ephoto_Thumb_Browser *tb = (Ephoto_Thumb_Browser*)data;
	const char *thumb;

	file = eina_stringshare_ref(file);

	em->images = eina_list_append(em->images, file);
	ethumb_client_file_set(tb->ec, file, NULL);
	if (!ethumb_client_thumb_exists(tb->ec))
	{
		ethumb_client_generate(tb->ec, _ephoto_thumbnail_generated, tb, NULL);
	}
	else
	{
		ethumb_client_thumb_path_get(tb->ec, &thumb, NULL);
		_ephoto_thumbnail_generated(tb, tb->ec, 0, file, NULL,
					    thumb, NULL, EINA_TRUE);
	}
}

/* Start a thread to list images in a directory without locking the interface */
void
ephoto_populate_thumbnails(Evas_Object *obj)
{
        Ephoto_Thumb_Browser *tb = evas_object_data_get(obj, "thumb_browser");

	if (!tb->current_directory) return ;

	if (!efreet_mime_init())
		fprintf(stderr, "Could not init efreet_mime!\n");

	tb->list = eio_file_ls(tb->current_directory,
			   _ephoto_populate_filter,
			   _ephoto_populate_main,
			   _ephoto_populate_end,
			   _ephoto_populate_error,
			   tb);
}

/*Change the thumbnail size*/
static void
_ephoto_slider_changed(void *data, Evas_Object *obj, void *event)
{
	int val;
        Ephoto_Thumb_Browser *tb = data;

	val = elm_slider_value_get(tb->thumb_slider);
	elm_gengrid_item_size_set(tb->thumb_browser, val, val / THUMB_RATIO);

        em->config->thumb_size = val;
        ephoto_config_save(em, EINA_FALSE);
}

/*Callback when the client is connected*/
static void _ephoto_thumber_connected(void *data, Ethumb_Client *client, Eina_Bool success)
{
        Ephoto_Thumb_Browser *tb = data;
	if (success == EINA_TRUE)
	{
		ethumb_client_fdo_set(tb->ec, ETHUMB_THUMB_LARGE);
	        ethumb_client_format_set(tb->ec, ETHUMB_THUMB_FDO);
	        ethumb_client_aspect_set(tb->ec, ETHUMB_THUMB_KEEP_ASPECT);
		ephoto_populate_thumbnails(tb->layout);
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
        Ephoto_Thumb_Browser *tb = data;
	if (success)
	{
		Ephoto_Thumb_Data *etd;

		etd = calloc(1, sizeof(*etd));
		etd->thumb_path = eina_stringshare_add(thumb_path);
		etd->file = eina_stringshare_add(file);

		elm_gengrid_item_append(tb->thumb_browser, &tb->eg, etd, NULL, NULL);
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
		thumb = elm_layout_add(obj);
		elm_layout_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
				"/ephoto/thumb");
		evas_object_size_hint_weight_set(thumb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_show(thumb);

		o = elm_bg_add(thumb);
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
	
	/* evas_object_smart_callback_call(ef->flow_browser, "selected", file); */

	/* em->flow_browser = ephoto_create_flow_browser(em->layout); */
	/* ephoto_flow_browser_image_set(em->flow_browser, file); */
	/* elm_layout_content_set(em->layout, "ephoto.content.swallow", em->flow_browser); */
}

/*Check to see if the thumbnail was double clicked*/
static void 
_ephoto_thumb_clicked(void *data, Evas_Object *obj, void *event_info)
{
	const Eina_List *selected;
	Ephoto_Thumb_Data *etd;
	Evas_Object *o;
	Ephoto_Thumb_Browser *tb = data;

	selected = elm_gengrid_selected_items_get(tb->thumb_browser);
	o = eina_list_data_get(selected);
	etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get((Elm_Gengrid_Item *)o);
	evas_object_smart_callback_call(tb->layout, "selected", (char *)etd->file);
	//	ecore_job_add(_ephoto_thumb_clicked_job, etd->file);
}

/*File Selector is shown*/
static void
_ephoto_fileselector_shown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
        Ephoto_Thumb_Browser *tb = data;
	elm_fileselector_path_set(obj, tb->current_directory);
	evas_render(em->e);
}

/*Directory has been chosen*/
static void
_ephoto_directory_chosen(void *data, Evas_Object *obj, void *event_info)
{
	const Eina_List *l, *iter;
	Elm_Gengrid_Item *item;
	Ephoto_Thumb_Data *etd;
	const char *directory;
        Ephoto_Thumb_Browser *tb = data;

	directory = elm_fileselector_selected_get(obj);

	if ((directory) && (eina_stringshare_replace(&tb->current_directory, directory)))
	{
		l = elm_gengrid_items_get(tb->thumb_browser);
                EINA_LIST_FOREACH(l, iter, item)
                {
                        etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get(item);
                        eina_stringshare_del(etd->thumb_path);
                        eina_stringshare_del(etd->file);
                        free(etd);
                }
		elm_gengrid_clear(tb->thumb_browser);
		eina_list_free(em->images);
		em->images = NULL;
		ephoto_populate_thumbnails(tb->layout);
		elm_label_label_set(tb->dir_label, tb->current_directory);

		if (tb->current_directory)
		{
			char *buffer;
			int length;

			length = strlen(tb->current_directory) + strlen("Ephoto - ") + 1;
			buffer = alloca(length);
			snprintf(buffer, length, "Ephoto - %s", tb->current_directory);
			elm_win_title_set(em->win, buffer);
		} else {
			elm_win_title_set(em->win, "Ephoto");
		}
	}

	evas_object_smart_callback_call(tb->layout, "directory,changed", (char *)tb->current_directory);

	evas_object_del(tb->fsel_win);
	elm_toolbar_item_unselect_all(tb->toolbar);
}

/*Show the flow browser*/
static void 
_ephoto_view_large(void *data, Evas_Object *obj, void *event_info)
{
	const Eina_List *selected;
	Evas_Object *o;
	Ephoto_Thumb_Data *etd;
        Ephoto_Thumb_Browser *tb = data;

	selected = elm_gengrid_selected_items_get(tb->thumb_browser);
	
	/* em->flow_browser = ephoto_create_flow_browser(em->layout); */

	if (eina_list_data_get(selected))
	{
		o = eina_list_data_get(selected);
		etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get((Elm_Gengrid_Item *)o);
		/* _ephoto_thumb_clicked_job(etd->file); */
		evas_object_smart_callback_call(tb->layout, "selected", (char *)etd->file);

	}
	else
	{
                /* _ephoto_thumb_clicked_job( eina_list_data_get(em->images)); */
	  evas_object_smart_callback_call(tb->layout, "selected", eina_list_data_get(em->images));
	
	}
	
	elm_toolbar_item_unselect_all(tb->toolbar);
}

/*Change directory*/
static void
_ephoto_change_directory(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *fsel;
        Ephoto_Thumb_Browser *tb = data;

	tb->fsel_win = elm_win_inwin_add(em->win);
	
	fsel = elm_fileselector_add(tb->fsel_win);
	elm_fileselector_folder_only_set(fsel, EINA_TRUE);
	elm_fileselector_buttons_ok_cancel_set(fsel, EINA_TRUE);
	elm_fileselector_path_set(fsel, tb->current_directory);
	evas_object_event_callback_add(fsel, EVAS_CALLBACK_SHOW,
	 				_ephoto_fileselector_shown, tb);
	evas_object_smart_callback_add(fsel, "done", 
					_ephoto_directory_chosen, tb);

	elm_win_inwin_content_set(tb->fsel_win, fsel);
	elm_win_inwin_activate(tb->fsel_win);

	evas_object_show(fsel);
	evas_object_show(tb->fsel_win);
}

/*Show slideshow*/
static void
_ephoto_view_slideshow(void *data, Evas_Object *obj, void *event_info)
{ 
        Ephoto_Thumb_Browser *tb = data;
        /* FIXME */
        //ephoto_hide_thumb_browser();
	ephoto_show_slideshow(0, NULL);
	elm_toolbar_item_unselect_all(tb->toolbar);
}

/*Destroy the thumbnail browser object itself*/
static void
_ephoto_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
        Ephoto_Thumb_Browser *tb = data;
	const Eina_List *l, *iter;
	Elm_Gengrid_Item *item;
	Ephoto_Thumb_Data *etd;
	
	l = elm_gengrid_items_get(tb->thumb_browser);
	EINA_LIST_FOREACH(l, iter, item)
	{
	        etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get(item);
	        eina_stringshare_del(etd->thumb_path);
		eina_stringshare_del(etd->file);
		free(etd);
	}
	eina_stringshare_del(tb->current_directory);
	ethumb_client_disconnect(tb->ec);
}
