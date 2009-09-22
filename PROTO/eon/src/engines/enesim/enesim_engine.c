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
 *                              Paint Common                                  *
 *============================================================================*/
typedef struct Paint Paint;
struct Paint
{
	Enesim_Renderer *r;
	Eon_Paint *p;
	void (*style_setup)(Paint *p, Paint *rel);
};

typedef struct Paint_Drawer_Data
{
	Enesim_Rop rop;
	Eon_Color color;
} Paint_Drawer_Data;

static void paint_style_setup(Paint *p, Paint *rel, int ox, int oy)
{
	Enesim_Matrix m;

	eon_paint_style_inverse_matrix_get(p->p, rel->p, &m);
	enesim_renderer_transform_set(p->r, &m);
	enesim_renderer_origin_set(p->r, ox, oy);
}

static void paint_renderer_setup(Paint *p, int ox, int oy)
{
	Enesim_Matrix m;

	eon_paint_inverse_matrix_get(p->p, &m);
	enesim_renderer_transform_set(p->r, &m);
	enesim_renderer_origin_set(p->r, ox, oy);
}

static void paint_delete(void *ep)
{
	Paint *p = ep;

	enesim_renderer_delete(p->r);
	free(ep);
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
	int dx, dy, dw, dh;

	if (!eon_transition_paint_get((Eon_Transition *)hs, &p1, &p2, &step))
		return EINA_FALSE;
	if (!eon_paint_setup(p1, s))
		return EINA_FALSE;
	if (!eon_paint_setup(p2, s))
		return EINA_FALSE;

	eon_paint_style_coords_get(p->p, (Eon_Paint *)s, &dx, &dy, &dw, &dh);
	paint_setup(p, s, dx, dy);

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
	int dx, dy;

	if (!eon_transition_paint_get((Eon_Transition *)f, &p1, &p2, &step))
		return EINA_FALSE;
	if (!eon_paint_setup(p1, s))
		return EINA_FALSE;
	if (!eon_paint_setup(p2, s))
		return EINA_FALSE;

	eon_paint_style_coords_get(p->p, (Eon_Paint *)s, &dx, &dy, NULL, NULL);
	paint_setup(p, s, dx, dy);

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
 *                                   Checker                                  *
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

	eon_paint_style_coords_get(p->p, (Eon_Paint *)s, &dx, &dy, NULL, NULL);
	paint_setup(p, s, dx, dy);
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
 *                                   Stripes                                  *
 *============================================================================*/
static void * stripes_create(Eon_Stripes *s)
{
	Paint *p;

	p = calloc(1, sizeof(Paint));
	p->p = (Eon_Paint *)s;
	p->r = enesim_renderer_stripes_new();

	return p;
}

static Eina_Bool stripes_setup(void *data, Eon_Shape *s)
{
	Paint *p = data;
	Eon_Stripes *st = (Eon_Stripes *)p->p;
	int dx, dy;
	float th1, th2;
	Enesim_Color c1, c2;

	eon_paint_style_coords_get(p->p, (Eon_Paint *)s, &dx, &dy, NULL, NULL);
	paint_setup(p, s, dx, dy);
	th1 = eon_stripes_thickness1_get(st);
	th2 = eon_stripes_thickness2_get(st);

	c1 = eon_stripes_color1_get(st);
	c2 = eon_stripes_color2_get(st);
	enesim_renderer_stripes_color_set(p->r, c1, c2);
	enesim_renderer_stripes_thickness_set(p->r, th1, th2);

	return EINA_TRUE;
}

static void stripes_delete(void *data)
{
	Paint *p = data;

	enesim_renderer_delete(p->r);
	free(p);
}
/*============================================================================*
 *                                   Image                                    *
 *============================================================================*/
#if 0
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
	eon_paint_style_coords_get(p->p, (Eon_Paint *)s, &dx, &dy, &dw, &dh);
	paint_setup(p, s, dx, dy);
	enesim_renderer_surface_w_set(p->r, dw);
	enesim_renderer_surface_h_set(p->r, dh);
	enesim_renderer_surface_src_set(p->r, eon_image_surface_get(i));

	return EINA_TRUE;
}

