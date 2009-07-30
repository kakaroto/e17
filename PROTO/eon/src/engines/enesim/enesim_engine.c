#include "Eon.h"
#include "eon_private.h"

#include "Eon_Enesim.h"

#include <limits.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Eon_Engine_Enesim_Private
{

};


static void _color_set(void *c, int color)
{
	uint32_t cmul;
	uint8_t a = color >> 24;
	if (a != 256)
	{
		cmul = (color & 0xff000000) + (((((color) >> 8) & 0xff) * a) & 0xff00) +
			(((((color) & 0x00ff00ff) * a) >> 8) & 0x00ff00ff);
	}
	else
		color = cmul;
}
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
/*============================================================================*
 *                                 Horswitch                                  *
 *============================================================================*/
void * hswitch_create(Eon_Hswitch *hs)
{
	Paint *p;

	p = calloc(1, sizeof(Paint));
	p->p = (Eon_Paint *)hs;
	p->r = enesim_renderer_hswitch_new();

	return p;
}

Eina_Bool hswitch_setup(void *data, Eon_Shape *s)
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
	p->r = enesim_renderer_transition_new();

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
void * sqpattern_create(Eon_Checker *sq)
{
	Paint *p;

	p = calloc(1, sizeof(Paint));
	p->p = (Eon_Paint *)sq;
	p->r = enesim_renderer_sqpattern_new();

	return p;
}

Eina_Bool sqpattern_setup(void *data, Eon_Shape *s)
{
	Paint *p = data;
	Eon_Checker *sq = (Eon_Checker *)p->p;
	int dx, dy;

	paint_coords_get(p->p, s, &dx, &dy, NULL, NULL);

	enesim_renderer_origin_set(p->r, dx, dy);
	enesim_renderer_sqpattern_color1_set(p->r, eon_checker_color1_get(sq));
	enesim_renderer_sqpattern_color2_set(p->r, eon_checker_color2_get(sq));
	enesim_renderer_sqpattern_size_set(p->r, 20, 20);
	return EINA_TRUE;
}

static void sqpattern_delete(void *data)
{
	Paint *p = data;

	enesim_renderer_delete(p->r);
	free(p);
}
/*============================================================================*
 *                                   Image                                    *
 *============================================================================*/

/* FIXME do a real pattern! */
void _image_pattern_span(void *data, void *span, int x, int y, unsigned int len)
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

void * image_create(Eon_Image *i)
{
	Paint *p;

	p = calloc(1, sizeof(Paint));
	p->p = (Eon_Paint *)i;
	p->r = enesim_renderer_surface_new();

	return p;
}

Eina_Bool image_setup(void *data, Eon_Shape *s)
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

#if 0
/* As enesim doesnt support prescaling on the fly while doing transformations
 * we need to create a scaled image before transforming
 */
