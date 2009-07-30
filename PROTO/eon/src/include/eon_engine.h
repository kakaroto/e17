#ifndef EON_ENGINE_H_
#define EON_ENGINE_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Engine_Private Eon_Engine_Private;
struct _Eon_Engine
{
	Ekeko_Object parent;
	Eon_Engine_Private *private;
	/* document callbacks */
	void * (*document_create)(Eon_Document *d);
	void (*document_delete)(void *);
	/* canvas callbacks */
	void * (*canvas_create)(Eon_Canvas *c, Eina_Bool root, int w, int h);
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
	void * (*sqpattern_create)(Eon_Checker *i);
	Eina_Bool (*sqpattern_setup)(void *i, Eon_Shape *s);
	void (*sqpattern_delete)(void *i);
	/* debug */
	void (*debug_rect)(void *c, uint32_t color, int x, int y, int w, int h);
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_engine_type_get(void);

EAPI void * eon_engine_document_create(Eon_Engine *e, Eon_Document *d);

EAPI Eina_Bool eon_engine_canvas_blit(Eon_Engine *e, void *sc, Eina_Rectangle *r, void *c, Eina_Rectangle *sr);
EAPI Eina_Bool eon_engine_canvas_flush(Eon_Engine *e, void *c, Eina_Rectangle *r);
EAPI void * eon_engine_canvas_create(Eon_Engine *e, Eon_Canvas *c, Eina_Bool root, uint32_t w, uint32_t h);

EAPI void * eon_engine_rect_create(Eon_Engine *e, Eon_Rect *r);
EAPI void eon_engine_rect_render(Eon_Engine *e, void *r, void *c, Eina_Rectangle *clip);

EAPI void * eon_engine_circle_create(Eon_Engine *e, Eon_Circle *c);
EAPI void eon_engine_circle_render(Eon_Engine *e, void *r, void *c, Eina_Rectangle *clip);

EAPI void * eon_engine_polygon_create(Eon_Engine *e, Eon_Polygon *p);
EAPI void eon_engine_polygon_point_add(Eon_Engine *e, void *pd, int x, int y);
EAPI void eon_polygon_render(Eon_Engine *e, void *p, void *c, Eina_Rectangle *clip);

EAPI void * eon_engine_image_create(Eon_Engine *e, Eon_Paint *p);
EAPI Eina_Bool eon_engine_image_setup(Eon_Engine *e, void *engine_data, Eon_Shape *s);
EAPI void eon_engine_image_delete(Eon_Engine *e, void *engine_data);

EAPI void * eon_engine_hswitch_create(Eon_Engine *e, Eon_Paint *p);
EAPI Eina_Bool eon_engine_hswitch_setup(Eon_Engine *e, void *engine_data, Eon_Shape *s);
EAPI void eon_engine_hswitch_delete(Eon_Engine *e, void *engine_data);

EAPI void * eon_engine_fade_create(Eon_Engine *e, Eon_Paint *p);
EAPI Eina_Bool eon_engine_fade_setup(Eon_Engine *e, void *engine_data, Eon_Shape *s);
EAPI void eon_engine_fade_delete(Eon_Engine *e, void *engine_data);

EAPI void * eon_engine_sqpattern_create(Eon_Engine *e, Eon_Paint *p);
EAPI Eina_Bool eon_engine_sqpattern_setup(Eon_Engine *e, void *engine_data, Eon_Shape *s);
EAPI void eon_engine_sqpattern_delete(Eon_Engine *e, void *engine_data);

EAPI void eon_engine_debug_rect(Eon_Engine *e, void *c, uint32_t color, int x, int y, int w, int h);

#endif /* EON_ENGINE_H_ */
