#include "ephoto.h" 

#ifdef BUILD_EXIF_SUPPORT
static const char *get_image(void);
static void list_is_destroyed(Ewl_Widget *w, void *event, void *data);

Ecore_Hash *
get_exif_data(const char *file)
{
	const char **args, *title;
	char value[1024];
	unsigned int i, tag;
	Ecore_Hash *exif_info;
        ExifEntry *entry;
        ExifData *data;
        ExifLoader *loader;

	args = calloc(1, sizeof(const char *));
	args[0] = file;

	exif_info = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ecore_hash_free_key_cb_set(exif_info, free);
	ecore_hash_free_value_cb_set(exif_info, free);
	
	loader = exif_loader_new();
	exif_loader_write_file(loader, *args);

	data = exif_loader_get_data(loader);
	if (!data)
	{
		exif_loader_unref(loader);
		return NULL;
	}
	exif_loader_unref(loader);

	for(tag = 0; tag < 0xffff; tag++)
	{
		title = exif_tag_get_title(tag);
		for (i = 0; i < EXIF_IFD_COUNT; i++)
		{
			entry = exif_content_get_entry(data->ifd[i], tag);
			if (entry)
			{
				exif_entry_ref(entry);
				exif_entry_get_value(entry, value, 
							sizeof(value));
				ecore_hash_set(exif_info, strdup(title), 
							  strdup(value));
				exif_entry_unref(entry);
			}
		}
	}
	exif_data_unref(data);
	
	return exif_info;
}

static const char *
get_image(void)
{
	const char *img = NULL;

        img = ecore_dlist_current(em->images);
	if(!img) return NULL;
	return strdup(img);
}

static void
list_is_destroyed(Ewl_Widget *w, void *event, void *data)
{
	Ecore_List *destroy;

	destroy = ewl_widget_data_get(w, "list");
	ecore_list_destroy(destroy);
}

void 
display_exif(Ewl_Widget *w, void *event, void *data)
{
	const char *img;
	char *key, *value;
	char text[PATH_MAX];
	Ecore_Hash *exif_info;
	Ecore_List *keys, *values;

	values = ecore_list_new();
	ecore_list_free_cb_set(values, free);

        img = get_image();
	if(!img) return;
	exif_info = get_exif_data(img);

        if(ecore_hash_count(exif_info) == 0)
        {
		ecore_list_append(values, strdup(_("No Exif Data Exists")));
        }
	else
	{
		keys = ecore_hash_keys(exif_info);
		while (!ecore_list_empty_is(keys))
		{
			key = ecore_list_first_remove(keys);
	                value = ecore_hash_get(exif_info, key);
	
	                snprintf(text, PATH_MAX, "%s: %s", key, value);
			ecore_list_append(values, strdup(text));
		}
	}
	ewl_mvc_data_set(EWL_MVC(w), values);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, list_is_destroyed, NULL);
	ewl_widget_data_set(w, "list", values);
	ecore_hash_destroy(exif_info);
		
	return;
}

void 
add_exif_to_container(Ewl_Widget *c)
{
	const char *img;
	Ewl_Widget *sp, *list;
	Ewl_Model *model;
	Ewl_View *view;
	
	img = get_image();
	if(!img)
	{
		return;
	}

	sp = ewl_scrollpane_new();
	ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(c), sp);
	ewl_widget_show(sp);
	
	model = ewl_model_ecore_list_instance();
	view = ewl_label_view_get();

	list = ewl_list_new();
	ewl_mvc_model_set(EWL_MVC(list), model);
	ewl_mvc_view_set(EWL_MVC(list), view);
	ewl_object_fill_policy_set(EWL_OBJECT(list), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(sp), list);
        ewl_callback_append(list, EWL_CALLBACK_SHOW, display_exif, NULL);
        ewl_widget_show(list);
	
	return;
}
#endif