static void _image_transform(Enesim_Surface *dst, Eina_Rectangle *dclip, Context *c, Enesim_Surface *src, uint32_t sw, uint32_t sh, Eina_Rectangle *sgeom, Eina_Rectangle *sclip)
{
	Enesim_Cpu **cpus;
	int numcpus;
	Enesim_Operator drawer, tx;
	uint32_t sstride, dstride;
	uint32_t *s, *d, *t;
	Enesim_Surface *realsrc = src;
	int y;

	cpus = enesim_cpu_get(&numcpus);
	enesim_drawer_span_pixel_color_op_get(cpus[0], &drawer, c->rop, ENESIM_FORMAT_ARGB8888, ENESIM_FORMAT_ARGB8888, c->color);
	enesim_transformer_1d_op_get(&tx, cpus[0], ENESIM_FORMAT_ARGB8888, ENESIM_MATRIX_AFFINE,  ENESIM_FAST, ENESIM_FORMAT_ARGB8888);

	//printf("[%f %f %f]\n", c->matrix.m.xx, c->matrix.m.xy, c->matrix.m.xz);
	//printf("[%f %f %f]\n", c->matrix.m.yx, c->matrix.m.yy, c->matrix.m.yz);
	//printf("[%f %f %f]\n", c->matrix.m.zx, c->matrix.m.zy, c->matrix.m.zz);

	/* setup a temporary surface */
	/* downscale or upscale the image */
	if (((sgeom->w != sw) || (sgeom->h != sh)))
	{
		Enesim_Surface *tmp;
		Eina_Rectangle rclip;
		Context rc;

		rc.rop = ENESIM_FILL;
		rc.color = 0xffffffff;

		tmp = enesim_surface_new(ENESIM_FORMAT_ARGB8888, sgeom->w, sgeom->h);
		eina_rectangle_coords_from(&rclip, 0, 0, sgeom->w, sgeom->h);
		_image_scale(tmp, &rclip, &rc, src, sw, sh, &rclip, &rclip);
		realsrc = tmp;
	}
	_image_setup(realsrc, &s, &sstride, dst, &d, &dstride);

	t = malloc(dclip->w * sizeof(uint32_t));
	d =  d + (dclip->y * dstride) + dclip->x;
	y = 0;

	//printf("DCLIP %d %d %d %d\n", dclip->x, dclip->y, dclip->w, dclip->h);
	//printf("SGEOM %d %d %d %d\n", sgeom->x, sgeom->y, sgeom->w, sgeom->h);
	//printf("SCLIP %d %d %d %d\n", sclip->x, sclip->y, sclip->w, sclip->h);
	//printf("%d %d %d\n", w, h, sstride);
	//printf("old at %d %d %d %d\n", srect->x, srect->y, srect->w, srect->h);
	while (y < dclip->h)
	{
		memset(t, 0, sizeof(uint32_t) * dclip->w);
#if !DEBUG
		enesim_operator_transformer_1d(&tx, s,
				sstride, sgeom->w, sgeom->h,
				0, 0,
				c->matrix.m.xx, c->matrix.m.xy, c->matrix.m.xz,
				c->matrix.m.yx, c->matrix.m.yy, c->matrix.m.yz,
				c->matrix.m.zx, c->matrix.m.zy, c->matrix.m.zz,
				0, y, dclip->w, t);
		enesim_operator_drawer_span(&drawer, d, dclip->w, t, c->color, NULL);
		d += dstride;
#else
		enesim_operator_drawer_span(&drawer, d, dclip->w, s, 0, NULL);
		s += sstride;
		d += dstride;
#endif
		y++;
	}
	/* clean up */
	free(t);
	/* the temporary surface */
	if (realsrc != src)
	{
		enesim_surface_delete(realsrc);
	}
}

/*
 * surface: destintation surface
 * context: destination clipping?
 * srect: source rectangle (scaling??)
 * src: src image
 */
