/*
 * eon_engine_sdl.c
 *
 *  Created on: 03-feb-2009
 *      Author: jl
 */
#include "Eon.h"
#include "eon_private.h"

#include "Eon_Enesim.h"

#include "Ecore_Input.h"
#include "Ecore_Sdl.h"
#include "SDL.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EON_TYPE_ENGINE_SDL "Eon_Engine_SDL"
//#define SDL_PURE
#define SINGLE_BUFFER

static Ekeko_Type *_engine_sdl_type_get(void);

typedef struct _Engine_SDL_Private
{

} Engine_SDL_Private;

typedef struct _Engine_SDL
{
	Eon_Engine_Enesim parent;
	Engine_SDL_Private *prv;
} Engine_SDL;

static void _rect(void *surface, void *context, int x, int y, int w, int h)
{
	SDL_Rect rect;
	uint32_t color;

#ifdef EON_DEBUG
	printf("RENDERING A RECTANGLE at %d %d %d %d\n", x, y, w, h);
#endif
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_FillRect(surface, &rect, color);
}

static int _sdl_event(void *data)
{
	ecore_sdl_feed_events();
	return 1;
}

static int _resize_cb(void *data, int type, void *event)
{
	Ecore_Sdl_Event_Video_Resize *e = event;
	Eon_Document *doc = data;

	eon_document_resize(doc, e->w, e->h);

	return 1;
}
static int _mouse_down_cb(void *data, int type, void *event)
{
	Ekeko_Input *input = (Ekeko_Input *)data;
	ekeko_input_feed_mouse_down(input);
	return 1;
}

static int _mouse_up_cb(void *data, int type, void *event)
{
	Ekeko_Input *input = (Ekeko_Input *)data;
	ekeko_input_feed_mouse_up(input);
	return 1;
}

static int _mouse_move_cb(void *data, int type, void *event)
{
	Ekeko_Input *input = (Ekeko_Input *)data;
	Ecore_Event_Mouse_Move *e = event;

	ekeko_input_feed_mouse_move(input, e->x, e->y);
	return 1;
}

static int _in_cb(void *data, int type, void *event)
{
	Ekeko_Input *input = (Ekeko_Input *)data;
	return 1;
}
static int _out_cb(void *data, int type, void *event)
{
	Ekeko_Input *input = (Ekeko_Input *)data;
	return 1;
}

static void * document_create(Eon_Document *d)
{
	Ekeko_Input *input;
	Eon_Canvas *canvas;

	printf("[SDL] Initializing SDL\n");
	ecore_sdl_init(NULL);
	SDL_Init(SDL_INIT_VIDEO);

	canvas = eon_document_canvas_get(d);
	/* add the input */
	input = ekeko_canvas_input_new((Ekeko_Canvas *)canvas);
	/* add the callbacks */
	//ecore_event_handler_add(ECORE_SDL_EVENT_RESIZE, _resize_cb, canvas);
	ecore_event_handler_add(ECORE_SDL_EVENT_RESIZE, _resize_cb, d);
	ecore_event_handler_add(ECORE_SDL_EVENT_GOT_FOCUS, _in_cb, input);
	ecore_event_handler_add(ECORE_SDL_EVENT_LOST_FOCUS, _out_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, _mouse_down_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, _mouse_up_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, _mouse_move_cb, input);
	/* FIXME Ecore_SDL doesnt support the in/out events, make it always in */
	ekeko_input_feed_mouse_in(input);
	/* the event feeder evas/ecore has a very weird way to feed sdl events! */
	ecore_timer_add(0.008, _sdl_event, NULL);
	return NULL;
}

static void _delete(Eon_Document *d, void *data)
{
	ecore_sdl_shutdown();
}

static void _lock(void *s)
{
	SDL_Surface *src = s;

	SDL_LockSurface(src);
}

static void _unlock(void *s)
{
	SDL_Surface *src = s;

	SDL_UnlockSurface(src);
}

static void _blit(void *src, Eina_Rectangle *srect, void *context, void *dst, Eina_Rectangle *drect)
{
	SDL_Rect ssrect, sdrect;

	ssrect.x = srect->x;
	ssrect.y = srect->y;
	ssrect.w = srect->w;
	ssrect.h = srect->h;

	sdrect.x = drect->x;
	sdrect.y = drect->y;
	sdrect.w = drect->w;
	sdrect.h = drect->h;

#ifdef EON_DEBUG
	printf("[SDL] rendering into %p from %p (%d %d %d %d to %d %d %d %d)\n",
			dst, src, srect->x, srect->y, srect->w, srect->h,
			drect->x, drect->y, drect->w, drect->h);
#endif
	SDL_BlitSurface(src, &ssrect, dst, &sdrect);
}