static void image_style(Paint *p, Eon_Paint *p)
{

}

static void image_render(void *ei, void *cd, Eina_Rectangle *clip)
{
	Paint *p = ei;
	Eina_Rectangle geom;
	float rad;
	Eon_Coord cx, cy, cw, ch;

	paint_renderer_setup((Eon_Paint *)r->er, r->r);
	eon_square_coords_get((Eon_Square *)r->er, &cx, &cy, &cw, &ch);

	enesim_renderer_origin_set(r->r, cx.final, cy.final);
	enesim_renderer_rectangle_size_set(r->r, cw.final, ch.final);

	image_setup(r);
	enesim_renderer_state_setup(r->r);
	shape_renderer_draw((Eon_Shape *)r->er, cd, r->r, clip);
}

static void image_delete(void *i)
{
	Paint *p = i;

	enesim_renderer_delete(p->r);
	//free(p->data);
	free(p);
}
#endif
/*============================================================================*
 *                              Shape Common                                  *
 *============================================================================*/
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

	color = eon_paint_color_get(s);
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
static void shape_renderer_setup(Paint *ep)
{
	Eon_Shape *s = (Eon_Shape *)ep->p;
	Eon_Paint *p;
	Eon_Filter *f;
	Enesim_Renderer *r = ep->r;
	float stroke;
	Eon_Color color;
	Enesim_Shape_Draw_Mode mode;
	Enesim_Matrix m;

	/* the fill properties */
	p = eon_shape_fill_paint_get(s);
	if (p)
	{
		Paint *pa;

		pa = eon_paint_engine_data_get(p);
		if (pa)
		{
			enesim_renderer_shape_fill_renderer_set(r, pa->r);
			if (pa->style_setup)
				pa->style_setup(pa, ep);
		}
	}
	else
	{
		color = eon_shape_fill_color_get(s);
		enesim_renderer_shape_fill_color_set(r, color);
	}
	/* the stroke properties */
	p = eon_shape_stroke_paint_get(s);
	if (p)
	{
		Paint *pa;

		pa = eon_paint_engine_data_get(p);
		if (pa)
		{
			enesim_renderer_shape_outline_renderer_set(r, pa->r);
			if (pa->style_setup)
				pa->style_setup(pa, ep);
		}
	}
	else
	{
		color = eon_shape_stroke_color_get(s);
		enesim_renderer_shape_outline_color_set(r, color);
	}
	stroke = eon_shape_stroke_width_get(s);
	enesim_renderer_shape_outline_weight_set(r, stroke);

	/* common fill/stroke properties */
	mode = eon_shape_draw_mode_get(s);
	enesim_renderer_shape_draw_mode_set(r, mode);

#if 0
	/* the filter */
	f = eon_shape_filter_get(s);
	if (f)
	{
		/* in case of a filter render we should draw into a temporary
		 * buffer, call the filter renderer to only draw the clipping
		 * area and compose as usual
		 */
	}
#endif
}

/*============================================================================*
 *                           Rasterizer Common                                *
 *============================================================================*/
/* TODO add all the possible compositor parameters */
typedef struct Rasterizer_Drawer_Data
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
} Rasterizer_Drawer_Data;

static void aliased_color_cb(Enesim_Scanline *sl, void *data)
{
	Rasterizer_Drawer_Data *sdd = data;
	uint32_t *ddata;
	uint32_t stride;

	ddata = enesim_surface_data_get(sdd->dst);
	stride = enesim_surface_stride_get(sdd->dst);
	ddata = ddata + (sl->data.alias.y * stride) + sl->data.alias.x;
	sdd->span(ddata, sl->data.alias.w, NULL, sdd->color, NULL);
}

