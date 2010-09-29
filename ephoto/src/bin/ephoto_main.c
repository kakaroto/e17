#include "ephoto.h"

/*Ephoto Main Global*/
Ephoto *em;

/*Inline Callbacks*/
static void _ephoto_delete_main_window(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_flow_browser_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_thumb_browser_selected_cb(void *data, Evas_Object *obj, void *event_info);

/* Objects Callbacks */
static void 
_ephoto_flow_browser_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
        em->thumb_browser = ephoto_create_thumb_browser(em->layout);
	elm_layout_content_set(em->layout, "ephoto.content.swallow", em->thumb_browser);
	evas_object_smart_callback_add(em->thumb_browser, 
				       "selected",
				       _ephoto_thumb_browser_selected_cb,
				       NULL);
	em->state = EPHOTO_STATE_THUMB;
	em->flow_browser = NULL;
}

static void 
_ephoto_thumb_browser_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
        const char *file = event_info;

	em->flow_browser = ephoto_create_flow_browser(em->layout);
	ephoto_flow_browser_image_set(em->flow_browser, file);
	elm_layout_content_set(em->layout, "ephoto.content.swallow", em->flow_browser);
	evas_object_smart_callback_add(em->flow_browser, 
				       "delete,request",
				       _ephoto_flow_browser_delete_cb,
				       NULL);
	em->state = EPHOTO_STATE_FLOW;
}

/*Create the main ephoto window*/
void 
ephoto_create_main_window(const char *directory, const char *image)
{
        char current_directory[PATH_MAX];

	em = calloc(1, sizeof(Ephoto));
        if (!ephoto_config_init(em))
                _ephoto_delete_main_window(NULL, NULL, NULL);

	/*Setup the main window*/
	em->win = elm_win_add(NULL, "ephoto", ELM_WIN_BASIC);
	elm_win_title_set(em->win, "Ephoto");

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

	/* Add the main layout to the window */
	em->layout = elm_layout_add(em->win);
	elm_layout_file_set(em->layout,
			    PACKAGE_DATA_DIR "/themes/default/ephoto.edj",
			    "ephoto/main/layout");
	elm_win_resize_object_add(em->win, em->layout);
	evas_object_size_hint_weight_set(em->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_fill_set(em->layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(em->layout);


        /* Prepare the slideshow beforehand, in order
         * to obtain the list of transitions */
	ephoto_create_slideshow();

        if (!directory)
        {
                getcwd(current_directory, PATH_MAX);
        }

        if (!em->config->directory)
        {
                if (directory)
                        em->config->directory = eina_stringshare_add(directory);
                else
                        em->config->directory = eina_stringshare_add(current_directory);
        }
        else if (directory || !em->config->remember_directory)
        {
                if (directory)
                        eina_stringshare_replace(&em->config->directory,
                                                 directory);
                else
                        eina_stringshare_replace(&em->config->directory,
                                                 current_directory);
        }

	if (image)
	{
                em->flow_browser = ephoto_create_flow_browser(em->layout);
		ephoto_flow_browser_image_set(em->flow_browser, image);
		eina_stringshare_del(image);	
		elm_layout_content_set(em->layout, "ephoto.content.swallow", em->flow_browser);
		evas_object_smart_callback_add(em->flow_browser, 
					       "delete,request",
					       _ephoto_flow_browser_delete_cb,
					       NULL);
		em->state = EPHOTO_STATE_FLOW;
	}
	else
        {
                em->thumb_browser =  ephoto_create_thumb_browser(em->layout);
		evas_object_show(em->thumb_browser);
		elm_layout_content_set(em->layout, "ephoto.content.swallow", em->thumb_browser);
		evas_object_smart_callback_add(em->thumb_browser, 
					       "selected",
					       _ephoto_thumb_browser_selected_cb,
					       NULL);
		em->state = EPHOTO_STATE_THUMB;
	}

	evas_object_resize(em->win, 1024, 534);
}

/*Delete the main ephoto window*/
static void
_ephoto_delete_main_window(void *data, Evas_Object *obj, void *event_info)
{
        ephoto_config_save(em, EINA_TRUE);

        if (em->thumb_browser)
                evas_object_del(em->thumb_browser);
        if (em->flow_browser)
	        evas_object_del(em->flow_browser);

	evas_object_del(em->layout);
	evas_object_del(em->bg);
	if (em->images)
		eina_list_free(em->images);
        ephoto_config_free(em);
	free(em);
	elm_exit();
}

