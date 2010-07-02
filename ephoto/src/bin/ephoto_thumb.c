#include "ephoto.h"

/*Ephoto Smart Data */
typedef struct _Thumb_Smart_Data Ephoto_Thumb_Smart_Data;

struct _Thumb_Smart_Data
{
	Evas_Object *thumb;
	Evas_Object *border;
	Evas_Coord x, y, w, h;
	Evas_Object *obj;	
	char fill_inside : 1;
	char use_border: 1;
};

static void ephoto_thumb_smart_reconfigure(Ephoto_Thumb_Smart_Data *sd);
static void ephoto_thumb_smart_init(void);
static void ephoto_thumb_smart_add(Evas_Object *obj);
static void ephoto_thumb_smart_del(Evas_Object *obj);
static void ephoto_thumb_smart_move(Evas_Object *obj, Evas_Coord x,
						Evas_Coord y);
static void ephoto_thumb_smart_resize(Evas_Object *obj, Evas_Coord w, 
						Evas_Coord h);
static void ephoto_thumb_smart_show(Evas_Object *obj);
static void ephoto_thumb_smart_hide(Evas_Object *obj);
static void ephoto_thumb_smart_color_set(Evas_Object *obj, int r, int g,
						int b, int a);
static void ephoto_thumb_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void ephoto_thumb_smart_clip_unset(Evas_Object *obj);

/*Ephoto Globals*/
static Evas_Smart *ephoto_thumb_smart = NULL;

Evas_Object *
ephoto_thumb_add(void)
{
	ephoto_thumb_smart_init();
	return evas_object_smart_add(em->e, ephoto_thumb_smart);
}

void 
ephoto_thumb_file_set(Evas_Object *obj, const char *file, int w, int h)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	evas_object_image_load_size_set(sd->thumb, w, h);
	evas_object_image_file_set(sd->thumb, file, NULL);
	evas_object_image_preload(sd->thumb, TRUE);
	ephoto_thumb_smart_reconfigure(sd);
}

void 
ephoto_thumb_fill_inside_set(Evas_Object *obj, int fill)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if (fill && (sd->fill_inside == 1))
		return;
	if (!fill && (sd->fill_inside == 0))
		return;
	if (fill)
		sd->fill_inside = 1;
	else
		sd->fill_inside = 0;
	ephoto_thumb_smart_reconfigure(sd);
}

void 
ephoto_thumb_size_get(Evas_Object *obj, int *w, int *h)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	return evas_object_image_size_get(sd->thumb, w, h);
}

static void 
ephoto_thumb_smart_reconfigure(Ephoto_Thumb_Smart_Data *sd)
{
	Evas_Coord x, y, w, h;
	int tw, th;

	evas_object_image_size_get(sd->thumb, &tw, &th);

	if (tw < 1) tw = 1;
	if (th < 1) th = 1;

	if (sd->fill_inside)
	{
		w = sd->w;
		h = ((double)th *w) / (double)tw;
		if (h > sd->h)
		{
			h = sd->h;
			w = ((double)tw * h) / (double)th;
		}
	}
	else
	{
		w = sd->w;
		h = ((double)th * w) / (double)tw;
		if ( h < sd->h)
		{
			h = sd->h;
			w = ((double)tw * h) / (double)th;
		}
	}
	x = sd->x + ((sd->w - w) / 2);
	y = sd->y + ((sd->h - h) / 2);

	evas_object_move(sd->thumb, x, y);
	evas_object_image_fill_set(sd->thumb, 0, 0, w, h);
	evas_object_resize(sd->thumb, w, h);
}

static void 
ephoto_thumb_smart_init(void)
{
	if (ephoto_thumb_smart)
		return;
	static const Evas_Smart_Class sc =
	{
		"ephoto_thumb",
		EVAS_SMART_CLASS_VERSION,
		ephoto_thumb_smart_add,
		ephoto_thumb_smart_del,
		ephoto_thumb_smart_move,
		ephoto_thumb_smart_resize,
		ephoto_thumb_smart_show,
		ephoto_thumb_smart_hide,
		ephoto_thumb_smart_color_set,
		ephoto_thumb_smart_clip_set,
		ephoto_thumb_smart_clip_unset,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	};

	ephoto_thumb_smart = evas_smart_class_new(&sc);
}

static void
ephoto_thumb_smart_add(Evas_Object *obj)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = calloc(1, sizeof(Ephoto_Thumb_Smart_Data));
	if (!sd)
		return;

	sd->thumb = evas_object_image_add(em->e);
	evas_object_image_smooth_scale_set(sd->thumb, TRUE);
	sd->x = 0;
	sd->y = 0;
	sd->w = 0;
	sd->h = 0;
	sd->fill_inside = 1;
	evas_object_smart_member_add(sd->thumb, obj);
	evas_object_smart_data_set(obj, sd);
}

static void 
ephoto_thumb_smart_del(Evas_Object *obj)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	evas_object_del(sd->thumb);
	free(sd);
}

static void 
ephoto_thumb_smart_move(Evas_Object *obj, Evas_Coord x, 
					Evas_Coord y)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if ((sd->x == x) && (sd->y == y))
		return;
	sd->x = x;
	sd->y = y;
	ephoto_thumb_smart_reconfigure(sd);
}

static void 
ephoto_thumb_smart_resize(Evas_Object *obj, Evas_Coord w,
					Evas_Coord h)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	if ((sd->w == w) && (sd->h == h))
		return;
	sd->w = w;
	sd->h = h;
	ephoto_thumb_smart_reconfigure(sd);
}

static void 
ephoto_thumb_smart_show(Evas_Object *obj)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	evas_object_show(sd->thumb);
}

static void 
ephoto_thumb_smart_hide(Evas_Object *obj)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	evas_object_hide(sd->thumb);
}

static void 
ephoto_thumb_smart_color_set(Evas_Object *obj, int r, int g,
						int b, int a)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	evas_object_color_set(sd->thumb, r, g, b, a);
}

static void 
ephoto_thumb_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	evas_object_clip_set(sd->thumb, clip);
}

static void 
ephoto_thumb_smart_clip_unset(Evas_Object *obj)
{
	Ephoto_Thumb_Smart_Data *sd;

	sd = evas_object_smart_data_get(obj);
	evas_object_clip_unset(sd->thumb);
}
