#include "ephoto.h"

/*Ephoto Main Global*/
Ephoto *em;

/*Inline Callbacks*/
static void _ephoto_delete_main_window(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_flow_browser_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_thumb_browser_selected_cb(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_thumb_browser_directory_changed_cb(void *data, Evas_Object *obj, void *event_info);
/* Objects Callbacks */
static void 
_ephoto_flow_browser_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
  em->thumb_browser = ephoto_create_thumb_browser(em->layout, em->cur_directory);
  elm_layout_content_set(em->layout, "ephoto.content.swallow", em->thumb_browser);
  evas_object_smart_callback_add(em->thumb_browser, 
				 "selected",
				 _ephoto_thumb_browser_selected_cb,
				 NULL);
 evas_object_smart_callback_add(em->thumb_browser, 
				 "directory,changed",
				 _ephoto_thumb_browser_directory_changed_cb,
				 NULL);

  em->state = EPHOTO_STATE_THUMB;
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

static void 
_ephoto_thumb_browser_directory_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  const char *dir = event_info;

  eina_stringshare_del(em->cur_directory);
  em->cur_directory = eina_stringshare_add(dir);
  printf("Directory changed : %s\n", em->cur_directory);
}

/*Create the main ephoto window*/
void 
ephoto_create_main_window(const char *directory, const char *image)
{
	em = calloc(1, sizeof(Ephoto));

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


	if (image)
	{
                em->flow_browser = ephoto_create_flow_browser(em->layout);
		ephoto_flow_browser_image_set(em->flow_browser, image);
		em->cur_directory = eina_stringshare_add(ecore_file_dir_get(image));
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
                em->thumb_browser =  ephoto_create_thumb_browser(em->layout, directory);
		em->cur_directory = eina_stringshare_add(directory);
		evas_object_show(em->thumb_browser);
		elm_layout_content_set(em->layout, "ephoto.content.swallow", em->thumb_browser);
		evas_object_smart_callback_add(em->thumb_browser, 
					       "selected",
					       _ephoto_thumb_browser_selected_cb,
					       NULL);
		evas_object_smart_callback_add(em->thumb_browser, 
					       "directory,changed",
					       _ephoto_thumb_browser_directory_changed_cb,
					       NULL);
		em->state = EPHOTO_STATE_THUMB;
	}

	evas_object_resize(em->win, 1024, 534);
}

/*Delete the main ephoto window*/
static void
_ephoto_delete_main_window(void *data, Evas_Object *obj, void *event_info)
{
        if (em->thumb_browser)
                evas_object_del(em->thumb_browser);
        if (em->flow_browser)
	        evas_object_del(em->flow_browser);

	evas_object_del(em->layout);
	evas_object_del(em->bg);
	if (em->images)
		eina_list_free(em->images);
	free(em);
	elm_exit();
}