void eon_enesim_image(void *surface, void *context, Enesim_Surface *src, Eina_Rectangle *srect)
{
	Enesim_Surface *dst = surface;
	Context *c = context;
	Eina_Rectangle dclip, sclip;
	uint32_t sw, sh;
	uint32_t dw, dh;

#ifdef EON_DEBUG
	printf("[Eon_Eon_Engine_Enesim] RENDERING AN IMAGE %d %d %d %d\n", srect->x, srect->y, srect->w, srect->h);
#endif
	enesim_surface_size_get(src, &sw, &sh);
	enesim_surface_size_get(dst, &dw, &dh);
	eina_rectangle_coords_from(&dclip, srect->x, srect->y, srect->w, srect->h);

	/* the context has the clipping rect relative to the canvas */
	//printf("ENTERING\n");
	//printf("S = %d %d D = %d %d\n", sw, sh, dw, dh);
	//printf("SGEOM %d %d %d %d\n", srect->x, srect->y, srect->w, srect->h);
	//printf("DCLIP %d %d %d %d\n", dclip.x, dclip.y, dclip.w, dclip.h);
	if (c->clip.used)
	{
		//printf("CLIP %d %d %d %d\n", c->clip.rect.x, c->clip.rect.y, c->clip.rect.w, c->clip.rect.h);
		eina_rectangle_rescale_in(srect, &c->clip.rect, &sclip);
		eina_rectangle_coords_from(&dclip, c->clip.rect.x, c->clip.rect.y, c->clip.rect.w, c->clip.rect.h);
	}
	//printf("SCLIP %d %d %d %d\n", sclip.x, sclip.y, sclip.w, sclip.h);
	//printf("DCLIP %d %d %d %d\n", dclip.x, dclip.y, dclip.w, dclip.h);

	/* transformed matrix */
	if (c->matrix.used)
	{
		//printf("Transforming image  %d %d - %d %d\n", sw, sh, srect->w, srect->h);
		_image_transform(dst, &dclip, c, src, sw, sh, srect, &sclip);
	}
	// TODO filter
	/* downscale or upscale the image */
	else if (((srect->w != sw) || (srect->h != sh)))
	{
		//printf("Scaling image  %d %d - %d %d\n", sw, sh, srect->w, srect->h);
		_image_scale(dst, &dclip, c, src, sw, sh, srect, &sclip);
		return;
	}
	/* no scale */
	else
	{
		//printf("No scaling image  %d %d - %d %d\n", sw, sh, srect->w, srect->h);
		_image_noscale(dst, &dclip, c, src, srect, &sclip);
	}
}
#endif

/*============================================================================*
 *                                  Common                                    *
 *============================================================================*/
/* TODO add all the possible compositor parameters */
typedef struct Shape_Drawer_Data
{
	Enesim_Surface *dst;
	Enesim_Operator op;
	Eon_Color color;
	/* the sape information */
	Eon_Shape *shape;
	/* the callback */
	Enesim_Scanline_Callback cb;
	/* paint engine data */
	Paint *paint;
} Shape_Drawer_Data;


void aliased_color_cb(Enesim_Scanline *sl, void *data)
{
	Shape_Drawer_Data *sdd = data;
	uint32_t *ddata;
	uint32_t stride;

	ddata = enesim_surface_data_get(sdd->dst);
	stride = enesim_surface_stride_get(sdd->dst);
	ddata = ddata + (sl->data.alias.y * stride) + sl->data.alias.x;
	enesim_operator_drawer_span(&sdd->op, ddata, sl->data.alias.w, NULL, sdd->color, NULL);
}

void aliased_fill_cb(Enesim_Scanline *sl, void *data)
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
	enesim_operator_drawer_span(&sdd->op, ddata, sl->data.alias.w, fdata, sdd->color, NULL);
	free(fdata);
}

