#include "ephoto.h"

static void image_clicked(void *data, Evas *e, Evas_Object *obj, void *event_info);

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
	Evas_Object *obj;
	int visiblew;
	int visibleh;
	int paddingw;
	int paddingh;
	int tw;
	int th;
	int x;
	int y;
	int w;
	int h;
	int current_head;
	int current_page;
	int item_w, item_h;
	int items_per_page;
	int total_pages;
	int visible_items;
	Eina_List *items;
	Eina_List *images;
};

static Evas_Smart *_smart = NULL;

static void _table_smart_change_page(Smart_Data *sd, int direction);
static void _table_smart_reconfigure(Smart_Data *sd);
static void _table_smart_init(void);
static void _table_smart_add(Evas_Object *obj);
static void _table_smart_del(Evas_Object *obj);
static void _table_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _table_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _table_smart_show(Evas_Object *obj);
static void _table_smart_hide(Evas_Object *obj);
static void _table_smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _table_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _table_smart_clip_unset(Evas_Object *obj);

static void move_left(void *data, Evas_Object *obj, const char *emission, const char *source);
static void move_right(void *data, Evas_Object *obj, const char *emission, const char *source);
static void connect_callback(void *data, Ethumb_Client *client, Eina_Bool success);
static void thumb_generated(void *data, Ethumb_Client *client, int id, const char *file, const char *key, 
                                        const char *thumb_path, const char *thumb_key,
                                        Eina_Bool success);

static void move_left(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	ephoto_table_prev_page(em->image_browser);
	printf("Left\n");
}

static void move_right(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	ephoto_table_next_page(em->image_browser);
	printf("Right\n");
}

static void connect_callback(void *data, Ethumb_Client *client, Eina_Bool success)
{
	printf("Connected to ethumb client: %d\n", success);

	ethumb_client_fdo_set(client, ETHUMB_THUMB_LARGE);
	ethumb_client_format_set(client, ETHUMB_THUMB_FDO);
	ethumb_client_aspect_set(client, ETHUMB_THUMB_KEEP_ASPECT);
	populate_thumbnails();
}

