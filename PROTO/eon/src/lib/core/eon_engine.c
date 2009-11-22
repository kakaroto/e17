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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Hash *_engines = NULL;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_engine_init(void)
{
	_engines = eina_hash_string_superfast_new(NULL);
	/* now the canvas engines */
#ifdef BUILD_ENGINE_SDL
	engine_sdl_init();
#endif
#ifdef BUILD_ENGINE_IPC
	engine_remote_init();
#endif
#ifdef BUILD_ENGINE_BUFFER
	engine_buffer_init();
#endif
}

void eon_engine_shutdown(void)
{
	/* TODO remove the hash */
}

void eon_engine_register(const char *name, Eon_Engine *e)
{
	eina_hash_add(_engines, name, e);
}

Eon_Engine * eon_engine_get(const char *name)
{
	Ekeko_Object *o;
	Ekeko_Type *t;
	Eon_Type_Constructor n;

	return eina_hash_find(_engines, name);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void * eon_engine_document_create(Eon_Engine *e, Eon_Document *d,
		int w, int h, const char *options)
{
	return e->document_create(d, w, h, options);
}

EAPI void * eon_engine_layout_create(Eon_Engine *e, Eon_Layout *l, void *dd, uint32_t w, uint32_t h)
{
	return e->layout_create(l, dd, w, h);
}

EAPI Eina_Bool eon_engine_layout_blit(Eon_Engine *e, void *sc, Eina_Rectangle *sr, void *c, Eina_Rectangle *r)
{
	return e->layout_blit(sc, sr, c, r);
}

EAPI Eina_Bool eon_engine_layout_flush(Eon_Engine *e, void *c, Eina_Rectangle *r)
{
	return e->layout_flush(c, r);
}

EAPI void eon_engine_layout_delete(Eon_Engine *e, void *c)
{
	return e->layout_delete(c);
}

EAPI void * eon_engine_rect_create(Eon_Engine *e, Eon_Rect *r)
{
	return e->rect_create(r);
}

EAPI void eon_engine_rect_render(Eon_Engine *e, void *r, void *c, Eina_Rectangle *clip)
{
	e->rect_render(r, c, clip);

}

EAPI void * eon_engine_circle_create(Eon_Engine *e, Eon_Circle *c)
{
	return e->circle_create(c);
}

EAPI void eon_engine_circle_render(Eon_Engine *e, void *r, void *c, Eina_Rectangle *clip)
{
	e->circle_render(r, c, clip);

}

EAPI void * eon_engine_polygon_create(Eon_Engine *e, Eon_Polygon *p)
{
	return e->polygon_create(p);
}

EAPI void eon_engine_polygon_point_add(Eon_Engine *e, void *pd, int x, int y)
{
	e->polygon_point_add(pd, x, y);
}

EAPI void eon_engine_polygon_render(Eon_Engine *e, void *p, void *c, Eina_Rectangle *clip)
{
	e->polygon_render(p, c, clip);
}

EAPI void * eon_engine_text_create(Eon_Engine *e, Eon_Text *t)
{
	return e->text_create(t);
}

EAPI void eon_engine_text_render(Eon_Engine *e, void *t, void *c, Eina_Rectangle *clip)
{
	e->text_render(t, c, clip);
}

/* Paint objects */
EAPI void * eon_engine_image_create(Eon_Engine *e, Eon_Paint *p)
{
	return e->image_create((Eon_Image *)p);
}

EAPI void eon_engine_image_render(Eon_Engine *e, void *d, void *c, Eina_Rectangle *clip)
{
	return e->image_render(d, c, clip);
}

EAPI void eon_engine_image_delete(Eon_Engine *e, void *engine_data)
{
	e->image_delete(engine_data);
}

EAPI void * eon_engine_hswitch_create(Eon_Engine *e, Eon_Paint *p)
{
	return e->hswitch_create((Eon_Hswitch *)p);
}

EAPI void eon_engine_hswitch_render(Eon_Engine *e, void *d, void *c, Eina_Rectangle *clip)
{
	return e->hswitch_render(d, c, clip);
}

EAPI void eon_engine_hswitch_delete(Eon_Engine *e, void *engine_data)
{
	e->hswitch_delete(engine_data);
}

EAPI void * eon_engine_fade_create(Eon_Engine *e, Eon_Paint *p)
{
	return e->fade_create((Eon_Hswitch *)p);
}

EAPI void eon_engine_fade_render(Eon_Engine *e, void *d, void *c, Eina_Rectangle *clip)
{
	return e->fade_render(d, c, clip);
}

EAPI void eon_engine_fade_delete(Eon_Engine *e, void *engine_data)
{
	e->fade_delete(engine_data);
}

EAPI void * eon_engine_checker_create(Eon_Engine *e, Eon_Paint *p)
{
	return e->checker_create((Eon_Checker *)p);
}

EAPI void eon_engine_checker_render(Eon_Engine *e, void *d, void *c, Eina_Rectangle *clip)
{
	return e->checker_render(d, c, clip);
}

EAPI void eon_engine_checker_delete(Eon_Engine *e, void *engine_data)
{
	e->checker_delete(engine_data);
}

EAPI void * eon_engine_stripes_create(Eon_Engine *e, Eon_Paint *p)
{
	return e->stripes_create((Eon_Stripes *)p);
}

EAPI void eon_engine_stripes_render(Eon_Engine *e, void *d, void *c, Eina_Rectangle *clip)
{
	return e->stripes_render(d, c, clip);
}

EAPI void eon_engine_stripes_delete(Eon_Engine *e, void *engine_data)
{
	e->stripes_delete(engine_data);
}

EAPI void * eon_engine_compound_create(Eon_Engine *e, Eon_Paint *p)
{
	return e->compound_create((Eon_Stripes *)p);
}

EAPI void eon_engine_compound_render(Eon_Engine *e, void *d, void *c, Eina_Rectangle *clip)
{
	return e->compound_render(d, c, clip);
}

EAPI void eon_engine_compound_delete(Eon_Engine *e, void *engine_data)
{
	e->compound_delete(engine_data);
}

EAPI void * eon_engine_grid_create(Eon_Engine *e, Eon_Paint *p)
{
	return e->grid_create((Eon_Stripes *)p);
}

EAPI void eon_engine_grid_render(Eon_Engine *e, void *d, void *c, Eina_Rectangle *clip)
{
	return e->grid_render(d, c, clip);
}

EAPI void eon_engine_grid_delete(Eon_Engine *e, void *engine_data)
{
	e->grid_delete(engine_data);
}

EAPI void * eon_engine_buffer_create(Eon_Engine *e, Eon_Paint *p)
{
	return e->buffer_create((Eon_Stripes *)p);
}

EAPI void eon_engine_buffer_render(Eon_Engine *e, void *d, void *c, Eina_Rectangle *clip)
{
	return e->buffer_render(d, c, clip);
}

EAPI void eon_engine_buffer_delete(Eon_Engine *e, void *engine_data)
{
	e->buffer_delete(engine_data);
}

EAPI void eon_engine_buffer_update(Eon_Engine *e, void *engine_data)
{
	e->buffer_update(engine_data);
}

/* Debug */
EAPI void eon_engine_debug_rect(Eon_Engine *e, void *c, uint32_t color, int x,  int y, int w, int h)
{
	e->debug_rect(c, color, x, y, w, h);
}

