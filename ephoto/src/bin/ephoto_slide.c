#include "ephoto.h"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
	Evas_Object *obj;
	Evas_Object *images;
	int x;
	int y;
	int w;
	int h;
	int iw;
	int ih;
	Evas_Object *center_image_one;
	Evas_Object *center_image_two;
	Eina_List *current_node;
	Eina_List *list;
	Ecore_Timer *timer;
};

static Evas_Smart *_smart = NULL;

static void _slide_smart_reconfigure(Smart_Data *sd);
static void _slide_smart_init(void);
static void _slide_smart_add(Evas_Object *obj);
static void _slide_smart_del(Evas_Object *obj);
static void _slide_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _slide_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _slide_smart_show(Evas_Object *obj);
static void _slide_smart_hide(Evas_Object *obj);
static void _slide_smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _slide_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _slide_smart_clip_unset(Evas_Object *obj);

static int change_image(void *data);
static void go_back(void *data, Evas_Object *obj, const char *emission, const char *source);
static void done(void *data, Evas_Object *obj, const char *emission, const char *source);

static int change_image(void *data)
{
	char *center;
	Smart_Data *sd;

        sd = data;

	done(sd, sd->obj, NULL, NULL);

	if (sd->current_node->next)
                sd->current_node = sd->current_node->next;
        else
                sd->current_node = sd->list;

	center = sd->current_node->data;

	edje_object_part_unswallow(sd->images, sd->center_image_two);
	ephoto_image_fill_inside_set(sd->center_image_two, 1);
        ephoto_image_file_set(sd->center_image_two, center, sd->iw, sd->ih);
	edje_object_part_swallow(sd->images, "ephoto.swallow.content.two", sd->center_image_two);	

	edje_object_signal_emit(sd->images, "ephoto.change.image", "ephoto");

	return 1;
}

static void go_back(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	Smart_Data *sd;

        sd = data;

	ephoto_slide_stop(obj);
	hide_slideshow();
	show_flow_view(sd->current_node, sd->list);
}

static void done(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	char *center;
	Smart_Data *sd;

        sd = data;

	center = sd->current_node->data;

	edje_object_part_unswallow(sd->images, sd->center_image_one);
	ephoto_image_fill_inside_set(sd->center_image_one, 1);
        ephoto_image_file_set(sd->center_image_one, center, sd->iw, sd->ih);
	edje_object_part_swallow(sd->images, "ephoto.swallow.content.one", sd->center_image_one);
}

Evas_Object *ephoto_slide_add(Evas *e)
{
	_slide_smart_init();
	return evas_object_smart_add(e, _smart);
}

void ephoto_slide_current_node_set(Evas_Object *obj, Eina_List *node)
{
	char *center;
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);

	sd->current_node = node;
        center = sd->current_node->data;

	edje_object_part_unswallow(sd->images, sd->center_image_one);
	ephoto_image_fill_inside_set(sd->center_image_one, 1);
	ephoto_image_file_set(sd->center_image_one, center, sd->iw, sd->ih);
	edje_object_part_swallow(sd->images, "ephoto.swallow.content.one", sd->center_image_one);

	return;
}

void ephoto_slide_item_list_set(Evas_Object *obj, Eina_List *list)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);

	sd->list = list;
	return;
}

void ephoto_slide_start(Evas_Object *obj)
{
	Smart_Data *sd;

        sd = evas_object_smart_data_get(obj);

	sd->timer = ecore_timer_add(3, change_image, sd);	
}

void ephoto_slide_stop(Evas_Object *obj)
{
	Smart_Data *sd;

        sd = evas_object_smart_data_get(obj);

	ecore_timer_del(sd->timer);
}

