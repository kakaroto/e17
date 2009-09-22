/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
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
//#define SDL_PURE
#define SINGLE_BUFFER

typedef struct _Engine_SDL_Document
{
	/* main surface */
	SDL_Surface *s;
	 // FIXME remove this?
	Eon_Canvas *root;
	/* options */
	char resizable:1;
	char dbuffer:1;
	Ecore_Event_Handler *handler_rz;
} Engine_SDL_Document;

typedef struct _Engine_SDL_Canvas
{
	Engine_SDL_Document *sdoc;
	/* mouse, key, etc handlers */
	Ecore_Event_Handler *handlers[7];
	Enesim_Surface *s;
} Engine_SDL_Canvas;

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

static int _key_down_cb(void *data, int type, void *event)
{
	Ekeko_Input *input = (Ekeko_Input *)data;

	ekeko_input_feed_key_down(input, 0, 0);
	return 1;
}

static int _key_up_cb(void *data, int type, void *event)
{
	Ekeko_Input *input = (Ekeko_Input *)data;

	ekeko_input_feed_key_up(input, 0, 0);
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

static void _sdl_surface_new(Engine_SDL_Document *sdoc, int w, int h)
{
	Uint32 flags = SDL_SRCALPHA;

	if (sdoc->resizable)
		flags |= SDL_RESIZABLE;
	/* the destination surface */
	printf("[SDL] Setting video mode to %d %d\n", w, h);
#ifdef SINGLE_BUFFER
	sdoc->s = SDL_SetVideoMode(w, h, 32, flags);
#else
	sdoc->s = SDL_SetVideoMode(w, h, 32, flags | SDL_DOUBLEBUF);
#endif
}

static void _document_size_cb(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Engine_SDL_Document *sdoc = data;
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;

	_sdl_surface_new(sdoc,  em->curr->value.rect.w, em->curr->value.rect.h);
}

static void * document_create(Eon_Document *d, const char *options)
{
	Engine_SDL_Document *sdoc;
	Uint32 flags = SDL_SRCALPHA;

	printf("[SDL] Initializing SDL\n");
	sdoc = calloc(1, sizeof(Engine_SDL_Document));
	if (options)
	{
		if (!strcmp(options, "resizable"))
			sdoc->resizable = 1;
	}
	ecore_sdl_init(NULL);
	SDL_Init(SDL_INIT_VIDEO);
	/* whenever the size property changes recreate the surface */
	ekeko_event_listener_add((Ekeko_Object *)d,
			EON_DOCUMENT_SIZE_CHANGED, _document_size_cb,
			EINA_FALSE, sdoc);
	/* called whenever the wm changes the window */
	ecore_event_handler_add(ECORE_SDL_EVENT_RESIZE, _resize_cb, d);

	return sdoc;
}

static void document_delete(void *data)
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

static void _root_canvas_create(Ekeko_Canvas *c)
{
	Ekeko_Input *input;

	/* Ecore_sdl interval isnt enough */
	SDL_EnableKeyRepeat(10, 10);
	/* add the input */
	input = ekeko_canvas_input_new((Ekeko_Canvas *)c);
	/* add the callbacks */
	ecore_event_handler_add(ECORE_SDL_EVENT_GOT_FOCUS, _in_cb, input);
	ecore_event_handler_add(ECORE_SDL_EVENT_LOST_FOCUS, _out_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, _mouse_down_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, _mouse_up_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, _mouse_move_cb, input);
	ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, _key_down_cb, input);
	ecore_event_handler_add(ECORE_EVENT_KEY_UP, _key_up_cb, input);
	/* FIXME Ecore_SDL doesnt support the in/out events, make it always in */
	ekeko_input_feed_mouse_in(input);
	/* the event feeder evas/ecore has a very weird way to feed sdl events! */
	ecore_timer_add(0.008, _sdl_event, NULL);
}

static void * canvas_create(Eon_Canvas *c, void *dd, Eina_Bool root, int w, int h)
{
	Engine_SDL_Document *sdoc = dd;
	Enesim_Surface *es;

	es = enesim_surface_new(ENESIM_FORMAT_ARGB8888, w, h);
	if (root)
	{
		/* only register the events once */
		if (!sdoc->root)
		{
			_root_canvas_create(c);
			sdoc->root = c;
		}
		enesim_surface_private_set(es, sdoc->s);
	}
	return es;
}

static void canvas_delete(void *c)
{
	/* TODO handle correctly the deletion of a canvas, delete
	 * every handler, etc */
	enesim_surface_delete(c);
}

static Eina_Bool canvas_flush(void *src, Eina_Rectangle *srect)
{
	Enesim_Surface *es = src;
	Enesim_Converter_1D conv;
	Enesim_Converter_Data cdata;
	uint32_t *sdata;
	uint8_t *sdldata;

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
	sdldata = s->pixels;

	soffset = (stride * srect->y) + srect->x;
	coffset = (s->pitch * srect->y) + (srect->x * 4);

	sdldata += coffset;
	sdata += soffset;
	cdata.argb8888.plane0 = (uint32_t *)sdldata;
	cdata.argb8888.plane0_stride = s->pitch / 4;
	/* convert */
	conv = enesim_converter_span_get(ENESIM_CONVERTER_ARGB8888, ENESIM_ANGLE_0, ENESIM_FORMAT_ARGB8888);

	_lock(s);
	while (h--)
	{
		conv(&cdata, srect->w, sdata);
		sdata += stride;
		cdata.argb8888.plane0 += cdata.argb8888.plane0_stride;
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

static Eon_Engine _sdl_engine = {
	.document_create = document_create,
	.document_delete = document_delete,
	.canvas_create = canvas_create,
	.canvas_flush = canvas_flush,
	.canvas_delete = canvas_delete,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void engine_sdl_init(void)
{
	eon_engine_enesim_setup(&_sdl_engine);
	eon_engine_register("sdl", &_sdl_engine);
}

void engine_sdl_shutdown(void)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

