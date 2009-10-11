#include "ephoto.h"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
	Evas_Object *obj;
	int x;
	int y;
	int w;
	int h;
	Evas_Object *center_image;
	Eina_List *current_node;
	Eina_List *list;
};

static Evas_Smart *_smart = NULL;

static void _flow_smart_reconfigure(Smart_Data *sd);
static void _flow_smart_init(void);
static void _flow_smart_add(Evas_Object *obj);
static void _flow_smart_del(Evas_Object *obj);
static void _flow_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _flow_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _flow_smart_show(Evas_Object *obj);
static void _flow_smart_hide(Evas_Object *obj);
static void _flow_smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _flow_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _flow_smart_clip_unset(Evas_Object *obj);

static void move_left(void *data, Evas_Object *obj, const char *emission, const char *source);
static void move_right(void *data, Evas_Object *obj, const char *emission, const char *source);
static void go_back(void *data, Evas_Object *obj, const char *emission, const char *source);

static void move_left(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	Smart_Data *sd;

	sd = data;

        if (sd->current_node->prev)
		sd->current_node = sd->current_node->prev;
	else
		sd->current_node = eina_list_last(sd->list);
	_flow_smart_reconfigure(sd);
}

static void move_right(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	Smart_Data *sd;

	sd = data;

        if (sd->current_node->next)
                sd->current_node = sd->current_node->next;
        else
                sd->current_node = sd->list;
        _flow_smart_reconfigure(sd);
}

static void go_back(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	hide_flow_view();
	show_image_browser();
}

Evas_Object *ephoto_flow_add(Evas *e)
{
	_flow_smart_init();
	return evas_object_smart_add(e, _smart);
}

void ephoto_flow_current_node_set(Evas_Object *obj, Eina_List *node)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);

	sd->current_node = node;
	_flow_smart_reconfigure(sd);
	return;
}

void ephoto_flow_item_list_set(Evas_Object *obj, Eina_List *list)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);

	sd->list = list;
	return;
}

static void _flow_smart_reconfigure(Smart_Data *sd)
{
	char *center;
	int w, h, nw, nh;

	center = sd->current_node->data;

	evas_object_geometry_get(sd->obj, 0, 0, &w, &h);

	nw = w/1.2;
	nh = h/1.2;

	edje_object_part_unswallow(sd->obj, sd->center_image);	
	ephoto_image_fill_inside_set(sd->center_image, 1);
	ephoto_image_file_set(sd->center_image, center, nw, nh);
	evas_object_resize(sd->center_image, nw, nh);
        evas_object_size_hint_min_set(sd->center_image, nw, nh);
        evas_object_size_hint_max_set(sd->center_image, nw, nh);
	edje_object_part_swallow(sd->obj, "ephoto.swallow.content", sd->center_image);

	return;
}

static void _flow_smart_init(void)
{
	if (_smart)
		return;
	{
		static const Evas_Smart_Class sc =
		{
			"ephoto_flow",
			EVAS_SMART_CLASS_VERSION,
			_flow_smart_add,
			_flow_smart_del,
			_flow_smart_move,
			_flow_smart_resize,
			_flow_smart_show,
			_flow_smart_hide,
			_flow_smart_color_set,
			_flow_smart_clip_set,
			_flow_smart_clip_unset,
			NULL,
			NULL
		};
		_smart = evas_smart_class_new(&sc);
	}
}

static void _flow_smart_add(Evas_Object *obj)
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
        edje_object_file_set(sd->obj, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", "/ephoto/flow/view");
	evas_object_smart_data_set(obj, sd);

	edje_object_signal_callback_add(sd->obj, "mouse,up,1", "ephoto.move.left", move_left, sd);

        edje_object_signal_callback_add(sd->obj, "mouse,up,1", "ephoto.move.right", move_right, sd);

	edje_object_signal_callback_add(sd->obj, "mouse,up,1", "ephoto.move.back", go_back, sd);

	sd->center_image = ephoto_image_add();
	edje_object_part_swallow(sd->obj, "ephoto.swallow.content", sd->center_image);	
}

static void _flow_smart_del(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
        evas_object_del(sd->center_image);
	evas_object_del(sd->obj);
	free(sd);
}

static void _flow_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
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
	_flow_smart_reconfigure(sd);
}

static void _flow_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
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
	_flow_smart_reconfigure(sd);
}

static void _flow_smart_show(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;

	evas_object_show(sd->obj);
	evas_object_show(sd->center_image);
}

static void _flow_smart_hide(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;

	evas_object_hide(sd->obj);
	evas_object_hide(sd->center_image);
}

static void _flow_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_color_set(sd->obj, r, g, b, a);
}

static void _flow_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_clip_set(sd->obj, clip);

	return;
}

static void _flow_smart_clip_unset(Evas_Object *obj)
{
	Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (!sd)
		return;
	evas_object_clip_unset(sd->obj);

	return;
}
