#include "ephoto.h"

#define SLIDER_MAX 300
#define SLIDER_MIN 80

#define PARENT_DIR "Up"

typedef struct _Ephoto_Thumb_Data Ephoto_Thumb_Data;
typedef struct _Ephoto_Thumb_Browser Ephoto_Thumb_Browser;

struct _Ephoto_Thumb_Data
{
   const char *thumb_path;
   const char *file;
   const char *basename;
   Eina_Bool is_directory;
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
        Elm_Toolbar_Item *view_large;
        Eio_File *list;
};

#define THUMB_RATIO (256 / 192)

/*Callbacks*/
static void _ephoto_slider_changed(void *data, Evas_Object *obj, void *event_info);
static char *_ephoto_get_label(void *data, Evas_Object *obj, const char *part);
static Evas_Object *_ephoto_get_icon(void *data, Evas_Object *obj, const char *part);
static Eina_Bool _ephoto_get_state(void *data, Evas_Object *obj, const char *part);
static void _ephoto_grid_del(void *data, Evas_Object *obj);
static void _ephoto_thumb_clicked_job(void *data);
static void _ephoto_thumb_clicked(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_thumb_selected(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_view_large(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_change_directory_window(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_view_slideshow(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_preferences(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_zoom_in(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_zoom_out(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_zoom_regular_size(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_thumbnail_add(Ephoto_Thumb_Browser *tb, const char *path, Eina_Bool is_dir);
static void _ephoto_change_directory(Ephoto_Thumb_Browser *tb, const char *directory);
static void _ephoto_populate_images(Ephoto_Thumb_Browser *tb);

/*A key has been pressed*/
static const struct
{
	const char *name;
	const char *modifiers;
	void (*func)(void *data, Evas_Object *obj, void *event_info);
} keys[] = {
	{ "F5", NULL, _ephoto_view_slideshow },
	{ "d", "Control",  _ephoto_change_directory_window},
	{ "p", "Control",  _ephoto_preferences},
	{ "plus", "Control", _ephoto_zoom_in},
	{ "minus", "Control", _ephoto_zoom_out},
	{ "0", "Control", _ephoto_zoom_regular_size},
	{ NULL, NULL, NULL }
};

static void
_ephoto_key_pressed(void *data, Evas *e, Evas_Object *obj, void *event_data)
{
	Evas_Event_Key_Down *eku;
	int i;

	eku = (Evas_Event_Key_Down *)event_data;
        DBG("Key name: %s", eku->key);
	for (i = 0; keys[i].name; ++i)
		if ((!strcmp(eku->key, keys[i].name)) &&
                    ((keys[i].modifiers == NULL) || (evas_key_modifier_is_set(eku->modifiers, keys[i].modifiers))))
			keys[i].func(data, obj, NULL);
}

static void
_ephoto_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_data)
{
	Evas_Event_Mouse_Wheel *emw = (Evas_Event_Mouse_Wheel *) event_data;
	if (evas_key_modifier_is_set(emw->modifiers, "Control"))
	{
		if (emw->z < 0)
			_ephoto_zoom_in(data, NULL, NULL);
		else
			_ephoto_zoom_out(data, NULL, NULL);
	}
}

static void
_ephoto_set_title(const char *file)
{
	char *buffer;
	int length;

	length = strlen(file) + strlen("Ephoto - ") + 1;
	buffer = alloca(length);
	snprintf(buffer, length, "Ephoto - %s", file);
	elm_win_title_set(em->win, buffer);
}

/*Create the thumbnail browser object*/
Evas_Object *
ephoto_create_thumb_browser(Evas_Object *parent)
{
	Evas_Object *o;
        Ephoto_Thumb_Browser *tb;

        tb = calloc(1, sizeof(Ephoto_Thumb_Browser));

	elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/themes/default/ephoto.edj");

	tb->layout = elm_layout_add(parent);
	elm_layout_file_set(tb->layout, 
			    PACKAGE_DATA_DIR "/themes/default/ephoto.edj", 
			    "ephoto/browser/layout");

	evas_object_size_hint_weight_set(tb->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_fill_set(tb->layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(tb->layout);

        tb->thbox = elm_box_add(tb->layout);
        elm_box_horizontal_set(tb->thbox, EINA_TRUE);
        evas_object_size_hint_weight_set(tb->thbox, EVAS_HINT_EXPAND, EVAS_HINT_FILL);
        evas_object_size_hint_fill_set(tb->thbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_layout_content_set(tb->layout, "ephoto.location.swallow", tb->thbox);

        tb->dir_label = elm_label_add(tb->thbox);
        elm_label_label_set(tb->dir_label, em->config->directory);
        evas_object_size_hint_weight_set(tb->dir_label, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(tb->dir_label, 0.01, 0.5);
        elm_box_pack_end(tb->thbox, tb->dir_label);

        tb->thumb_slider = elm_slider_add(tb->thbox);
        elm_slider_label_set(tb->thumb_slider, "Thumb Size:");
        elm_slider_span_size_set(tb->thumb_slider, 100);
        elm_slider_min_max_set(tb->thumb_slider, SLIDER_MIN, SLIDER_MAX);
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
	evas_object_smart_callback_add(tb->thumb_browser, "selected", _ephoto_thumb_selected, tb);
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
        elm_toolbar_item_add(tb->toolbar, o, "Change Directory", _ephoto_change_directory_window, tb);

        o = elm_icon_add(tb->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/filter.png", NULL);
        elm_toolbar_item_add(tb->toolbar, o, "Filter", NULL, NULL);

        o = elm_icon_add(tb->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/view_presentation.png", NULL);
        tb->view_large = elm_toolbar_item_add(tb->toolbar, o, "View Large", _ephoto_view_large, tb);

        o = elm_icon_add(tb->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/play_slideshow.png", NULL);
        elm_toolbar_item_add(tb->toolbar, o, "Play Slideshow", _ephoto_view_slideshow, tb);

        o = elm_icon_add(tb->toolbar);
        elm_icon_file_set(o, PACKAGE_DATA_DIR "/images/preferences.png", NULL);
        elm_toolbar_item_add(tb->toolbar, o, "Preferences", _ephoto_preferences, tb);

	tb->eg.item_style = "ephoto";
	tb->eg.func.label_get = _ephoto_get_label;
	tb->eg.func.icon_get = _ephoto_get_icon;
	tb->eg.func.state_get = _ephoto_get_state;
	tb->eg.func.del = _ephoto_grid_del;
        
        evas_object_data_set(tb->layout, "thumb_browser", tb);
        evas_object_event_callback_add(tb->layout, EVAS_CALLBACK_DEL, _ephoto_del_cb, tb);
        evas_object_event_callback_add(tb->layout, EVAS_CALLBACK_SHOW, _ephoto_show_cb, tb);
        evas_object_event_callback_add(tb->layout, EVAS_CALLBACK_KEY_DOWN,
                                       _ephoto_key_pressed, tb);
        evas_object_event_callback_add(tb->layout, EVAS_CALLBACK_MOUSE_WHEEL,
                                       _ephoto_mouse_wheel, tb);
        evas_object_focus_set(tb->layout, 1);
		
	ephoto_populate_thumbnails(tb->layout);
	_ephoto_set_title(em->config->directory);
		
	return tb->layout;
}


/*Show the thumbnail browser*/
static void
_ephoto_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)

{
        /* Ephoto_Thumb_Browser *tb = evas_object_data_get(obj, "thumb_browser"); */

	/* evas_object_show(tb->toolbar); */
	/* evas_object_show(tb->thumb_browser); */
	/* evas_object_show(tb->dir_label); */
	/* evas_object_show(tb->thumb_slider); */
	/* evas_object_show(tb->thbox); */
	_ephoto_set_title(em->config->directory);
}

/* Use ecore thread facility to avoid lock completly */

/* Check directory type from another thread */
static Eina_Bool
_ephoto_populate_filter_directory(void *data, const char *file)
{
	const char *type;

	if (!(type = efreet_mime_type_get(file)))
		return EINA_FALSE;
	if (!strncmp(type, "inode/directory", 15))
		return EINA_TRUE;

	return EINA_FALSE;
}

/* Check image type from another thread */
static Eina_Bool
_ephoto_populate_filter_image(void *data, const char *file)
{
   const char *type, *basename;

   /* TODO: eio_file_ls_direct() and get more useful parameter than file */
   basename = ecore_file_file_get(file);
   if ((!basename) || (basename[0] == '.'))
     return EINA_FALSE;

   /* TODO: speed up case for jpg/jpeg/png */
   if (!(type = efreet_mime_type_get(file)))
     return EINA_FALSE;
   if (!strncmp(type, "image", 5))
     return EINA_TRUE;

   return EINA_FALSE;
}

/*Done populating directories*/
static void
_ephoto_populate_end_directory(void *data)
{
	Ephoto_Thumb_Browser *tb = (Ephoto_Thumb_Browser *)data;
	_ephoto_populate_images(tb);
}

/*Done populating images*/
static void
_ephoto_populate_end_image(void *data)
{
        Ephoto_Thumb_Browser *tb = (Ephoto_Thumb_Browser *)data;

        tb->list = NULL;
        efreet_mime_shutdown();

        if (em->config->sort_images)
        {
                Eina_List *l;
                const char *file;
                em->images = eina_list_sort(em->images,
                                            eina_list_count(em->images),
                                            EINA_COMPARE_CB(strcoll));
                EINA_LIST_FOREACH(em->images, l, file)
		{
		        _ephoto_thumbnail_add(tb, file, EINA_FALSE);
		}
        }
}

static void
_ephoto_populate_error(int error, void *data)
{
        Ephoto_Thumb_Browser *tb = (Ephoto_Thumb_Browser*)data;
	/* We don't handle error case in ephoto */
	tb->list = NULL;

	efreet_mime_shutdown();
}

/* Build the interface component after detection from main thread */
static void
_ephoto_populate_main_directory(void *data, const char *file)
{
	Ephoto_Thumb_Browser *tb = (Ephoto_Thumb_Browser*)data;
	_ephoto_thumbnail_add(tb, file, EINA_TRUE);
}

/* Build the interface component after detection from main thread */
static void
_ephoto_populate_main_image(void *data, const char *file)
{
	Ephoto_Thumb_Browser *tb = (Ephoto_Thumb_Browser*)data;
	const char *type;

	file = eina_stringshare_ref(file);

        em->images = eina_list_append(em->images, file);
        if (em->config->sort_images) return;
	
	if (!(type = efreet_mime_type_get(file)))
		return;
	if (!strncmp(type, "image", 5))
	{
		em->images = eina_list_append(em->images, file);

		_ephoto_thumbnail_add(tb, file, EINA_FALSE);
	}
	else
	{
	  _ephoto_thumbnail_add(tb, file, EINA_TRUE);
	}


}

/*Create a thread to populate images*/
static void
_ephoto_populate_images(Ephoto_Thumb_Browser *tb)
{
	tb->list = eio_file_ls(em->config->directory,
			   _ephoto_populate_filter_image,
			   _ephoto_populate_main_image,
			   _ephoto_populate_end_image,
			   _ephoto_populate_error,
			   tb);
}

/* Start a thread to list images and directories in a directory without locking the interface */
void
ephoto_populate_thumbnails(Evas_Object *obj)
{
        Ephoto_Thumb_Browser *tb = evas_object_data_get(obj, "thumb_browser");

	if (!efreet_mime_init())
		fprintf(stderr, "Could not init efreet_mime!\n");

	_ephoto_thumbnail_add(tb, PARENT_DIR, EINA_TRUE);
	tb->list = eio_file_ls(em->config->directory,
			   _ephoto_populate_filter_directory,
			   _ephoto_populate_main_directory,
			   _ephoto_populate_end_directory,
			   _ephoto_populate_error,
			   tb);
}

/*Zoom out the thumbnail size*/
static void
_ephoto_zoom_out(void *data, Evas_Object *obj, void *event)
{
	int val;
	Ephoto_Thumb_Browser *tb = data;

	val = elm_slider_value_get(tb->thumb_slider);
	val -= (SLIDER_MAX + SLIDER_MIN) / 10;
	if (val > SLIDER_MAX)
		val = SLIDER_MAX;
	elm_slider_value_set(tb->thumb_slider, val);
	_ephoto_slider_changed(data, obj, event);
}

/*Zoom in the thumbnail size*/
static void
_ephoto_zoom_in(void *data, Evas_Object *obj, void *event)
{
	int val;
	Ephoto_Thumb_Browser *tb = data;

	val = elm_slider_value_get(tb->thumb_slider);
	val += (SLIDER_MAX + SLIDER_MIN) / 10;
	if (val > SLIDER_MAX)
		val = SLIDER_MAX;
	elm_slider_value_set(tb->thumb_slider, val);
	_ephoto_slider_changed(data, obj, event);

}

/*Zoom half the thumbnail size*/
static void
_ephoto_zoom_regular_size(void *data, Evas_Object *obj, void *event)
{
	int val;
	Ephoto_Thumb_Browser *tb = data;

	val = (SLIDER_MAX + SLIDER_MIN) / 2;
	elm_slider_value_set(tb->thumb_slider, val);
	_ephoto_slider_changed(data, obj, event);
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

/* Called when adding a directory or a file to elm_gengrid */
static void
_ephoto_thumbnail_add(Ephoto_Thumb_Browser *tb, const char *path, Eina_Bool is_dir)
{
	Ephoto_Thumb_Data *etd;

	etd = calloc(1, sizeof(*etd));
	etd->thumb_path = eina_stringshare_add(path);
	etd->file = eina_stringshare_add(path);
        etd->basename = ecore_file_file_get(etd->file);
	etd->is_directory = is_dir;
	elm_gengrid_item_append(tb->thumb_browser, &tb->eg, etd, NULL, NULL);
}

/*Get the label for the icon in the grid*/
static char *
_ephoto_get_label(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Thumb_Data *etd = data;
   return strdup(etd->basename);
}

/*Get the image for the icon in the grid*/
static Evas_Object *
_ephoto_get_icon(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Thumb_Data *etd = data;
   Evas_Object *thumb, *o;

   if (strcmp(part, "elm.swallow.icon") != 0) return NULL;

   thumb = elm_layout_add(obj);
   if (etd->is_directory)
     elm_layout_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
                         "/ephoto/thumb/no_border");
   else
     elm_layout_file_set(thumb, PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
                         "/ephoto/thumb");
   evas_object_size_hint_weight_set(thumb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(thumb);

   if (etd->is_directory)
     {
        o = ephoto_directory_thumb_add(thumb, etd->thumb_path);
     }
   else
     {
        o = elm_thumb_add(thumb);
        elm_thumb_file_set(o, etd->thumb_path, NULL);
     }
   evas_object_show(o);
   elm_layout_content_set(thumb, "ephoto.swallow.content", o);

   return thumb;
}

/*Get the state of the icon in the grid!*/
static Eina_Bool
_ephoto_get_state(void *data, Evas_Object *obj, const char *part)
{
	return EINA_FALSE;
}

/*Delete the grid*/
static void
_ephoto_grid_del(void *data, Evas_Object *obj)
{
	return;
}

/*Thumb clicked job*/
static void
_ephoto_thumb_clicked_job(void *data)
{
	const Eina_List *selected;
	char *parent_dir;
	Ephoto_Thumb_Data *etd;
	Evas_Object *o;
	Ephoto_Thumb_Browser *tb = data;

	selected = elm_gengrid_selected_items_get(tb->thumb_browser);
	o = eina_list_data_get(selected);
	etd = (Ephoto_Thumb_Data *)elm_gengrid_item_data_get((Elm_Gengrid_Item *)o);
	if (etd->is_directory)
	{
		if (!strcmp(etd->file, PARENT_DIR))
		{
			parent_dir = ecore_file_dir_get(em->config->directory);
			_ephoto_change_directory(tb, parent_dir);
			free(parent_dir);
		}
		else
			_ephoto_change_directory(tb, etd->file);
	}
	else
		evas_object_smart_callback_call(tb->layout, "selected", (char *)etd->file);
}

/* Change the current directory showed in thumb browser. */
static void
_ephoto_change_directory(Ephoto_Thumb_Browser *tb, const char *directory)
{
	const Eina_List *l, *iter;
	Elm_Gengrid_Item *item;
	Ephoto_Thumb_Data *etd;

	if ((directory) && (eina_stringshare_replace(&em->config->directory, directory)))
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
		elm_label_label_set(tb->dir_label, em->config->directory);
		_ephoto_set_title(em->config->directory);
	}

	evas_object_smart_callback_call(tb->layout, "directory,changed", (char *) em->config->directory);
}

/*Called when an item is selected in gengrid*/
static void
_ephoto_thumb_selected(void *data, Evas_Object *obj, void *event_info)
{
	Ephoto_Thumb_Browser *tb = data;
	Ephoto_Thumb_Data *etd = elm_gengrid_item_data_get(event_info);
	if (etd->is_directory)
		elm_toolbar_item_disabled_set(tb->view_large, EINA_TRUE);
	else
		elm_toolbar_item_disabled_set(tb->view_large, EINA_FALSE);
}

/*Check to see if the thumbnail was double clicked*/
static void 
_ephoto_thumb_clicked(void *data, Evas_Object *obj, void *event_info)
{
	ecore_job_add(_ephoto_thumb_clicked_job, data);
}

/*File Selector is shown*/
static void
_ephoto_fileselector_shown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	elm_fileselector_path_set(obj, em->config->directory);
	evas_render(em->e);
}

/*Directory has been chosen*/
static void
_ephoto_directory_chosen(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto_Thumb_Browser *tb = data;

	_ephoto_change_directory(tb, elm_fileselector_selected_get(obj));
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
_ephoto_change_directory_window(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *fsel;
        Ephoto_Thumb_Browser *tb = data;

	tb->fsel_win = elm_win_inwin_add(em->win);
	
	fsel = elm_fileselector_add(tb->fsel_win);
	elm_fileselector_folder_only_set(fsel, EINA_TRUE);
	elm_fileselector_buttons_ok_cancel_set(fsel, EINA_TRUE);
	elm_fileselector_path_set(fsel, em->config->directory);
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

/* Show the preferences window */
static void
_ephoto_preferences(void *data, Evas_Object *obj, void *event_info)
{ 
        Ephoto_Thumb_Browser *tb = data;

        ephoto_show_preferences(em);
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
	em->thumb_browser = NULL;
}
