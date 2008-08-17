#include <stdio.h>
#include <stdlib.h>
#include <Ecore_Evas.h>
#include <Evas.h>

typedef struct _Smart_Data {
	Evas_Object *canvas;
	Evas *evas;

	Evas_Coord x;
	Evas_Coord y;
	Evas_Coord w;
	Evas_Coord h;

	Evas_Coord v_x;
	Evas_Coord v_y;
	Evas_Coord v_w;
	Evas_Coord v_h;
} *Smart_Data;


#define PREVIEW_DATA_GET(dst, src) Smart_Data dst = evas_object_smart_data_get((src)); if(1) { if(!dst) return; } else

static Evas_Smart * _build_smart(void);

static void _cb_add(Evas_Object *o);
static void _cb_del(Evas_Object *o);
static void _cb_layer_set(Evas_Object *o, int l);
static void _cb_raise(Evas_Object *o);
static void _cb_lower(Evas_Object *o);
static void _cb_stack_above(Evas_Object *o, Evas_Object *above);
static void _cb_stack_below(Evas_Object *o, Evas_Object *below);
static void _cb_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _cb_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _cb_show(Evas_Object *o);
static void _cb_hide(Evas_Object *o);
static void _cb_color_set(Evas_Object *o, int r, int g, int b, int a);
static void _cb_clip_set(Evas_Object *o, Evas_Object *clip);
static void _cb_clip_unset(Evas_Object *o);


Evas_Object *
esmart_preview_new(Evas *e)
{
	Evas_Object *o;

	o = evas_object_smart_add(e, _build_smart());
	if(o)
	{

		Smart_Data sd = evas_object_smart_data_get(o);
		if(sd)
		{
			return o;
		} 
		else
		{
			evas_object_del(o);
		}
	}

	return NULL;
}

Evas*
esmart_preview_evas_get(Evas_Object *o)
{
	PREVIEW_DATA_GET(sd, o);
	return sd->evas;
}

void
esmart_preview_virtual_size_set(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_image_size_set(sd->canvas, w, h);
	evas_object_image_fill_set(sd->canvas, 0, 0, w, h);
	evas_object_resize(sd->canvas, w, h);

	sd->v_w = w;
	sd->v_h = h;
}



static Evas_Smart *
_build_smart(void)
{
	static Evas_Smart *s = NULL;
	if(s)
		return s;

	s = evas_smart_new("esmart_preview",
						_cb_add,
						_cb_del,
						_cb_layer_set,
						_cb_raise,
						_cb_lower,
						_cb_stack_above,
						_cb_stack_below,
						_cb_move,
						_cb_resize,
						_cb_show,
						_cb_hide,
						_cb_color_set,
						_cb_clip_set,
						_cb_clip_unset,
						NULL);

	return s;
}

static void
_cb_add(Evas_Object *o)
{
	Smart_Data sd = calloc(1, sizeof(*sd));

	Ecore_Evas *ee = ecore_evas_ecore_evas_get(evas_object_evas_get(o));
	if(!ee)
		return;

	sd->canvas = ecore_evas_object_image_new(ee);
	if(!sd->canvas)
		return; 

	sd->evas = ecore_evas_get(evas_object_data_get(sd->canvas, "Ecore_Evas"));
	if(!sd->evas)
		return;

	sd->x = sd->y = sd->w = sd->h = 0;
	sd->v_x = sd->v_y = sd->v_w = sd->v_h = 0;

	evas_object_smart_data_set(o, sd);
}

static void
_cb_del(Evas_Object *o)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_del(sd->canvas);
	free(sd);
}

static void
_cb_layer_set(Evas_Object *o, int l)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_layer_set(sd->canvas, l);
}

static void
_cb_raise(Evas_Object *o)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_raise(sd->canvas);
}

static void
_cb_lower(Evas_Object *o)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_lower(sd->canvas);
}

static void
_cb_stack_above(Evas_Object *o, Evas_Object *above)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_stack_above(sd->canvas, above);
}

static void
_cb_stack_below(Evas_Object *o, Evas_Object *below)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_stack_below(sd->canvas, below);
}

static void
_cb_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_move(sd->canvas, x, y);

	sd->x = x;
	sd->y = y;
}

static void
_cb_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_image_fill_set(sd->canvas, 0, 0, w, h);
	evas_object_resize(sd->canvas, w, h);
	
	sd->w = w;
	sd->h = h;
}

static void
_cb_show(Evas_Object *o)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_show(sd->canvas);
}

static void
_cb_hide(Evas_Object *o)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_hide(sd->canvas);
}

static void
_cb_color_set(Evas_Object *o, int r, int g, int b, int a)
{
}

static void
_cb_clip_set(Evas_Object *o, Evas_Object *clip)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_clip_set(sd->canvas, clip);
}

static void
_cb_clip_unset(Evas_Object *o)
{
	PREVIEW_DATA_GET(sd, o);
	evas_object_clip_unset(sd->canvas);
}

