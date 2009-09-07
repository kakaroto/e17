#include "Eon.h"
#include "eon_private.h"

#include "Eon_Enesim.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <limits.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static FT_Library library;

/*============================================================================*
 *                                  Paint                                    *
 *============================================================================*/
/* FIXME this should be the new renderer interface on enesim */
typedef struct Paint
{
	Enesim_Renderer *r;
	Eon_Paint *p;
	void *data;
} Paint;

static inline void length_calculate(Eon_Coord *sl, int plength, int *l)
{
	if (!l)
		return;
	if (sl->type == EON_COORD_RELATIVE)
	{
		*l = sl->value * plength / 100;
	}
	else
	{
		*l = sl->value;
	}
}
static inline void coord_calculate(Eon_Coord *sc, int pc, int plength, int *c)
{
	if (!c)
		return;
	if (sc->type == EON_COORD_RELATIVE)
	{
		*c = pc + ((sc->value * plength) / 100);
	}
	else
	{
		*c = sc->value;
	}
}

static void paint_coords_get(Eon_Paint *p, Eon_Shape *s, int *x, int *y, int *w,
		int *h)
{
	Eon_Coord px, py, pw, ph;
	Eina_Rectangle geom;

	/* setup the renderer correctly */
	if (eon_paint_coordspace_get(p) == EON_COORDSPACE_OBJECT)
	{
		ekeko_renderable_geometry_get((Ekeko_Renderable *)s, &geom);
	}
	else
	{
		/* FIXME we should get the topmost canvas uints not the parent
		 * canvas
		 */
		ekeko_renderable_geometry_get((Ekeko_Renderable *)eon_shape_canvas_topmost_get(s), &geom);
	}
	eon_paint_coords_get(p, &px, &py, &pw, &ph);
	coord_calculate(&px, geom.x, geom.w, x);
	coord_calculate(&py, geom.y, geom.h, y);
	length_calculate(&pw, geom.w, w);
	length_calculate(&ph, geom.h, h);
#if 0
	printf("CSPACE = %d [%d %d %d %d] -> %d %d %d %d\n", eon_paint_coordspace_get(p), geom.x, geom.y, geom.w, geom.h, x ? *x : -1, y ? *y : -1, w ? *w : -1, h ? *h : -1);
#endif
}

static void paint_setup(Paint *p, int ox, int oy)
{
	Enesim_Matrix m;

	eon_paint_matrix_inv_get(p->p, &m);
	enesim_renderer_transform_set(p->r, &m);
	enesim_renderer_origin_set(p->r, ox, oy);
}
/*============================================================================*
 *                                 Horswitch                                  *
 *============================================================================*/
static void * hswitch_create(Eon_Hswitch *hs)
{
	Paint *p;

	p = calloc(1, sizeof(Paint));
	p->p = (Eon_Paint *)hs;
	p->r = enesim_renderer_hswitch_new();

	return p;
}

static Eina_Bool hswitch_setup(void *data, Eon_Shape *s)
{
	Paint *p = data;
	Paint *tmp;
	Eon_Hswitch *hs = (Eon_Hswitch *)p->p;
	Eon_Paint *p1, *p2;
	float step;
	int dw, dh;

	if (!eon_transition_paint_get((Eon_Transition *)hs, &p1, &p2, &step))
		return EINA_FALSE;
	if (!eon_paint_setup(p1, s))
		return EINA_FALSE;
	if (!eon_paint_setup(p2, s))
		return EINA_FALSE;

	paint_coords_get(p->p, s, NULL, NULL, &dw, &dh);
	tmp = eon_paint_engine_data_get(p1);
	enesim_renderer_hswitch_left_set(p->r, tmp->r);
	tmp = eon_paint_engine_data_get(p2);
	enesim_renderer_hswitch_right_set(p->r, tmp->r);
	enesim_renderer_hswitch_w_set(p->r, dw);
	enesim_renderer_hswitch_h_set(p->r, dh);
	enesim_renderer_hswitch_step_set(p->r, step);
	return EINA_TRUE;
}

static void hswitch_delete(void *data)
{
	Paint *p = data;

	enesim_renderer_delete(p->r);
	free(p);
}
/*============================================================================*
 *                                   Fade                                     *
 *============================================================================*/
static void * fade_create(Eon_Fade *f)
{
	Paint *p;

	p = calloc(1, sizeof(Paint));
	p->p = (Eon_Paint *)f;
#if 0
	p->r = enesim_renderer_transition_new();
#endif
	return p;
}