static void aliased_fill_cb(Enesim_Scanline *sl, void *data)
{
	Rasterizer_Drawer_Data *sdd = data;
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
	eon_paint_style_coords_get(sdd->paint->p, (Eon_Paint *)sdd->shape, &px, &py, NULL, NULL);
	/* clip the paint coordinates to the shape's ones, only pass those */
	enesim_renderer_span_fill(sdd->paint->r, sl->data.alias.x, sl->data.alias.y, sl->data.alias.w, fdata);
	//enesim_renderer_span_fill(sdd->paint->r, sl->data.alias.x - px, sl->data.alias.y - py, sl->data.alias.w, fdata);

	/* compose the filled and the destination spans */
	sdd->span(ddata, sl->data.alias.w, fdata, sdd->color, NULL);
	free(fdata);
}

static void shape_rasterizer_setup(Eon_Shape *s, Rasterizer_Drawer_Data *d, Enesim_Surface *dst)
{
	Enesim_Rop rop;
	Eon_Paint *p;
	Enesim_Format dfmt;

	dfmt = enesim_surface_format_get(dst);
	rop = eon_paint_rop_get(s);
	d->color = eon_paint_color_get(s);
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
		/* paint + color */
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
	Rasterizer_Drawer_Data sdd;

	shape_rasterizer_setup((Eon_Shape *)p->p, &sdd, cd);
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
static void rect_setup(Paint *p)
{
	Eon_Rect *er = (Eon_Rect *)p->p;
	float rad;

	shape_renderer_setup(p);
	rad = eon_rect_corner_radius_get(er);
	enesim_renderer_rectangle_corner_radius_set(p->r, rad);
	/* FIXME fix this */
	enesim_renderer_rectangle_corners_set(p->r, 1, 1, 1, 1);
}

static void rect_style(Paint *p, Paint *rel)
{
	int dx, dy, dw, dh;

	eon_paint_style_coords_get(p->p, rel->p, &dx, &dy, &dw, &dh);
	enesim_renderer_rectangle_size_set(p->r, dw, dh);
	enesim_renderer_rectangle_size_set(p->r, 320, 240);

	rect_setup(p);
	paint_style_setup(p, rel, dx, dy);
}

static void rect_render(void *er, void *cd, Eina_Rectangle *clip)
{
	Paint *p = er;
	Eina_Rectangle geom;

	Eon_Coord cx, cy, cw, ch;

	eon_square_coords_get((Eon_Square *)p->p, &cx, &cy, &cw, &ch);
	enesim_renderer_rectangle_size_set(p->r, cw.final, ch.final);

	rect_setup(p);
	paint_renderer_setup(p, cx.final, cy.final);
	enesim_renderer_state_setup(p->r);
	shape_renderer_draw((Eon_Shape *)p->p, cd, p->r, clip);
}

static void * rect_create(Eon_Rect *er)
{
	Paint *p;

	p = malloc(sizeof(Paint));
	p->p = (Eon_Paint *)er;
	p->r = enesim_renderer_rectangle_new();
	p->style_setup = rect_style;

	return p;
}
/*============================================================================*
 *                                 Circle                                     *
 *============================================================================*/
static void circle_setup(Paint *p)
{
	Eon_Circle *c = (Eon_Circle *)p->p;
	float radius;
	Eon_Coord x;
	Eon_Coord y;

	shape_renderer_setup(p);
	eon_circle_x_get(c, &x);
	eon_circle_y_get(c, &y);
	radius = eon_circle_radius_get(c);
	enesim_renderer_circle_center_set(p->r, x.final, y.final);
	enesim_renderer_circle_radius_set(p->r, radius);
}

static void circle_style(Paint *p, Paint *rel)
{
	circle_setup(p);
	paint_style_setup(p, rel, 0, 0);
}

static void circle_render(void *ec, void *cd, Eina_Rectangle *clip)
{
	Paint *p = ec;

	circle_setup(p);
	paint_renderer_setup(p, 0, 0);
	enesim_renderer_state_setup(p->r);
	shape_renderer_draw((Eon_Shape *)p->p, cd, p->r, clip);
}

static void * circle_create(Eon_Circle *ec)
{
	Paint *p;

	p = malloc(sizeof(Paint));
	p->p = (Eon_Paint *)ec;
	p->r = enesim_renderer_circle_new();
	p->style_setup = circle_style;

	return p;
}
/*============================================================================*
 *                                  Text                                      *
 *============================================================================*/
/* rambling ...
 * The text/font support can be a little tricky
 * we could create a hash for each file-face tuple with a pointer to a hash
 * that will store each glyph metrics and image based on the char.
 * the kerning will be handled on the bounding box and the rendering functions
 * but no on the calculations of the positions
 * What about text selection? we need a way to get fast the char on a position
 * + length
 */

/* Enesim doesnt have a text rasterizer, we use freetype here */
typedef struct Text
{
	Eon_Text *t;
	FT_Face face;
	// Eon_Font font;
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

/* cache the position for each char and its glyph */
static void text_setup(Text *t)
{

}

static void text_compose(Text *t, FT_Bitmap *bmp, Enesim_Surface *dst, Enesim_Compositor_Span span, int left, int top)
{
	Enesim_Color color;
	uint32_t *ddata;
	int dstride;
	int y = bmp->rows;
	uint8_t *mask = bmp->buffer;

	color = eon_paint_color_get((Eon_Shape *)t->t);
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

#if 0
/* called whenever the font has changed */
static void text_font_set(void *et, Eon_Font *f)
{

}

/* given a text retrieve the bounding box */
static void text_boundings_get(void *et, Eina_Rectangle *r)
{
	Text *t = et;
	char *str;
	int i;

	if (!t)
		return;
	str = eon_text_string_get(t->t);
	/* TODO Move this into the shape object */
	if (!str)
		return;
	for (i = 0; str[i]; i++)
	{
		FT_BBox glyph_bbox;
		FT_UInt gindex;

		/* TODO handle kerning */
		/* TODO this can be cached */
		gindex = FT_Get_Char_Index(t->face, str[i]);

		/* load glyph image into the slot without rendering */
		error = FT_Load_Glyph(t->face, glyph_index, FT_LOAD_DEFAULT );
		if (error) continue; /* ignore errors, jump to next glyph */
		/* extract glyph image and store it in our table */
		error = FT_Get_Glyph(t->face->glyph, &glyphs[num_glyphs]);
		/* get the bounding box of the glyph */
		FT_Glyph_Get_CBox(glyphs[n], ft_glyph_bbox_pixels, &glyph_bbox);
		/* move it to the real x, y */
		glyph_bbox.xMin += pos[n].x;
		glyph_bbox.xMax += pos[n].x;
		glyph_bbox.yMin += pos[n].y;
		glyph_bbox.yMax += pos[n].y;
		/* compare agains global max, min x/y and update */
	}
	/* the rect will min,max x/y */
}
#endif

static void text_render(void *et, void *cd, Eina_Rectangle *clip)
{
	FT_Error error;
	Text *t = et;
	FT_GlyphSlot slot;
	int pen_x;
	int pen_y;
	char *str;
	int i;
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
	pen_y = geom.y + 16; // we can use this to place the text on a vertical align

	color = eon_paint_color_get((Eon_Paint *)t->t);
	rop = eon_paint_rop_get((Eon_Paint *)t->t);
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
		/* LTR */
		//if (pen_x > clip->w)
		//	break;
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
	e->rect_delete = paint_delete;
	e->circle_create = circle_create;
	e->circle_render = circle_render;
	e->circle_delete = paint_delete;
#if 0
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
	e->stripes_create = stripes_create;
	e->stripes_delete = stripes_delete;
	e->stripes_setup = stripes_setup;
#endif
	e->debug_rect = debug_rect;
}

EAPI void eon_engine_enesim_cleanup(Eon_Engine *e)
{
	enesim_shutdown();
}