static void thumb_generated(void *data, Ethumb_Client *client, int id, const char *file, const char *key,
					const char *thumb_path, const char *thumb_key,
					Eina_Bool success)
{
	Evas_Imaging_Image *i;
	Evas_Object *img, *edje, *o;
	int tmiw, tmih, tmaw, tmah, w, h;
	
	if (success)
	{
		img = data;
		evas_object_event_callback_add(img, EVAS_CALLBACK_MOUSE_DOWN, image_clicked, NULL);

		edje = edje_object_add(em->e);
        	edje_object_file_set(edje, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/thumb/image");
        	edje_object_signal_emit(edje, "ephoto.thumb.visible", "ephoto");
		evas_object_show(edje);

		o = ephoto_image_add();
		evas_object_show(o);
		edje_object_part_swallow(edje, "ephoto.swallow.content", o);
		edje_object_part_swallow(img, "ephoto.swallow.content", edje);

		edje_object_size_min_get(edje, &tmiw, &tmih);
       		edje_object_size_max_get(edje, &tmaw, &tmah);

		i = evas_imaging_image_load(file, NULL);
		evas_imaging_image_size_get(i, &w, &h);

		if (w > 120 || h > 120)
		{
			ephoto_image_fill_inside_set(o, 0);
			ephoto_image_file_set(o, thumb_path, tmiw, tmih);
	        	evas_object_resize(o, tmiw, tmih);
       		 	evas_object_size_hint_min_set(o, tmiw, tmih);
        		evas_object_size_hint_max_set(o, tmiw, tmih);
			evas_object_resize(edje, tmiw, tmih);
                        evas_object_size_hint_min_set(edje, tmiw, tmih);
                        evas_object_size_hint_max_set(edje, tmiw, tmih);
		}
		else
		{
			ephoto_image_fill_inside_set(o, 0);	
			ephoto_image_file_set(o, thumb_path, tmiw, tmih);
        	        evas_object_resize(o, tmiw, tmih);
        	        evas_object_size_hint_min_set(o, tmiw, tmih);
	                evas_object_size_hint_max_set(o, tmiw, tmih);
			evas_object_resize(edje, tmiw, tmih);
                        evas_object_size_hint_min_set(edje, tmiw, tmih);
                        evas_object_size_hint_max_set(edje, tmiw, tmih);
		}
	}
}

static void image_clicked(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Eina_List *node;
	Evas_Event_Mouse_Down *ev;
	Smart_Data *sd;

	ev = event_info;
	sd = evas_object_smart_data_get(em->image_browser);

	if (ev->flags == EVAS_BUTTON_DOUBLE_CLICK)
	{
		edje_object_signal_emit(em->image_browser, "ephoto.browser.hidden", "ephoto");
		node = evas_object_data_get(obj, "image");
		hide_image_browser();
		show_flow_view(node, sd->images);
		return;
	}

	if (obj == em->sel)
		return;
	if (em->sel)
		edje_object_signal_emit(em->sel, "ephoto.thumb.deselected", "ephoto");
	edje_object_signal_emit(obj, "ephoto.thumb.selected", "ephoto");
	em->sel = obj;
}

Evas_Object *ephoto_table_add(Evas *e)
{
	_table_smart_init();
	return evas_object_smart_add(e, _smart);
}

void ephoto_table_padding_set(Evas_Object *obj, int paddingw, int paddingh)
{
	Smart_Data *sd;
	
	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	sd->paddingw = paddingw;
	sd->paddingh = paddingh;
	sd->items_per_page = (sd->visiblew / (sd->item_w+sd->paddingw)) * (sd->visibleh / (sd->item_h+sd->paddingh));
}

void ephoto_table_pack(Evas_Object *obj, char *image)
{
	Smart_Data *sd;
	Evas_Imaging_Image *i;
	Evas_Object *img;
	int w, h;
	const char *thumb;

	sd = evas_object_smart_data_get(obj);
	
	sd->images = eina_list_append(sd->images, strdup(image));

	if (sd->visible_items+1 > sd->items_per_page)
	{
		char text[PATH_MAX];
		int pages, remain;

		pages = eina_list_count(sd->images)/sd->items_per_page;
		remain = eina_list_count(sd->images)%sd->items_per_page;
		if (remain > 0)
			pages++;
		sd->total_pages = pages;
		snprintf(text, PATH_MAX, "Page: %d of %d", sd->current_page, sd->total_pages);
		edje_object_part_text_set(sd->obj, "ephoto.text.object", text);

		return;
	}
	
	img = edje_object_add(em->e);
	edje_object_file_set(img, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/thumb/shadow");
	evas_object_show(img);
	evas_object_move(img, sd->tw, sd->th);
	edje_object_signal_emit(img, "ephoto.thumb.visible", "ephoto");
	evas_object_data_set(img, "image", eina_list_nth_list(sd->images, eina_list_count(sd->images)-1));

	evas_object_resize(img, sd->item_w, sd->item_h);
        evas_object_size_hint_min_set(img, sd->item_w, sd->item_h);
        evas_object_size_hint_max_set(img, sd->item_w, sd->item_h);
	sd->visible_items++;

	i = evas_imaging_image_load(image, NULL);
	evas_imaging_image_size_get(i, &w, &h);
	if (w > 120 || h > 120)
	{
		ethumb_client_file_set(em->thumb_cli, image, NULL);
		if (!ethumb_client_thumb_exists(em->thumb_cli))
			ethumb_client_generate(em->thumb_cli, thumb_generated, img, NULL);
		else
                {
                	ethumb_client_thumb_path_get(em->thumb_cli, &thumb, NULL);
                        thumb_generated(img, em->thumb_cli, 0, image, NULL, thumb, NULL, EINA_TRUE);
                }
	}
	else
		thumb_generated(img, em->thumb_cli, 0, image, NULL, image, NULL, EINA_TRUE);
	
	evas_imaging_image_free(i);

        sd->tw += (sd->item_w + sd->paddingw);

	sd->items = eina_list_append(sd->items, img);

	if (sd->tw+(sd->item_w + sd->paddingw) >= sd->visiblew)
        {
                sd->th += (sd->item_h + sd->paddingh);
                sd->tw = 60;
        }
}	

void ephoto_table_viewport_set(Evas_Object *obj, int w, int h)
{
	Smart_Data *sd;
	char text[PATH_MAX];
	int pages, remain;

	sd = evas_object_smart_data_get(obj);
	if ((sd->visiblew == w) && (sd->visibleh == h))
		return;
	sd->visiblew = w;
	sd->visibleh = h;	
	sd->items_per_page = (sd->visiblew / (sd->item_w+sd->paddingw)) * (sd->visibleh / (sd->item_h+sd->paddingh));
	evas_object_resize(sd->obj, sd->visiblew, sd->visibleh);
	if (!eina_list_count(sd->images))
		return;

        pages = eina_list_count(sd->images)/sd->items_per_page;
        remain = eina_list_count(sd->images)%sd->items_per_page;
        if (remain > 0)
		pages++;
	if (pages <= sd->current_page)
		sd->total_pages = sd->current_page;
	else
		sd->total_pages = pages;
        snprintf(text, PATH_MAX, "Page: %d of %d", sd->current_page, sd->total_pages);
        edje_object_part_text_set(sd->obj, "ephoto.text.object", text);
	
	_table_smart_reconfigure(sd);
}

void ephoto_table_next_page(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	
	_table_smart_change_page(sd, 1);
}

void ephoto_table_prev_page(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);

        _table_smart_change_page(sd, 0);
}

static void _table_smart_change_page(Smart_Data *sd, int direction)
{
	Evas_Imaging_Image *im;
	Evas_Object *i, *img;
        char *image, text[PATH_MAX];
	const char *thumb;
	int w, h, j, vis, head, back, pages, remain;
	Eina_List *iterator, *iteratorb;

	pages = eina_list_count(sd->images)/sd->items_per_page;
        remain = eina_list_count(sd->images)%sd->items_per_page;
        if (remain > 0)
                pages++;
        sd->total_pages = pages;
	if (sd->current_page > sd->total_pages)
	{
		sd->current_page = sd->total_pages;
		iterator = eina_list_nth_list(sd->items, sd->current_head);
		iteratorb = eina_list_nth_list(sd->items, sd->current_head);
	}
	else if (direction && sd->current_page < sd->total_pages)
	{
		if (sd->current_page == 1)
		{
			iterator = sd->items;
                        iteratorb = sd->images;
		}
		else
		{
			iterator = eina_list_nth_list(sd->items, sd->current_head);
			iteratorb = eina_list_nth_list(sd->images, sd->current_head);
		}
		sd->current_page++;
	}
	else if (!direction && sd->current_page > 1)
	{
		if (sd->current_page == 1)
		{
			iterator = sd->items;
			iteratorb = sd->images;
		}
		else
		{
			iterator = eina_list_nth_list(sd->items, sd->current_head);
			iteratorb = eina_list_nth_list(sd->images, sd->current_head);
		}
		sd->current_page--;
	}
	else
		return;
	
	snprintf(text, PATH_MAX, "Page: %d of %d", sd->current_page, sd->total_pages);
        edje_object_part_text_set(sd->obj, "ephoto.text.object", text);

	for (j = 0; j <= sd->items_per_page; j++)
        {
		i = eina_list_data_get(iterator);

                if (evas_object_visible_get(i))
                {
                        evas_object_hide(i);
                        edje_object_signal_emit(i, "ephoto.thumb.hidden", "ephoto");
                }
		iterator = eina_list_next(iterator);
	}
	if (sd->current_page == 1)
	{
		iterator = sd->items;
                iteratorb = sd->images;
		sd->current_head = 0;
	}
	else
	{
		vis = eina_list_count(sd->images)-((sd->current_page-1)*sd->items_per_page);

		if (vis < sd->items_per_page)
		{
			back = sd->items_per_page-vis;
			while (back > eina_list_count(sd->images))
				back--;
			head = ((sd->current_page-1)*sd->items_per_page)-back;
			iterator = eina_list_nth_list(sd->items, head);
                        iteratorb = eina_list_nth_list(sd->images, head);
			sd->current_head = head;
		}
		else
		{
			sd->current_head = (sd->current_page-1)*sd->items_per_page;
			iterator = eina_list_nth_list(sd->items, sd->current_head);
                	iteratorb = eina_list_nth_list(sd->images, sd->current_head);
		}
       	}

	sd->tw = 60;
        sd->th = 75;
	sd->visible_items = 0;
	
	for(j =0; j <= sd->items_per_page && eina_list_data_get(iterator) != NULL; j++)
	{
		i = eina_list_data_get(iterator);

		evas_object_show(i);
		evas_object_move(i, sd->tw, sd->th);
		edje_object_signal_emit(i, "ephoto.thumb.visible", "ephoto");
		
		sd->visible_items++;

		sd->tw += (sd->item_w+sd->paddingw);

                if (sd->tw+(sd->item_w+sd->paddingw) >= sd->visiblew)
                {
                        sd->th += (sd->item_h+sd->paddingh);
       	        	sd->tw = 60;
                }
	
		iterator = eina_list_next(iterator);
		iteratorb = eina_list_next(iteratorb);
	}
	if (sd->visible_items < sd->items_per_page)
	{
		for (j=0; j <= sd->items_per_page && eina_list_data_get(iteratorb) != NULL; j++)
		{
			if (sd->visible_items+1 > sd->items_per_page)
				return;

			image = eina_list_data_get(iteratorb);

			img = edje_object_add(em->e);
			edje_object_file_set(img, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/thumb/shadow");
			evas_object_move(img, sd->tw, sd->th);
			evas_object_show(img);
			evas_object_event_callback_add(img, EVAS_CALLBACK_MOUSE_UP, image_clicked, NULL);
			edje_object_signal_emit(img, "ephoto.thumb.visible", "ephoto");
			evas_object_data_set(img, "image", iteratorb);

			evas_object_resize(img, sd->item_w, sd->item_h);
       			evas_object_size_hint_min_set(img, sd->item_w, sd->item_h);
       			evas_object_size_hint_max_set(img, sd->item_w, sd->item_h);
			sd->visible_items++;

			im = evas_imaging_image_load(image, NULL);
			evas_imaging_image_size_get(im, &w, &h);
			if (w > 120 || h > 120)
			{
				ethumb_client_file_set(em->thumb_cli, image, NULL);
				if (!ethumb_client_thumb_exists(em->thumb_cli))
                        		ethumb_client_generate(em->thumb_cli, thumb_generated, img, NULL);
                		else
                                {
                                        ethumb_client_thumb_path_get(em->thumb_cli, &thumb, NULL);
                                        thumb_generated(img, em->thumb_cli, 0, image, NULL, thumb, NULL, EINA_TRUE);
                                }				
			}
			else
				thumb_generated(img, em->thumb_cli, 0, image, NULL, image, NULL, EINA_TRUE);
	
			evas_imaging_image_free(im);

        		sd->tw += (sd->item_w + sd->paddingw);
        		
        		sd->items = eina_list_append(sd->items, img);

			if (sd->tw+(sd->item_w + sd->paddingw) >= sd->visiblew)
        		{
                		sd->th += (sd->item_h + sd->paddingh);
                		sd->tw = 60;
        		}
			iteratorb = eina_list_next(iteratorb);
		}
	}

	return;
}

static void _table_smart_reconfigure(Smart_Data *sd)
{
	Evas_Imaging_Image *im;
	Evas_Object *i, *img;
	char *image;
	const char *thumb;
	int w, h, j;
	Eina_List *iterator, *iteratorb;

	if (sd->current_page == 1)
	{
		iterator = sd->items;
                iteratorb = sd->images;
	}
	else
	{
		iterator = eina_list_nth_list(sd->items, sd->current_head);
                iteratorb = eina_list_nth_list(sd->images, sd->current_head);
       	}

	sd->tw = 60;
	sd->th = 75;

	sd->visible_items = 0;

	for(j =0; j <= sd->items_per_page && eina_list_data_get(iterator) != NULL; j++)
	{
		i = eina_list_data_get(iterator);

	        if (sd->visible_items+1 > sd->items_per_page)
		{
                	evas_object_hide(i);
			edje_object_signal_emit(i, "ephoto.thumb.hidden", "ephoto");
		}
		else
		{
			evas_object_show(i);
			edje_object_signal_emit(i, "ephoto.thumb.visible", "ephoto");
			evas_object_move(i, sd->tw, sd->th);
			edje_object_signal_emit(i, "ephoto.thumb.visible", "ephoto");

			sd->visible_items++;

			sd->tw += (sd->item_w+sd->paddingw);

                	if (sd->tw+(sd->item_w+sd->paddingw) >= sd->visiblew)
                	{
                        	sd->th += (sd->item_h+sd->paddingh);
                		sd->tw = 60;
                	}
		}
		iterator = eina_list_next(iterator);
		iteratorb = eina_list_next(iteratorb);
	}
	if (sd->visible_items < sd->items_per_page)
	{
		for (j=0; j <= sd->items_per_page && eina_list_data_get(iteratorb) != NULL; j++)
		{
			if (sd->visible_items+1 > sd->items_per_page)
				return;

			image = eina_list_data_get(iteratorb);

			img = edje_object_add(em->e);
			edje_object_file_set(img, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/thumb/shadow");
			evas_object_move(img, sd->tw, sd->th);
			evas_object_show(img);
			evas_object_event_callback_add(img, EVAS_CALLBACK_MOUSE_UP, image_clicked, NULL);
			edje_object_signal_emit(img, "ephoto.thumb.visible", "ephoto");
			evas_object_data_set(img, "image", iteratorb);

			evas_object_resize(img, sd->item_w, sd->item_h);
       			evas_object_size_hint_min_set(img, sd->item_w, sd->item_h);
       			evas_object_size_hint_max_set(img, sd->item_w, sd->item_h);
			sd->visible_items++;

			im = evas_imaging_image_load(image, NULL);
			evas_imaging_image_size_get(im, &w, &h);
			if (w > 120 || h > 120)
			{
				ethumb_client_file_set(em->thumb_cli, image, NULL);
				if (!ethumb_client_thumb_exists(em->thumb_cli))
                                        ethumb_client_generate(em->thumb_cli, thumb_generated, img, NULL);
                                else
				{
					ethumb_client_thumb_path_get(em->thumb_cli, &thumb, NULL);
                                        thumb_generated(img, em->thumb_cli, 0, image, NULL, thumb, NULL, EINA_TRUE);
                                }
			}
			else
				thumb_generated(img, em->thumb_cli, 0, image, NULL, image, NULL, EINA_TRUE);
	
			evas_imaging_image_free(im);

        		sd->tw += (sd->item_w + sd->paddingw);
        		
        		sd->items = eina_list_append(sd->items, img);

			if (sd->tw+(sd->item_w + sd->paddingw) >= sd->visiblew)
        		{
                		sd->th += (sd->item_h + sd->paddingh);
                		sd->tw = 60;
        		}
			iteratorb = eina_list_next(iteratorb);
		}
	}
	return;
}

static void _table_smart_init(void)
{
	if (_smart)
		return;
	{
		static const Evas_Smart_Class sc =
		{
			"ephoto_table",
			EVAS_SMART_CLASS_VERSION,
			_table_smart_add,
			_table_smart_del,
			_table_smart_move,
			_table_smart_resize,
			_table_smart_show,
			_table_smart_hide,
			_table_smart_color_set,
			_table_smart_clip_set,
			_table_smart_clip_unset,
			NULL,
			NULL
		};
		_smart = evas_smart_class_new(&sc);
	}
}

static void _table_smart_add(Evas_Object *obj)
{
	Smart_Data *sd;
	Evas_Object *img;
	int w, h;

	sd = calloc(1, sizeof(Smart_Data));
	if (!sd)
		return;

	edje_object_part_geometry_get(em->bg, "ephoto.swallow.content", 0, 0, &w, &h);
	
	sd->visiblew = w;
	sd->visibleh = h;
	sd->tw = 60;
	sd->th = 75;
	sd->paddingw = 0;
	sd->paddingh = 0;
	sd->w = 0;
	sd->h = 0;
	sd->x = 0;
	sd->y = 0;
	sd->current_head = 0;
	sd->current_page = 1;
	sd->visible_items = 0;
	sd->total_pages = 1;

	sd->obj = edje_object_add(em->e);
	edje_object_file_set(sd->obj, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/image/browser");
	evas_object_smart_data_set(obj, sd);

	edje_object_signal_callback_add(sd->obj, "mouse,up,1", "ephoto.move.left", move_left, NULL);

        edje_object_signal_callback_add(sd->obj, "mouse,up,1", "ephoto.move.right", move_right, NULL);

	img = edje_object_add(em->e);
        edje_object_file_set(img, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/thumb/shadow");
        edje_object_size_min_get(img, &sd->item_w, &sd->item_h);
	evas_object_del(img);

	sd->items_per_page = (sd->visiblew / sd->item_w+sd->paddingw) * (sd->visibleh / sd->item_h+sd->paddingh);
	em->thumb_cli = ethumb_client_connect(connect_callback, NULL, NULL);
}

static void _table_smart_del(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	while (eina_list_data_get(sd->items))
	{
		Evas_Object *child;

		child = eina_list_data_get(sd->items);
		if (evas_object_visible_get(child))
			evas_object_del(child);
		sd->items = eina_list_next(sd->items);
	}
	evas_object_del(sd->obj);
	eina_list_free(sd->images);
	eina_list_free(sd->items);
	free(sd);
}

static void _table_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	if ((x == sd->x) && (y == sd->y))
		return;
	sd->x = x;
	sd->y = y;
	evas_object_move(sd->obj, x, y);
}

static void _table_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
	Smart_Data *sd;
	int ibw, ibh;
	
	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;

	evas_object_geometry_get(sd->obj, 0, 0, &ibw, &ibh);

	if ((w == sd->w) && (h == sd->h))
		return;
	sd->w = w;
	sd->h = h;
	evas_object_resize(sd->obj, w, h);
	if (!eina_list_count(sd->images))
		return;
	if ((ibw == sd->visiblew) && (ibh == sd->visibleh))
		return;
	else
		ephoto_table_viewport_set(obj, w, h);
}

static void _table_smart_show(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_show(sd->obj);
	_table_smart_reconfigure(sd);
}

static void _table_smart_hide(Evas_Object *obj)
{
	Eina_List *iterator;
	Evas_Object *o;
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	iterator = sd->items;
	while (eina_list_data_get(iterator))
	{
		
		o = eina_list_data_get(iterator);
		edje_object_signal_emit(o, "ephoto.thumb.hidden", "ephoto");
		evas_object_hide(o);
		iterator = eina_list_next(iterator);
	}
	evas_object_hide(sd->obj);
}

static void _table_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_color_set(sd->obj, r, g, b, a);
}

static void _table_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_clip_set(sd->obj, clip);

	return;
}

static void _table_smart_clip_unset(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_clip_unset(sd->obj);

	return;
}