static Eina_Bool fade_setup(void *data, Eon_Shape *s)
{
	Paint *p = data;
	Paint *tmp;
	Eon_Fade *f = (Eon_Fade *)p->p;
	Eon_Paint *p1, *p2;
	float step;
	int dw, dh;
#if 0
	if (!eon_transition_paint_get((Eon_Transition *)f, &p1, &p2, &step))
		return EINA_FALSE;
	if (!eon_paint_setup(p1, s))
		return EINA_FALSE;
	if (!eon_paint_setup(p2, s))
		return EINA_FALSE;

	paint_coords_get(p->p, s, NULL, NULL, &dw, &dh);
	tmp = eon_paint_engine_data_get(p1);
	enesim_renderer_transition_source_set(p->r, tmp->r);
	tmp = eon_paint_engine_data_get(p2);
	enesim_renderer_transition_target_set(p->r, tmp->r);
	enesim_renderer_transition_value_set(p->r, step);
#endif
	return EINA_TRUE;
}

static void fade_delete(void *data)
{
	Paint *p = data;

	enesim_renderer_delete(p->r);
	free(p);
}
/*============================================================================*
 *                                 Sqpattern                                  *
 *============================================================================*/
static void * checker_create(Eon_Checker *sq)
{
	Paint *p;

	p = calloc(1, sizeof(Paint));
	p->p = (Eon_Paint *)sq;
	p->r = enesim_renderer_checker_new();

	return p;
}

static Eina_Bool checker_setup(void *data, Eon_Shape *s)
{
	Paint *p = data;
	Eon_Checker *sq = (Eon_Checker *)p->p;
	int dx, dy;

	paint_coords_get(p->p, s, &dx, &dy, NULL, NULL);
	paint_setup(p, dx, dy);
	enesim_renderer_checker_color1_set(p->r, eon_checker_color1_get(sq));
	enesim_renderer_checker_color2_set(p->r, eon_checker_color2_get(sq));
	enesim_renderer_checker_size_set(p->r, 20, 20);
	return EINA_TRUE;
}

static void checker_delete(void *data)
{
	Paint *p = data;

	enesim_renderer_delete(p->r);
	free(p);
}
/*============================================================================*
 *                                   Image                                    *
 *============================================================================*/

/* FIXME do a real pattern! */
static void _image_pattern_span(void *data, void *span, int x, int y, unsigned int len)
{
	uint32_t *dst = span;
	uint32_t color[2];
	const int size = 10;

	if ((y / size) % 2)
	{
		color[0] = 0xff555555;
		color[1] = 0x00000000;
	}
	else
	{
		color[1] = 0xff555555;
		color[0] = 0x00000000;
	}
	while (len--)
	{
		*dst = color[(x / size) % 2];
		dst++;
		x++;
	}
}

static void * image_create(Eon_Image *i)
{
	Paint *p;

	p = calloc(1, sizeof(Paint));
	p->p = (Eon_Paint *)i;
	p->r = enesim_renderer_surface_new();

	return p;
}

static Eina_Bool image_setup(void *data, Eon_Shape *s)
{
	Paint *p = data;
	Eon_Image *i = (Eon_Image *)p->p;
	int dw, dh;
	int dx, dy;

	if (!eon_image_loaded(i))
	{
		return EINA_FALSE;
	}
	paint_coords_get(p->p, s, &dx, &dy, &dw, &dh);
	paint_setup(p, dx, dy);
	enesim_renderer_surface_w_set(p->r, dw);
	enesim_renderer_surface_h_set(p->r, dh);
	enesim_renderer_origin_set(p->r, dx, dy);
	enesim_renderer_surface_src_set(p->r, eon_image_surface_get(i));

	return EINA_TRUE;
}

static void image_delete(void *i)
{
	Paint *p = i;

	enesim_renderer_delete(p->r);
	//free(p->data);
	free(p);
}

/*============================================================================*
 *                                  Common                                    *
 *============================================================================*/
/* TODO add all the possible compositor parameters */
typedef struct Shape_Drawer_Data
{
	Enesim_Surface *dst;
	Enesim_Compositor_Span span;
	Eon_Color color;
	/* the sape information */
	Eon_Shape *shape;
	/* the callback */
	Enesim_Scanline_Callback cb;
	/* paint engine data */
	Paint *paint;
} Shape_Drawer_Data;