static void shape_setup(Eon_Shape *s, Shape_Drawer_Data *d, Enesim_Surface *dst)
{
	Enesim_Cpu **cpus;
	int numcpus;
	Enesim_Rop rop;
	Eon_Paint *p;

	cpus = enesim_cpu_get(&numcpus);
	rop = eon_shape_rop_get(s);
	d->color = eon_shape_color_get(s);
	d->dst = dst;
	d->shape = s;
	p = eon_shape_fill_get(s);
	/* color based */
	if (!p)
	{
		enesim_drawer_span_color_op_get(cpus[0], &d->op, rop, ENESIM_FORMAT_ARGB8888, d->color);
		d->cb = aliased_color_cb;
	}
	/* paint based */
	else
	{
		/* TODO paint + color */
		d->paint = eon_paint_engine_data_get(p);
		enesim_drawer_span_pixel_color_op_get(cpus[0], &d->op, rop, ENESIM_FORMAT_ARGB8888, ENESIM_FORMAT_ARGB8888, d->color);
		d->cb = aliased_fill_cb;
	}
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

void polygon_point_add(void *pd, int x, int y)
{
	Polygon *p = pd;

	/* create a rasterizer */
	/* TODO check the quality to create a kiia or a cpsc rasterizer */
	/* add a vertex to it */
	enesim_rasterizer_vertex_add(p->r, x, y);
}

void polygon_render(void *pd, void *cd, Eina_Rectangle *clip)
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
static void * rect_create(Eon_Rect *r)
{
	return r;
}

static void rect_render(void *er, void *cd, Eina_Rectangle *clip)
{

	Enesim_Scanline sl;
	Eon_Rect *r = er;
	Shape_Drawer_Data sdd;
	uint32_t *dst;
	uint32_t stride;

	shape_setup((Eon_Shape *)r, &sdd, cd);
	stride = enesim_surface_stride_get(sdd.dst);
	sl.type = ENESIM_SCANLINE_ALIAS;
	sl.data.alias.x = clip->x;
	sl.data.alias.y = clip->y;
	sl.data.alias.w = clip->w;

	/* we are using the same functions as the other rasterizers
	 * but the surface pointer calculations has to be done on every callback
	 */
	while (clip->h--)
	{
		sdd.cb(&sl, &sdd);
		sl.data.alias.y++;
	}
}
/*============================================================================*
 *                                 Circle                                     *
 *============================================================================*/
static void shape_renderer_draw(Eon_Shape *s, Enesim_Surface *dst,  Enesim_Renderer *r, Eina_Rectangle *clip)
{
	Enesim_Cpu **cpus;
	int numcpus;
	Enesim_Rop rop;
	Enesim_Operator op;
	Eon_Color color;
	int dh, dy;
	uint32_t *ddata;
	uint32_t *fdata;
	int stride;

	ddata = enesim_surface_data_get(dst);
	stride = enesim_surface_stride_get(dst);
	ddata = ddata + (clip->y * stride) + clip->x;

	cpus = enesim_cpu_get(&numcpus);
	rop = eon_shape_rop_get(s);
	color = eon_shape_color_get(s);
	enesim_drawer_span_pixel_color_op_get(cpus[0], &op, rop, ENESIM_FORMAT_ARGB8888, ENESIM_FORMAT_ARGB8888, color);

	dh = clip->h;
	dy = clip->y;
	/* fill the new span */
	while (dh--)
	{
		fdata = calloc(clip->w, sizeof(uint32_t));
		enesim_renderer_span_fill(r, clip->x, dy, clip->w, fdata);
		/* compose the filled and the destination spans */
		enesim_operator_drawer_span(&op, ddata, clip->w, fdata, color, NULL);
		dy++;
		ddata += stride;
		free(fdata);
	}
}

typedef struct Circle
{
	Eon_Circle *c;
#if RASTERIZER
	Enesim_Rasterizer *r;
#else
	Enesim_Renderer *r;
#endif
} Circle;

static void * circle_create(Eon_Circle *ec)
{
	Circle *c;

	c = malloc(sizeof(Circle));
	c->c = ec;
#if RASTERIZER
	c->r = enesim_rasterizer_circle_new();
	enesim_rasterizer_circle_fill_policy_set(c->r, ENESIM_RASTERIZER_FILL_POLICY_FILL);
#else
	c->r = enesim_renderer_circle_new();
	//c->r = enesim_renderer_rectangle_new();
#endif
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
	int radius;
	Shape_Drawer_Data sdd;
	int dh, dy;
	Eon_Paint *p;
	Paint *pa;

	eon_circle_x_get(c->c, &x);
	eon_circle_y_get(c->c, &y);
	radius = eon_circle_radius_get(c->c);
#if RASTERIZER
	enesim_rasterizer_circle_radius_set(c->r, radius);
        enesim_rasterizer_vertex_add(c->r, x.final, y.final);
	shape_setup((Eon_Shape *)c->c, &sdd, cd);
	enesim_rasterizer_generate(c->r, clip, sdd.cb, &sdd);
#else

	enesim_renderer_circle_center_set(c->r, x.final, y.final);
	enesim_renderer_circle_radius_set(c->r, radius);
	//enesim_renderer_rectangle_size_set(c->r, clip->w, clip->h);
	p = eon_shape_fill_get((Eon_Shape *)c->c);
	if (!p)
	{
		printf("The circle doesnt have a fill paint\n");
		return;
	}
	pa = eon_paint_engine_data_get(p);
	enesim_renderer_circle_fill_renderer_set(c->r, pa->r);
	//enesim_renderer_rectangle_fill_paint_set(c->r, pa->r);
#if 0
	{
		Enesim_Renderer *patt;

		patt = enesim_renderer_sqpattern_new();
		enesim_renderer_sqpattern_color1_set(patt, 0xff555555);
		enesim_renderer_sqpattern_color2_set(patt, 0xaa888888);
		enesim_renderer_sqpattern_size_set(patt, 10, 10);
		enesim_renderer_circle_fill_renderer_set(c->r, patt);
	}
#endif
	//enesim_renderer_rectangle_draw_mode_set(c->r, ENESIM_RENDERER_DRAW_MODE_FILL);
	enesim_renderer_circle_draw_mode_set(c->r, ENESIM_RENDERER_DRAW_MODE_FILL);
	//enesim_renderer_circle_fill_color_set(c->r, 0xaa555555);
	enesim_renderer_circle_outline_weight_set(c->r, 2.0);
	enesim_renderer_state_setup(c->r);
	shape_renderer_draw((Eon_Shape *)c->c, cd, c->r, clip);
#endif
}

static void circle_delete(void *ec)
{
	Circle *c = ec;

	enesim_rasterizer_delete(c->r);
	free(ec);
}
/*============================================================================*
 *                                  Debug                                     *
 *============================================================================*/
static void debug_rect(void *cd, uint32_t color, int x, int y, int w, int h)
{
	Enesim_Cpu **cpus;
	int numcpus;
	Enesim_Surface *s = cd;
	uint32_t *dst;
	uint32_t stride;
	Enesim_Operator op;

	cpus = enesim_cpu_get(&numcpus);
	enesim_drawer_span_color_op_get(cpus[0], &op, ENESIM_FILL, ENESIM_FORMAT_ARGB8888, color);

	dst = enesim_surface_data_get(s);
	stride = enesim_surface_stride_get(s);
	dst = dst + (y * stride) + x;

	while (h--)
	{
		enesim_operator_drawer_span(&op, dst, w, NULL, color, NULL);
		dst += stride;
	}
}

static void _ctor(void *instance)
{
	Eon_Engine_Enesim *e;
	Eon_Engine_Enesim_Private *prv;

	e = (Eon_Engine_Enesim *) instance;
	e->prv = prv = ekeko_type_instance_private_get(eon_engine_enesim_type_get(), instance);
	e->parent.rect_create = rect_create;
	e->parent.rect_render = rect_render;
	e->parent.circle_create = circle_create;
	e->parent.circle_render = circle_render;
	e->parent.polygon_create = polygon_create;
	e->parent.polygon_point_add = polygon_point_add;
	e->parent.polygon_render = polygon_render;
	e->parent.image_create = image_create;
	e->parent.image_delete = image_delete;
	e->parent.image_setup = image_setup;
	e->parent.fade_create = fade_create;
	e->parent.fade_delete = fade_delete;
	e->parent.fade_setup = fade_setup;
	e->parent.hswitch_create = hswitch_create;
	e->parent.hswitch_delete = hswitch_delete;
	e->parent.hswitch_setup = hswitch_setup;
	e->parent.sqpattern_create = sqpattern_create;
	e->parent.sqpattern_delete = sqpattern_delete;
	e->parent.sqpattern_setup = sqpattern_setup;
	e->parent.debug_rect = debug_rect;
}

static void _dtor(void *instance)
{

}


/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ekeko_Type * eon_engine_enesim_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_ENGINE_ENESIM, sizeof(Eon_Engine_Enesim),
				sizeof(Eon_Engine_Enesim_Private), eon_engine_type_get(),
				_ctor, _dtor, NULL);
	}

	return type;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
