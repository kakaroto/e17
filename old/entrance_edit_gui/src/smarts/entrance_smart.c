#include <stdio.h>
#include <stdlib.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include "Entrance_Smart.h"


typedef struct _Entrance_Smart {
	Evas_Object *background;
	Evas_Object *main;
	Evas_Object *session;
	Evas_Object *clip;

	Evas_Coord w;
	Evas_Coord h;
	Evas_Coord x;
	Evas_Coord y;
} *Entrance_Smart;


#define ENTRANCE_SMART_OBJECT_GET(dst, src) Entrance_Smart dst = evas_object_smart_data_get((src)); if(1) { if(!dst) return; } else

static Evas_Smart *_es_build(void);
static void _es_cb_add(Evas_Object *o);
static void _es_cb_del(Evas_Object *o);
static void _es_cb_layer_set(Evas_Object *o, int l);
static void _es_cb_raise(Evas_Object *o);
static void _es_cb_lower(Evas_Object *o);
static void _es_cb_stack_above(Evas_Object *o, Evas_Object *above);
static void _es_cb_stack_below(Evas_Object *o, Evas_Object *below);
static void _es_cb_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _es_cb_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _es_cb_show(Evas_Object *o);
static void _es_cb_hide(Evas_Object *o);
static void _es_cb_color_set(Evas_Object *o, int r, int g, int b, int a);
static void _es_cb_clip_set(Evas_Object *o, Evas_Object *clip);
static void _es_cb_clip_unset(Evas_Object *o);

int
entrance_smart_init(void)
{
	/*FIXME:? Incase someone forgets to call this*/
	if(!ecore_evas_init())
		return 0;

	if(!evas_init())
		return 0;

	if(!edje_init())
		return 0;

	return 1;
}

int
entrance_smart_shutdown(void)
{
}


Evas_Object* 
es_new(Evas *e)
{
	Entrance_Smart data;
	Evas_Object *o;

	o = evas_object_smart_add(e, _es_build());
	if(o)
	{
		data = evas_object_smart_data_get(o);
		if(data)
			return o;
		else
			evas_object_del(o);
	}

	return NULL;
}

void
es_free(Evas_Object *o)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);

	evas_object_del(es->background);
	evas_object_del(es->main);
	evas_object_del(es->session);
	evas_object_del(es->clip);

	free(es);
	evas_object_del(o);
}

void
es_background_edje_set(Evas_Object *o, const char *file)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	Evas *e = evas_object_evas_get(o);
	if(!e)
		return;
	if(es->background)
		evas_object_del(es->background);

	es->background = edje_object_add(e);
	if(es->background == NULL)
		return;

	if(!edje_object_file_set(es->background, file, "Background"))
		if(!edje_object_file_set(es->background, file, "e/desktop/background"))
			return;

	evas_object_clip_set(es->background, es->clip);
}

void
es_main_edje_set(Evas_Object *o, const char *file)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	Evas *e = evas_object_evas_get(o);
	if(!e)
		return;

	if(es->main)
		evas_object_del(es->main);

	es->main = edje_object_add(e);
	if(es->main == NULL)
		return;

	if(!edje_object_file_set(es->main, file, "Main"))
			return;

	evas_object_clip_set(es->main, es->clip);
}

static Evas_Smart*
_es_build(void)
{
	static Evas_Smart *s = NULL;
	if(s)
		return s;
	
	s = evas_smart_new("entrance_smart",
						_es_cb_add,
						_es_cb_del,
						_es_cb_layer_set,
						_es_cb_raise,
						_es_cb_lower,
						_es_cb_stack_above,
						_es_cb_stack_below,
						_es_cb_move,
						_es_cb_resize,
						_es_cb_show,
						_es_cb_hide,
						_es_cb_color_set,
						_es_cb_clip_set,
						_es_cb_clip_unset,
						NULL);

	return s;
}

static void
_es_cb_add(Evas_Object *o)
{
	Entrance_Smart es = calloc(1, sizeof(*es));
	if(!es)
		return;
	
	Evas *evas = evas_object_evas_get(o);
	Ecore_Evas *ee = ecore_evas_ecore_evas_get(evas);

	es->clip =  evas_object_rectangle_add(evas);
	es->background = NULL;
	es->main = NULL;
	es->session = NULL;

	es->x = es->y = es->w = es->h = 0;

	evas_object_smart_data_set(o, es);
}


static void
_es_cb_del(Evas_Object *o)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);

	evas_object_del(es->clip);
	if(es->background)
		evas_object_del(es->background);
	if(es->main)
		evas_object_del(es->main);
	/*evas_object_del(es->session);*/

	free(es);
}

static void
_es_cb_layer_set(Evas_Object *o, int l)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);

	evas_object_layer_set(es->clip, l);
	if(es->background)
		evas_object_layer_set(es->background, l);
	if(es->main)
		evas_object_layer_set(es->main, l);
	/*if(es->session) 
		evas_object_layer_set(es->session, l);*/
}

static void
_es_cb_raise(Evas_Object *o)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_raise(es->clip);
	if(es->background)
		evas_object_raise(es->background);
	if(es->main)
		evas_object_raise(es->main);
	/*if(es->session)
		evas_object_raise(es->session);*/
}

static void
_es_cb_lower(Evas_Object *o)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_lower(es->clip);
	if(es->background)
		evas_object_lower(es->background);
	if(es->main)
		evas_object_lower(es->main);
	/*if(es->session)
		evas_object_lower(es->session);*/
}

static void
_es_cb_stack_above(Evas_Object *o, Evas_Object *above)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_stack_above(es->clip, above);
	if(es->background)
		evas_object_stack_above(es->background, above);
	if(es->main)
		evas_object_stack_above(es->main, above);
	/*if(es->session)
		evas_object_stack_above(es->session, above);*/
}

static void
_es_cb_stack_below(Evas_Object *o, Evas_Object *below)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_stack_below(es->clip, below);
	if(es->background)
		evas_object_stack_below(es->background, below);
	if(es->main)
		evas_object_stack_below(es->background, below);
	/*if(es->session)
		evas_object_stack_below(es->session, below);*/
}

static void
_es_cb_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_move(es->clip, x, y);
	if(es->background)
		evas_object_move(es->background, x, y);
	if(es->main)
		evas_object_move(es->main, x, y);
	/*if(es->session)
		evas_object_move(es->session, x, y);*/
}

static void
_es_cb_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_resize(es->clip, w, h);
	if(es->background)
		evas_object_resize(es->background, w, h);
	if(es->main)
		evas_object_resize(es->main, w, h);
}

static void
_es_cb_show(Evas_Object *o)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_show(es->clip);
	if(es->background)
		evas_object_show(es->background);
	if(es->main)
		evas_object_show(es->main);
}

static void
_es_cb_hide(Evas_Object *o)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_hide(es->clip);
	if(es->background)
		evas_object_hide(es->background);
	if(es->main)
		evas_object_hide(es->main);
}

static void
_es_cb_color_set(Evas_Object *o, int r, int g, int b, int a)
{
}

static void
_es_cb_clip_set(Evas_Object *o, Evas_Object *clip)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_clip_set(es->clip, clip);
}

static void
_es_cb_clip_unset(Evas_Object *o)
{
	ENTRANCE_SMART_OBJECT_GET(es, o);
	evas_object_clip_unset(es->clip);
}