static void _slide_smart_reconfigure(Smart_Data *sd)
{
	int w, h, nw, nh;

	evas_object_geometry_get(sd->obj, 0, 0, &w, &h);

	nw = w/1.2;
        nh = h/1.2;

	sd->iw = nw;
	sd->ih = nh;

	edje_object_part_unswallow(sd->images, sd->center_image_one);
	edje_object_part_unswallow(sd->images, sd->center_image_two);
	edje_object_part_unswallow(sd->obj, sd->images);
	evas_object_image_load_size_set(sd->center_image_one, nw, nh);
	evas_object_image_load_size_set(sd->center_image_two, nw, nh);
	evas_object_resize(sd->center_image_one, nw, nh);
	evas_object_resize(sd->center_image_two, nw, nh);
	evas_object_resize(sd->images, nw, nh);
	evas_object_size_hint_min_set(sd->images, nw, nh);
        evas_object_size_hint_max_set(sd->images, nw, nh);
	edje_object_part_swallow(sd->obj, "ephoto.swallow.content", sd->images);
	edje_object_part_swallow(sd->images, "ephoto.swallow.content.one", sd->center_image_one);
	edje_object_part_swallow(sd->images, "ephoto.swallow.content.two", sd->center_image_two);

	return;
}

static void _slide_smart_init(void)
{
	if (_smart)
		return;
	{
		static const Evas_Smart_Class sc =
		{
			"ephoto_slide",
			EVAS_SMART_CLASS_VERSION,
			_slide_smart_add,
			_slide_smart_del,
			_slide_smart_move,
			_slide_smart_resize,
			_slide_smart_show,
			_slide_smart_hide,
			_slide_smart_color_set,
			_slide_smart_clip_set,
			_slide_smart_clip_unset,
			NULL,
			NULL
		};
		_smart = evas_smart_class_new(&sc);
	}
}

static void _slide_smart_add(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = calloc(1, sizeof(Smart_Data));
	if (!sd)
		return;

	sd->w = 0;
	sd->h = 0;
	sd->x = 0;
	sd->y = 0;

	sd->obj = edje_object_add(em->e);
        edje_object_file_set(sd->obj, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/slide/view");
	evas_object_smart_data_set(obj, sd);

	sd->images = edje_object_add(em->e);
	edje_object_file_set(sd->images, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/slide/images");
	edje_object_part_swallow(sd->obj, "ephoto.swallow.content", sd->images);

	sd->center_image_one = ephoto_image_add();
	edje_object_part_swallow(sd->images, "ephoto.swallow.content.one", sd->center_image_one);
	sd->center_image_two = ephoto_image_add();
	edje_object_part_swallow(sd->images, "ephoto.swallow.content.two", sd->center_image_two);

	edje_object_signal_callback_add(sd->obj, "mouse,up,1", "ephoto.move.back", go_back, sd);
	edje_object_signal_callback_add(sd->images, "ephoto.transition.done", "ephoto", done, sd);	
}

static void _slide_smart_del(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_del(sd->images);
	evas_object_del(sd->obj);
	evas_object_del(sd->center_image_one);
	evas_object_del(sd->center_image_two);
	free(sd);
}

static void _slide_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
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
	_slide_smart_reconfigure(sd);
}

static void _slide_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
	Smart_Data *sd;
	
	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;

	if ((w == sd->w) && (h == sd->h))
		return;
	sd->w = w;
	sd->h = h;
	evas_object_resize(sd->obj, w, h);
	_slide_smart_reconfigure(sd);
}

static void _slide_smart_show(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;

	evas_object_show(sd->obj);
	evas_object_show(sd->images);
	evas_object_show(sd->center_image_one);
	evas_object_show(sd->center_image_two);
}

static void _slide_smart_hide(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;

	evas_object_hide(sd->obj);
	evas_object_hide(sd->images);
	evas_object_hide(sd->center_image_one);
        evas_object_hide(sd->center_image_two);
}

static void _slide_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_color_set(sd->obj, r, g, b, a);
}

static void _slide_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_clip_set(sd->obj, clip);

	return;
}

static void _slide_smart_clip_unset(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_clip_unset(sd->obj);

	return;
}