static void shape_renderer_draw(Eon_Shape *s, Enesim_Surface *dst,  Enesim_Renderer *r, Eina_Rectangle *clip)
{
	Enesim_Compositor_Span span;
	Enesim_Rop rop;
	Eon_Color color;
	int dh, dy;
	uint32_t *ddata;
	uint32_t *fdata;
	int stride;
	Enesim_Format dfmt;

	dfmt = enesim_surface_format_get(dst);
	ddata = enesim_surface_data_get(dst);
	stride = enesim_surface_stride_get(dst);
	ddata = ddata + (clip->y * stride) + clip->x;

	color = eon_shape_color_get(s);
	span = enesim_compositor_span_get(rop, &dfmt, ENESIM_FORMAT_ARGB8888, color, ENESIM_FORMAT_NONE);

	dh = clip->h;
	dy = clip->y;
	/* fill the new span */
	while (dh--)
	{
		fdata = calloc(clip->w, sizeof(uint32_t));
		enesim_renderer_span_fill(r, clip->x, dy, clip->w, fdata);
		/* compose the filled and the destination spans */
		span(ddata, clip->w, fdata, color, NULL);
		dy++;
		ddata += stride;
		free(fdata);
	}
	/* TODO set the format again */
}

static void aliased_color_cb(Enesim_Scanline *sl, void *data)
{
	Shape_Drawer_Data *sdd = data;
	uint32_t *ddata;
	uint32_t stride;

	ddata = enesim_surface_data_get(sdd->dst);
	stride = enesim_surface_stride_get(sdd->dst);
	ddata = ddata + (sl->data.alias.y * stride) + sl->data.alias.x;
	sdd->span(ddata, sl->data.alias.w, NULL, sdd->color, NULL);
}

static void aliased_fill_cb(Enesim_Scanline *sl, void *data)
{
	Shape_Drawer_Data *sdd = data;
	int px, py;
	uint32_t *ddata;
	uint32_t stride;
	uint32_t *fdata;

	ddata = enesim_surface_data_get(sdd->dst);
	stride = enesim_surface_stride_get(sdd->dst);
	ddata = ddata + (sl->data.alias.y * stride) + sl->data.alias.x;

	/* fill the new span */
	fdata = calloc(sl->data.alias.w, sizeof(uint32_t));
	/* match the coordinates */
	paint_coords_get(sdd->paint->p, sdd->shape, &px, &py, NULL, NULL);
	/* clip the paint coordinates to the shape's ones, only pass those */
	enesim_renderer_span_fill(sdd->paint->r, sl->data.alias.x, sl->data.alias.y, sl->data.alias.w, fdata);
	//enesim_renderer_span_fill(sdd->paint->r, sl->data.alias.x - px, sl->data.alias.y - py, sl->data.alias.w, fdata);

	/* compose the filled and the destination spans */
	sdd->span(ddata, sl->data.alias.w, fdata, sdd->color, NULL);
	free(fdata);
}

static void shape_renderer_setup(Eon_Shape *s, Enesim_Renderer *r)
{
	Eon_Paint *p;
	Paint *pa;
	float stroke;
	Eon_Color color;

	/* the fill properties */
	p = eon_shape_fill_paint_get(s);
	if (p)
	{
		pa = eon_paint_engine_data_get(p);
		enesim_renderer_shape_fill_renderer_set(r, pa->r);
	}
	/* the stroke properties */
	stroke = eon_shape_stroke_width_get(s);
	color = eon_shape_stroke_color_get(s);
	enesim_renderer_shape_outline_weight_set(r, stroke);
	enesim_renderer_shape_outline_color_set(r, color);
	/* common fill/stroke properties */
	enesim_renderer_shape_draw_mode_set(r, ENESIM_SHAPE_DRAW_MODE_STROKE_FILL);
}

static void shape_setup(Eon_Shape *s, Shape_Drawer_Data *d, Enesim_Surface *dst)
{
	Enesim_Rop rop;
	Eon_Paint *p;
	Enesim_Format dfmt;

	dfmt = enesim_surface_format_get(dst);
	rop = eon_shape_rop_get(s);
	d->color = eon_shape_color_get(s);
	d->dst = dst;
	d->shape = s;
	p = eon_shape_fill_paint_get(s);
	/* color based */
	if (!p)
	{
		d->span = enesim_compositor_span_get(rop, &dfmt, ENESIM_FORMAT_NONE, d->color, ENESIM_FORMAT_NONE);
		d->cb = aliased_color_cb;
	}
	/* paint based */
	else
	{
		/* TODO paint + color */
		d->paint = eon_paint_engine_data_get(p);
		enesim_renderer_state_setup(d->paint->r);
		d->span = enesim_compositor_span_get(rop, &dfmt, ENESIM_FORMAT_ARGB8888, d->color, ENESIM_FORMAT_NONE);
		d->cb = aliased_fill_cb;
	}
	/* TODO set the format again */
}
/*============================================================================*
 *                                  Polygon                                   *
 *============================================================================*/