static Eina_Bool _flush(void *src, Eina_Rectangle *srect)
{
	SDL_Surface *s = src;

#ifdef EON_DEBUG
	printf("[SDL] Flushing surface %p\n", s);
#endif
#ifdef SINGLE_BUFFER
	SDL_UpdateRect(s, srect->x, srect->y, srect->w, srect->h);
	return EINA_FALSE;
#else
	SDL_Flip(s);
	return EINA_TRUE;
#endif

}

static void * _sdl_surface_create(Eina_Bool root, int w, int h)
{
}


static void * canvas_create(Eon_Canvas *c, Eina_Bool root, int w, int h)
{
	Enesim_Surface *es;

	es = enesim_surface_new(ENESIM_FORMAT_ARGB8888, w, h);
	if (root)
	{
		SDL_Surface *s;

		/* the destination surface */
		printf("[SDL] Setting video mode to %d %d\n", w, h);
#ifdef SINGLE_BUFFER
		s = SDL_SetVideoMode(w, h, 32, SDL_RESIZABLE | SDL_SRCALPHA);
		//return SDL_SetVideoMode(w, h, 32, SDL_SRCALPHA);
#else
		//return SDL_SetVideoMode(w, h, 32, SDL_RESIZABLE | SDL_SRCALPHA |
		//		SDL_DOUBLEBUF);
		s = SDL_SetVideoMode(w, h, 32, SDL_SRCALPHA | SDL_DOUBLEBUF);
#endif
		enesim_surface_private_set(es, s);
	}
	return es;
}


static void _enesim_blit(void *s, void *context, void *src, Eina_Rectangle *srect)
{
	eon_enesim_image(s, context, src, srect);
}

static Eina_Bool canvas_flush(void *src, Eina_Rectangle *srect)
{
	Enesim_Surface *es = src;
	Enesim_Operator op;
	Enesim_Cpu **cpus;
	uint32_t *sdata;
	uint8_t *cdata;

	SDL_Surface *s;

	int h = srect->h;
	int stride;
	int inc;
	int numcpus;
	int soffset;
	int coffset;

#if 0
	printf("Flushing the canvas %d %d %d %d\n", srect->x, srect->y, srect->w, srect->h);
	{
		int w, h;
		enesim_surface_size_get(es, &w, &h);
		printf("%d %d\n", w, h);
	}
#endif
	/* setup the pointers */
	s = enesim_surface_private_get(es);
	stride = enesim_surface_stride_get(es);

	sdata = enesim_surface_data_get(es);
	cdata = s->pixels;

	soffset = (stride * srect->y) + srect->x;
	coffset = (s->pitch * srect->y) + (srect->x * 4);

	cdata += coffset;
	sdata += soffset;
	/* convert */
	cpus = enesim_cpu_get(&numcpus);
	enesim_converter_1d_from_get(&op, cpus[0], ENESIM_FORMAT_ARGB8888, ENESIM_CONVERTER_ARGB8888);

	_lock(s);
	while (h--)
	{
		enesim_operator_converter_1d(&op, sdata, srect->w, cdata);
		sdata += stride;
		cdata += s->pitch;
	}
	_unlock(s);
	return _flush(s, srect);
}

static void _enesim_lock(void *src)
{
	Enesim_Surface *es = src;
	SDL_Surface *s;

	/* FIXME here we can have a SDL_Surface or an Enesim_Surface */
	s = enesim_surface_private_get(es);
	if (s)
		_lock(s);
}

static void _enesim_unlock(void *src)
{
	Enesim_Surface *es = src;
	SDL_Surface *s;

	/* FIXME here we can have a SDL_Surface or an Enesim_Surface */
	s = enesim_surface_private_get(es);
	if (s)
		_unlock(s);
}



static void _ctor(void *instance)
{
	Engine_SDL *e;
	Engine_SDL_Private *prv;

	e = (Engine_SDL *) instance;
	e->prv = prv = ekeko_type_instance_private_get(_engine_sdl_type_get(), instance);
	e->parent.parent.document_create = document_create;
	e->parent.parent.canvas_create = canvas_create;
	e->parent.parent.canvas_flush = canvas_flush;
}

static void _dtor(void *instance)
{

}
static Ekeko_Type *_engine_sdl_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_ENGINE_SDL, sizeof(Engine_SDL),
				sizeof(Engine_SDL_Private), eon_engine_enesim_type_get(),
				_ctor, _dtor, NULL);
	}

	return type;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void engine_sdl_init(void)
{
	eon_engine_register("sdl", _engine_sdl_type_get);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

