#include "ephoto.h"

static Evas_Object *_ephoto_get_image(void *data, Evas_Object *obj);
static void _ephoto_end_slideshow(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _ephoto_switch_fullscreen();

static Elm_Slideshow_Item_Class itc;
static int return_view;
static const char *cur_image;

/*Key is pressed*/
static void
_ephoto_key_pressed(void *data, Evas *e, Evas_Object *obj, void *event_data)
{
	Evas_Event_Key_Down *eku = (Evas_Event_Key_Down *)event_data;
	if ((!strcmp(eku->key, "F5")) || (!strcmp(eku->key, "Escape")))
		ephoto_hide_slideshow();
	if ((!strcmp(eku->key, "F11")))
		_ephoto_switch_fullscreen();
}

static void
_ephoto_switch_fullscreen(void)
{
	elm_win_fullscreen_set(em->win, !elm_win_fullscreen_get(em->win));
}

/*Create the slideshow*/
void 
ephoto_create_slideshow(void)
{
	em->slideshow = elm_slideshow_add(em->win);
	elm_slideshow_loop_set(em->slideshow, EINA_TRUE);
	elm_slideshow_layout_set(em->slideshow, "fullscreen");
	evas_object_size_hint_weight_set(em->slideshow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(em->slideshow, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_event_callback_add(em->slideshow, EVAS_CALLBACK_MOUSE_DOWN,
						_ephoto_end_slideshow, NULL);
	evas_object_event_callback_add(em->slideshow, EVAS_CALLBACK_KEY_DOWN,
						_ephoto_key_pressed, NULL);
        em->slideshow_notify = elm_notify_add(em->win);
        Evas_Object *label = elm_label_add(em->win);
        elm_label_label_set(label, "Press F11 to turn off fullscreen");
        elm_notify_content_set(em->slideshow_notify, label);
        elm_notify_orient_set(em->slideshow_notify, ELM_NOTIFY_ORIENT_TOP);
        elm_notify_timeout_set(em->slideshow_notify, 2);
}

/*Start and show the slideshow*/
void 
ephoto_show_slideshow(int view, const char *current_image)
{
	Eina_List *l = NULL, *iter = NULL;
	Elm_Slideshow_Item *cur_item = NULL, *item;
	const char *image;
	int x, y, w, h;

	elm_slideshow_transition_set(em->slideshow, em->config->slideshow_transition);
	elm_slideshow_timeout_set(em->slideshow, em->config->slideshow_timeout);
	elm_win_fullscreen_set(em->win, EINA_TRUE);

	return_view = view;
	cur_image = current_image;

	evas_object_geometry_get(em->win, &x, &y, &w, &h);
	evas_object_resize(em->slideshow, w, h);
	evas_object_move(em->slideshow, 0, 0);

	itc.func.get = _ephoto_get_image;
	itc.func.del = NULL;

	elm_slideshow_clear(em->slideshow);
        /* TODO:
	l = em->images;
	EINA_LIST_FOREACH(l, iter, image)
	{
		item = elm_slideshow_item_add(em->slideshow, &itc, image);
		if ((current_image) && (!strcmp(image, current_image)))
			cur_item = item;
	}
        */
	if (cur_item)
		elm_slideshow_show(cur_item);

	evas_object_show(em->slideshow);
	evas_object_focus_set(em->slideshow, EINA_TRUE);
	elm_win_resize_object_add(em->win, em->slideshow);

        evas_object_show(em->slideshow_notify);
}

/*Hide the slideshow object*/
void 
ephoto_hide_slideshow(void)
{
        elm_win_fullscreen_set(em->win, EINA_FALSE);
        evas_object_hide(em->slideshow);
        elm_slideshow_clear(em->slideshow);
        if (return_view == 0)
                evas_object_focus_set(em->thumb_browser, 1);
        else if (return_view == 1)
                evas_object_focus_set(em->flow_browser, 1);

	// FIXME
	/*
	if (return_view == 0)
		ephoto_show_thumb_browser();
		else*/
	//ephoto_show_flow_browser(cur_image);
}

/*Delete the slideshow object*/
void 
ephoto_delete_slideshow(void)
{
	evas_object_del(em->slideshow);
}

/*Get the image for the slideshow*/
static Evas_Object *
_ephoto_get_image(void *data, Evas_Object *obj)
{
	char *file;
	Evas_Object *image;
	char *buffer;
	int length;

	file = data;

	image = elm_photo_add(obj);
	elm_photo_file_set(image, file);
	elm_photo_fill_inside_set(image, EINA_TRUE);
	elm_object_style_set(image, "shadow");

	length = strlen(file) + strlen("Ephoto - ") + 1;
	buffer = alloca(length);
	snprintf(buffer, length, "Ephoto - %s", file);
	elm_win_title_set(em->win, buffer);

	return image;
}

/*End the slideshow*/
static void 
_ephoto_end_slideshow(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	ephoto_hide_slideshow();
}