typedef struct Polygon
{
	Eon_Polygon *p;
	Enesim_Rasterizer *r;
} Polygon;

static void * polygon_create(Eon_Polygon *ep)
{
	Polygon *p = calloc(1, sizeof(Polygon));
	p->p = ep;
	/* FIXME alias by now */
	p->r = enesim_rasterizer_cpsc_new();

	return p;
}

static void polygon_point_add(void *pd, int x, int y)
{
	Polygon *p = pd;

	/* create a rasterizer */
	/* TODO check the quality to create a kiia or a cpsc rasterizer */
	/* add a vertex to it */
	enesim_rasterizer_vertex_add(p->r, x, y);
}

static void polygon_render(void *pd, void *cd, Eina_Rectangle *clip)
{
	Polygon *p = pd;
	Shape_Drawer_Data sdd;

	shape_setup((Eon_Shape *)p->p, &sdd, cd);
	enesim_rasterizer_generate(p->r, clip, sdd.cb, &sdd);
}

static void polygon_delete(void *ep)
{
	Polygon *p = ep;

	enesim_rasterizer_delete(p->r);
	free(p);
}
/*============================================================================*
 *                                   Rect                                     *
 *============================================================================*/
typedef struct Rectangle
{
	Eon_Rect *er;
	Enesim_Renderer *r;
} Rectangle;

static void * rect_create(Eon_Rect *er)
{
	Rectangle *r;

	r = malloc(sizeof(Rectangle));
	r->er = er;
	r->r = enesim_renderer_rectangle_new();

	return r;
}

static void rect_render(void *er, void *cd, Eina_Rectangle *clip)
{
	Rectangle *r = er;
	Eina_Rectangle geom;
	float rad;

	shape_renderer_setup((Eon_Shape *)r->er, r->r);
	ekeko_renderable_geometry_get((Ekeko_Renderable *)r->er, &geom);
	rad = eon_rect_corner_radius_get(r->er);

	enesim_renderer_origin_set(r->r, geom.x, geom.y);
	enesim_renderer_rectangle_size_set(r->r, geom.w, geom.h);
	enesim_renderer_rectangle_corner_radius_set(r->r, rad);
	/* FIXME fix this */
	enesim_renderer_rectangle_corners_set(r->r, 1, 1, 1, 1);
	enesim_renderer_state_setup(r->r);
	shape_renderer_draw((Eon_Shape *)r->er, cd, r->r, clip);
}
/*============================================================================*
 *                                 Circle                                     *
 *============================================================================*/
typedef struct Circle
{
	Eon_Circle *c;
	Enesim_Renderer *r;
} Circle;

static void * circle_create(Eon_Circle *ec)
{
	Circle *c;

	c = malloc(sizeof(Circle));
	c->c = ec;
	c->r = enesim_renderer_circle_new();

	return c;
}

/* TODO instead of reseting the rasterizer and add the vertex every time
 * just add a callback for radius, x and y change and reset there
 */
static void circle_render(void *ec, void *cd, Eina_Rectangle *clip)
{
	Circle *c = ec;
	Eon_Coord x;
	Eon_Coord y;
	float radius;
	int dh, dy;

	shape_renderer_setup((Eon_Shape *)c->c, c->r);
	eon_circle_x_get(c->c, &x);
	eon_circle_y_get(c->c, &y);
	radius = eon_circle_radius_get(c->c);
	enesim_renderer_circle_center_set(c->r, x.final, y.final);
	enesim_renderer_circle_radius_set(c->r, radius);
	enesim_renderer_state_setup(c->r);
	shape_renderer_draw((Eon_Shape *)c->c, cd, c->r, clip);
}

static void circle_delete(void *ec)
{
	Circle *c = ec;

	enesim_renderer_delete(c->r);
	free(ec);
}
/*============================================================================*
 *                                  Text                                      *
 *============================================================================*/
/* Enesim doesnt have a text rasterizer, we use freetype here */
typedef struct Text
{
	Eon_Text *t;
	FT_Face face;
} Text;


static void * text_create(Eon_Text *et)
{
	Text *t;
	FT_Error error;

	/* FIXME this is hardcoded for now, we need to create an eon_font
	 * abstraction
	 */
	t = malloc(sizeof(Text));
	t->t = et;
	error = FT_New_Face(library, "/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf", 0, &t->face);

	if (error)
	{
		printf("Error loading the font\n");
		free(t);
		return NULL;
	}
	FT_Set_Pixel_Sizes(t->face, /* handle to face object */
		0, /* pixel_width */
		16 ); /* pixel_height */

	return t;
}

