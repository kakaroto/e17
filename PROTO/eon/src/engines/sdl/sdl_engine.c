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
#define SINGLE_BUFFER


typedef struct _Eon_SDL
{
	Eon_Document *doc;
	SDL_Surface *s; /* main surface */
	 // FIXME remove this?
	Eon_Canvas *root;
	/* options */
	char resizable:1;
	char dbuffer:1;
	Ecore_Event_Handler *handler_rz;
	/* mouse, key, etc handlers */
	Ecore_Event_Handler *handlers[7];
} Eon_SDL;

static void _sdl_surface_new(Eon_SDL *sdoc, int w, int h)
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

static Eina_Bool _sdl_event(void *data)
{
	ecore_sdl_feed_events();
	return EINA_TRUE;
}

static Eina_Bool _resize_cb(void *data, int type, void *event)
{
	Ecore_Sdl_Event_Video_Resize *e = event;
	Eon_SDL *sdoc = data;

	eon_document_resize(sdoc->doc, e->w, e->h);
	_sdl_surface_new(sdoc, e->w, e->h);

	return EINA_TRUE;
}

static Eina_Bool _mouse_down_cb(void *data, int type, void *event)
{
	Eon_Input *input = (Eon_Input *)data;
	eon_input_feed_mouse_down(input);
	return EINA_TRUE;
}

static Eina_Bool _mouse_up_cb(void *data, int type, void *event)
{
	Eon_Input *input = (Eon_Input *)data;
	eon_input_feed_mouse_up(input);
	return EINA_TRUE;
}

static Eina_Bool _mouse_move_cb(void *data, int type, void *event)
{
	Eon_Input *input = (Eon_Input *)data;
	Ecore_Event_Mouse_Move *e = event;

	eon_input_feed_mouse_move(input, e->x, e->y);
	return EINA_TRUE;
}

static Eina_Bool _key_down_cb(void *data, int type, void *event)
{
	Eon_Input *input = (Eon_Input *)data;

	eon_input_feed_key_down(input, 0, 0);
	return EINA_TRUE;
}

static Eina_Bool _key_up_cb(void *data, int type, void *event)
{
	Eon_Input *input = (Eon_Input *)data;

	eon_input_feed_key_up(input, 0, 0);
	return EINA_TRUE;
}

static Eina_Bool _in_cb(void *data, int type, void *event)
{
	Eon_Input *input = (Eon_Input *)data;
	return EINA_TRUE;
}
static Eina_Bool _out_cb(void *data, int type, void *event)
{
	Eon_Input *input = (Eon_Input *)data;
	return EINA_TRUE;
}

static void _root_canvas_create(Eon_Layout *c)
{
	Eon_Input *input;

	/* Ecore_sdl interval isnt enough */
	SDL_EnableKeyRepeat(10, 10);
	/* add the input */
	input = eon_layout_input_new((Eon_Layout *)c);
	/* add the callbacks */
	ecore_event_handler_add(ECORE_SDL_EVENT_GOT_FOCUS, _in_cb, input);
	ecore_event_handler_add(ECORE_SDL_EVENT_LOST_FOCUS, _out_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, _mouse_down_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, _mouse_up_cb, input);
	ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, _mouse_move_cb, input);
	ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, _key_down_cb, input);
	ecore_event_handler_add(ECORE_EVENT_KEY_UP, _key_up_cb, input);
	/* FIXME Ecore_SDL doesnt support the in/out events, make it always in */
	eon_input_feed_mouse_in(input);
}

Eina_Bool _sdl_flush(void *data, Enesim_Surface *src, Eina_Rectangle *rect)
{
	Eon_SDL *sdl = (Eon_SDL *)data;
	SDL_Surface *dst;

	Enesim_Converter_1D conv;
	Enesim_Converter_Data cdata;
	uint32_t *src_data;
	uint8_t *dst_data;

	int h = rect->h;
	int stride;
	int inc;
	int numcpus;
	int soffset;
	int coffset;

#if 1
	printf("Flushing the surface %p %d %d %d %d\n", src, rect->x, rect->y, rect->w, rect->h);
#endif
#if 0
	/* setup the pointers */
	stride = enesim_surface_stride_get(src);
	soffset = (stride * rect->y) + rect->x;

	sdata = enesim_surface_data_get(es);
	sdldata = ss->pixels;

	coffset = (ss->pitch * rect->y) + (rect->x * 4);

	sdldata += coffset;
	sdata += soffset;
	cdata.argb8888.plane0 = (uint32_t *)sdldata;
	cdata.argb8888.plane0_stride = s->pitch / 4;
	/* convert */
	conv = enesim_converter_span_get(ENESIM_CONVERTER_ARGB8888, ENESIM_ANGLE_0, ENESIM_FORMAT_ARGB8888);
	SDL_LockSurface(ss);
	while (h--)
	{
		conv(&cdata, rect->w, sdata);
		sdata += stride;
		cdata.argb8888.plane0 += cdata.argb8888.plane0_stride;
	}
	SDL_UnlockSurface(ss);

#ifdef SINGLE_BUFFER
	SDL_UpdateRect(s, rect->x, rect->y, rect->w, rect->h);
	return EINA_FALSE;
#else
	SDL_Flip(s);
	return EINA_TRUE;
#endif
#endif
}

void * _sdl_setup(Eon_Document *doc, int w, int h, const char *options)
{
	Eon_SDL *sdl;
	Uint32 flags = SDL_SRCALPHA;

	printf("[SDL] Initializing SDL\n");
	sdl = calloc(1, sizeof(Eon_SDL));
	if (options)
	{
		if (!strcmp(options, "resizable"))
			sdl->resizable = 1;
	}
	sdl->doc = doc;
	ecore_sdl_init(NULL);
	SDL_Init(SDL_INIT_VIDEO);
	_sdl_surface_new(sdl, w, h);
	/* the event feeder evas/ecore has a very weird way to feed sdl events! */
	ecore_timer_add(0.008, _sdl_event, NULL);
	/* called whenever the wm changes the window */
	ecore_event_handler_add(ECORE_SDL_EVENT_RESIZE, _resize_cb, sdl);

	return sdl;
}

void _sdl_cleanup(void *data)
{
	ecore_sdl_shutdown();
}

static Eon_Engine _sdl_engine = {
	.setup = _sdl_setup,
	.cleanup = _sdl_cleanup,
	.flush = _sdl_flush,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void engine_sdl_init(void)
{
	eon_engine_register("sdl", &_sdl_engine);
}

void engine_sdl_shutdown(void)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

