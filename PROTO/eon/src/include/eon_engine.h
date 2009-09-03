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
#ifndef EON_ENGINE_H_
#define EON_ENGINE_H_

struct _Eon_Engine
{
	/* document callbacks */
	void * (*document_create)(Eon_Document *d, const char *options);
	void (*document_delete)(void *);
	/* canvas callbacks */
	void * (*canvas_create)(void *d, Eon_Canvas *c, Eina_Bool root, int w, int h);
	void (*canvas_delete)(void *);
	Eina_Bool (*canvas_blit)(void *sc, Eina_Rectangle *r, void *c, Eina_Rectangle *sr);
	Eina_Bool (*canvas_flush)(void *c, Eina_Rectangle *r);
	/* rect callbacks */
	void * (*rect_create)(Eon_Rect *r);
	void (*rect_render)(void *r, void *c, Eina_Rectangle *clip);
	void (*rect_delete)(void *r);
	/* circle callbacks */
	void * (*circle_create)(Eon_Circle *r);
	void (*circle_render)(void *r, void *c, Eina_Rectangle *clip);
	void (*circle_delete)(void *c);
	/* polygon callbacks */
	void * (*polygon_create)(Eon_Polygon *p);
	void (*polygon_point_add)(void *pd, int x, int y);
	void (*polygon_render)(void *p, void *c, Eina_Rectangle *clip);
	void (*polygon_reset)(void *p);
	void (*polygon_delete)(void *p);
	/* text callbacks */
	void * (*text_create)(Eon_Text *t);
	void (*text_render)(void *p, void *c, Eina_Rectangle *clip);
	/* image callbacks */
	void * (*image_create)(Eon_Image *i);
	Eina_Bool (*image_setup)(void *i, Eon_Shape *s);
	void (*image_delete)(void *i);
	/* hswitch callbacks */
	void * (*hswitch_create)(Eon_Hswitch *i);
	Eina_Bool (*hswitch_setup)(void *i, Eon_Shape *s);
	void (*hswitch_delete)(void *i);
	/* hswitch callbacks */
	void * (*fade_create)(Eon_Hswitch *i);
	Eina_Bool (*fade_setup)(void *i, Eon_Shape *s);
	void (*fade_delete)(void *i);
	/* sqpattern callbacks */
	void * (*checker_create)(Eon_Checker *i);
	Eina_Bool (*checker_setup)(void *i, Eon_Shape *s);
	void (*checker_delete)(void *i);
	/* debug */
	void (*debug_rect)(void *c, uint32_t color, int x, int y, int w, int h);
};


EAPI Ekeko_Type *eon_engine_type_get(void);

EAPI void * eon_engine_document_create(Eon_Engine *e, Eon_Document *d, const char *options);

EAPI Eina_Bool eon_engine_canvas_blit(Eon_Engine *e, void *sc, Eina_Rectangle *r, void *c, Eina_Rectangle *sr);
EAPI Eina_Bool eon_engine_canvas_flush(Eon_Engine *e, void *c, Eina_Rectangle *r);
EAPI void * eon_engine_canvas_create(Eon_Engine *e, void *dd, Eon_Canvas *c, Eina_Bool root, uint32_t w, uint32_t h);

EAPI void * eon_engine_rect_create(Eon_Engine *e, Eon_Rect *r);
EAPI void eon_engine_rect_render(Eon_Engine *e, void *r, void *c, Eina_Rectangle *clip);

EAPI void * eon_engine_circle_create(Eon_Engine *e, Eon_Circle *c);
EAPI void eon_engine_circle_render(Eon_Engine *e, void *r, void *c, Eina_Rectangle *clip);

EAPI void * eon_engine_polygon_create(Eon_Engine *e, Eon_Polygon *p);
EAPI void eon_engine_polygon_point_add(Eon_Engine *e, void *pd, int x, int y);
EAPI void eon_polygon_render(Eon_Engine *e, void *p, void *c, Eina_Rectangle *clip);

EAPI void * eon_engine_text_create(Eon_Engine *e, Eon_Text *t);
EAPI void eon_engine_text_render(Eon_Engine *e, void *c, void *t, Eina_Rectangle *clip);

EAPI void * eon_engine_image_create(Eon_Engine *e, Eon_Paint *p);
EAPI Eina_Bool eon_engine_image_setup(Eon_Engine *e, void *engine_data, Eon_Shape *s);
EAPI void eon_engine_image_delete(Eon_Engine *e, void *engine_data);

EAPI void * eon_engine_hswitch_create(Eon_Engine *e, Eon_Paint *p);
EAPI Eina_Bool eon_engine_hswitch_setup(Eon_Engine *e, void *engine_data, Eon_Shape *s);
EAPI void eon_engine_hswitch_delete(Eon_Engine *e, void *engine_data);

EAPI void * eon_engine_fade_create(Eon_Engine *e, Eon_Paint *p);
EAPI Eina_Bool eon_engine_fade_setup(Eon_Engine *e, void *engine_data, Eon_Shape *s);
EAPI void eon_engine_fade_delete(Eon_Engine *e, void *engine_data);

EAPI void * eon_engine_checker_create(Eon_Engine *e, Eon_Paint *p);
EAPI Eina_Bool eon_engine_checker_setup(Eon_Engine *e, void *engine_data, Eon_Shape *s);
EAPI void eon_engine_checker_delete(Eon_Engine *e, void *engine_data);

EAPI void eon_engine_debug_rect(Eon_Engine *e, void *c, uint32_t color, int x, int y, int w, int h);

#endif /* EON_ENGINE_H_ */
