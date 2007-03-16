#include "ephoto.h" 

#ifdef BUILD_EXIF_SUPPORT
static const char *get_image(void);
static void close_dialog(Ewl_Widget *w, void *event, void *data);
static void add_exif_to_container(Ewl_Widget *w, void *event, void *data);

/*Get the Exif Data for an image and return it in a hash*/
Ecore_Hash *get_exif_data(const char *file)
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
	ecore_hash_set_free_key(exif_info, free);
	ecore_hash_set_free_value(exif_info, free);
	
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
				exif_entry_get_value(entry, value, sizeof(value));
				ecore_hash_set(exif_info, strdup(title), strdup(value));
				exif_entry_unref(entry);
			}
		}
	}
	exif_data_unref(data);
	
	return exif_info;
}

/*Get the current image*/
static const char *get_image(void)
{
	const char *img = NULL;

        if (VISIBLE(em->fbox_vbox) && em->currentf)
        {
                img = ewl_widget_name_get(em->currentf);
        }
        else if (VISIBLE(em->list_vbox) && em->currentl)
        {
                img = ewl_widget_name_get(em->currentl);
        }
        else if (VISIBLE(em->edit_vbox))
        {
                img = ewl_image_file_path_get(EWL_IMAGE(em->eimage));
        }
	if(!img) return NULL;
	return strdup(img);
}

/*Close a dialog*/
static void close_dialog(Ewl_Widget *w, void *event, void *data)
{
	Ecore_List *destroy;
	Ewl_Widget *win;

	win = data;
	destroy = ewl_widget_data_get(win, "list");
	ecore_list_destroy(destroy);
	ewl_widget_destroy(win);

	return;
}

/*Add all the exif information to the container w*/
static void add_exif_to_container(Ewl_Widget *w, void *event, void *data)
{
	const char *img;
	char *key, *value;
	char text[PATH_MAX];
	Ecore_Hash *exif_info;
	Ecore_List *keys, *values;
	Ewl_Widget *win;

	win = data;

	values = ecore_list_new();
	ecore_list_set_free_cb(values, free);

        img = get_image();
	if(!img) return;
	exif_info = get_exif_data(img);

        if(ecore_hash_count(exif_info) == 0)
        {
		ecore_list_append(values, strdup("No Exif Data Exists"));
        }
	else
	{
		keys = ecore_hash_keys(exif_info);
		while (!ecore_list_is_empty(keys))
		{
			key = ecore_list_remove_first(keys);
	                value = ecore_hash_get(exif_info, key);
	
	                snprintf(text, PATH_MAX, "%s: %s", key, value);
			ecore_list_append(values, strdup(text));
		}
	}
	ewl_mvc_data_set(EWL_MVC(w), values);
	ewl_widget_data_set(win, "list", values);
	ecore_hash_destroy(exif_info);
		
	return;
}

/*Display a dialog which will display exif data*/
void display_exif_dialog(Ewl_Widget *w, void *event, void *data)
{
	const char *img;
	Ewl_Widget *win, *vbox, *image, *sp, *list, *text;
	Ewl_Model *model;
	Ewl_View *view;
	
	img = get_image();
	if(!img)
	{
		return;
	}

	win = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(win), "Ephoto Exif Info");
        ewl_window_name_set(EWL_WINDOW(win), "Ephoto Exif Info");
	ewl_window_dialog_set(EWL_WINDOW(win), 1);
        ewl_object_size_request(EWL_OBJECT(win), 310, 460);
        ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, close_dialog, win);
	ewl_widget_show(win);

	vbox = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(win), vbox);
	ewl_widget_show(vbox);

	text = add_text(vbox, "Preview");

	image = add_image(vbox, img, 1, NULL, NULL);
	ewl_image_constrain_set(EWL_IMAGE(image), 120);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_SHRINK);

	text = add_text(vbox, "Image Information");

	sp = ewl_scrollpane_new();
	ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vbox), sp);
	ewl_widget_show(sp);
	
	model = ewl_model_ecore_list_get();
	view = ewl_label_view_get();

	list = ewl_list_new();
	ewl_mvc_model_set(EWL_MVC(list), model);
	ewl_mvc_view_set(EWL_MVC(list), view);
	ewl_object_fill_policy_set(EWL_OBJECT(list), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(sp), list);
        ewl_callback_append(list, EWL_CALLBACK_SHOW, add_exif_to_container, win);
        ewl_widget_show(list);

	add_button(vbox, "Close", PACKAGE_DATA_DIR "/images/dialog-close.png", close_dialog, win);
	
	return;
}
#endif