static void text_boundings_get(void *et, Eina_Rectangle *r)
{

}

static void text_compose(Text *t, FT_Bitmap *bmp, Enesim_Surface *dst, Enesim_Compositor_Span span, int left, int top)
{
	Enesim_Color color;
	uint32_t *ddata;
	int dstride;
	int y = bmp->rows;
	uint8_t *mask = bmp->buffer;

	color = eon_shape_color_get((Eon_Shape *)t->t);
	ddata = enesim_surface_data_get(dst);
	dstride = enesim_surface_stride_get(dst);

	ddata = ddata + (top * dstride) + left;
	//printf("compose at %d %d %p %d\n", left, top, span, y);
	while (y--)
	{
		span(ddata, bmp->width, NULL, color, mask);
		mask += bmp->pitch;
		ddata += dstride;
	}
}

static void text_render(void *et, void *cd, Eina_Rectangle *clip)
{
	FT_Error error;
	Text *t = et;
	char *str;
	int i;
	FT_GlyphSlot slot;
	int pen_x;
	int pen_y;
	Enesim_Compositor_Span span;
	Enesim_Color color;
	Enesim_Rop rop;
	Enesim_Format fmt;
	Enesim_Surface *dst = cd;
	Eina_Rectangle geom;

	if (!t)
		return;
	str = eon_text_string_get(t->t);
	/* TODO Move this into the shape object */
	if (!str)
		return;


	ekeko_renderable_geometry_get((Ekeko_Renderable *)t->t, &geom);
	pen_x = geom.x;
	pen_y = geom.y + 16;

	color = eon_shape_color_get((Eon_Shape *)t->t);
	rop = eon_shape_rop_get((Eon_Shape *)t->t);
	fmt = enesim_surface_format_get(dst);
	span = enesim_compositor_span_get(rop, &fmt, ENESIM_FORMAT_NONE, color, ENESIM_FORMAT_A8);

	slot = t->face->glyph;
	for (i = 0; str[i]; i++)
	{
		error = FT_Load_Char(t->face, str[i], FT_LOAD_RENDER);
		if (error)
		{
			printf("error %d\n", error);
			continue; /* ignore errors */
		}
		//printf("%c %d %d\n", str[i], slot->bitmap_left, slot->bitmap_top);
		text_compose(t, &slot->bitmap, cd, span,
				pen_x + slot->bitmap_left, pen_y - slot->bitmap_top);
		/* increment pen position */
		pen_x += slot->advance.x >> 6;
	}
	/* TODO set the format again */
}

/*============================================================================*
 *                                  Debug                                     *
 *============================================================================*/
static void debug_rect(void *cd, uint32_t color, int x, int y, int w, int h)
{
	Enesim_Compositor_Span span;
	Enesim_Surface *s = cd;
	Enesim_Format fmt;
	uint32_t *dst;
	uint32_t stride;

	fmt = enesim_surface_format_get(s);
	span = enesim_compositor_span_get(ENESIM_FILL, &fmt,
			ENESIM_FORMAT_NONE, color, ENESIM_FORMAT_NONE);

	dst = enesim_surface_data_get(s);
	stride = enesim_surface_stride_get(s);
	dst = dst + (y * stride) + x;

	while (h--)
	{
		span(dst, w, NULL, color, NULL);
		dst += stride;
	}
	/* TODO set the format again */
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void eon_engine_enesim_setup(Eon_Engine *e)
{
	enesim_init();
	FT_Init_FreeType(&library);

	e->rect_create = rect_create;
	e->rect_render = rect_render;
	e->circle_create = circle_create;
	e->circle_render = circle_render;
	e->polygon_create = polygon_create;
	e->polygon_point_add = polygon_point_add;
	e->polygon_render = polygon_render;
	e->text_create = text_create;
	e->text_render = text_render;
	e->image_create = image_create;
	e->image_delete = image_delete;
	e->image_setup = image_setup;
	e->fade_create = fade_create;
	e->fade_delete = fade_delete;
	e->fade_setup = fade_setup;
	e->hswitch_create = hswitch_create;
	e->hswitch_delete = hswitch_delete;
	e->hswitch_setup = hswitch_setup;
	e->checker_create = checker_create;
	e->checker_delete = checker_delete;
	e->checker_setup = checker_setup;
	e->debug_rect = debug_rect;
}

EAPI void eon_engine_enesim_cleanup(Eon_Engine *e)
{
	enesim_shutdown();
}
