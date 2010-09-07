#include "ephoto.h"

static Evas_Object *_ephoto_get_image(void *data, Evas_Object *obj);
static void _ephoto_end_slideshow(void *data, Evas *e, Evas_Object *obj, void *event_info);

static Elm_Slideshow_Item_Class itc;
static int return_view;
static const char *cur_image;

/*Create the slideshow*/
void 
ephoto_create_slideshow(void)
{
	em->slideshow = elm_slideshow_add(em->win);
	elm_slideshow_loop_set(em->slideshow, EINA_TRUE);
	elm_slideshow_layout_set(em->slideshow, "fullscreen");
	elm_slideshow_transition_set(em->slideshow, "fade");
	evas_object_size_hint_weight_set(em->slideshow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(em->slideshow, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_event_callback_add(em->slideshow, EVAS_CALLBACK_MOUSE_DOWN,
						_ephoto_end_slideshow, NULL);
}

/*Start and show the slideshow*/
void 
ephoto_show_slideshow(int view, const char *current_image)
{
	Eina_List *l = NULL, *iter = NULL;
	const char *image;
	int x, y, w, h;

	ephoto_create_slideshow();

	return_view = view;
	cur_image = current_image;

	evas_object_geometry_get(em->box, &x, &y, &w, &h);
	evas_object_resize(em->slideshow, w, h);
	evas_object_move(em->slideshow, 0, 0);

	itc.func.get = _ephoto_get_image;
	itc.func.del = NULL;

	l = em->images;
	EINA_LIST_FOREACH(l, iter, image)
	{
		elm_slideshow_item_add(em->slideshow, &itc, image);
	}
	evas_object_show(em->slideshow);
	elm_win_resize_object_add(em->win, em->slideshow);
	elm_slideshow_timeout_set(em->slideshow, 4);
}

/*Hide the slideshow object*/
void 
ephoto_hide_slideshow(void)
{
	ephoto_delete_slideshow();
	if (return_view == 0)
		ephoto_show_thumb_browser();
	else
		ephoto_show_flow_browser(cur_image);
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

