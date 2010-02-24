/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
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
#ifndef RENDERER_H_
#define RENDERER_H_

/* TODO
 * + add a surface drawer too, not only span based :)
 * + add common parameters to the renderer here like transformation matrix and quality
 */

typedef void (*Enesim_Renderer_Span_Draw)(void *r, int x, int y, unsigned int len, uint32_t *dst);
typedef void (*Enesim_Renderer_Delete)(void *r);
typedef Eina_Bool (*Enesim_Renderer_State_Setup)(void *r);
typedef void (*Enesim_Renderer_State_Cleanup)(void *r);

struct _Enesim_Renderer
{
	EINA_MAGIC;
	int type_id;
	Enesim_Renderer_Span_Draw span;
	Enesim_Renderer_Delete free;
	Enesim_Renderer_State_Setup state_setup;
	Enesim_Renderer_State_Cleanup state_cleanup;
	Eina_Bool changed;
	/* the renderer common properties */
	int ox, oy; /* the origin */
	struct {
		Enesim_F16p16_Matrix values;
		Enesim_Matrix_Type type;
	} matrix;
};

typedef struct _Enesim_Renderer_Shape
{
	Enesim_Renderer base;
	struct {
		Enesim_Color color;
		Enesim_Renderer *rend;
		float weight;
	} stroke;

	struct {
		Enesim_Color color;
		Enesim_Renderer *rend;
	} fill;
	Enesim_Shape_Draw_Mode draw_mode;
} Enesim_Renderer_Shape;

typedef struct _Enesim_Renderer_Gradient
{
	Enesim_Renderer base;
	uint32_t *src;
	int slen;
	Eina_List *stops;
} Enesim_Renderer_Gradient;

#define ENESIM_RENDERER_DELETE(f) ((Enesim_Renderer_Delete)(f))
#define ENESIM_RENDERER_SPAN_DRAW(f) ((Enesim_Renderer_Span_Draw)(f))
#define ENESIM_RENDERER_STATE_SETUP(f) ((Enesim_Renderer_State_Setup)(f))
#define ENESIM_RENDERER_STATE_CLEANUP(f) ((Enesim_Renderer_State_Cleanup)(f))

/* Helper functions needed by other renderers */
static inline void renderer_affine_setup(Enesim_Renderer *r, int x, int y,
		Eina_F16p16 *fpx, Eina_F16p16 *fpy)
{
	Eina_F16p16 xx, yy;

	x -= r->ox;
	y -= r->oy;

	xx = eina_f16p16_int_from(x);
	yy = eina_f16p16_int_from(y);

	*fpx = eina_f16p16_mul(r->matrix.values.xx, xx) +
			eina_f16p16_mul(r->matrix.values.xy, yy) +
			r->matrix.values.xz;
	*fpy = eina_f16p16_mul(r->matrix.values.yx, xx) +
			eina_f16p16_mul(r->matrix.values.yy, yy) +
			r->matrix.values.yz;
}

static inline void renderer_projective_setup(Enesim_Renderer *r, int x, int y,
		Eina_F16p16 *fpx, Eina_F16p16 *fpy, Eina_F16p16 *fpz)
{
	Eina_F16p16 xx, yy;

	x -= r->ox;
	y -= r->oy;

	xx = eina_f16p16_int_from(x);
	yy = eina_f16p16_int_from(y);

	*fpy = eina_f16p16_mul(r->matrix.values.yx, xx) +
			eina_f16p16_mul(r->matrix.values.yy, yy) +
			r->matrix.values.yz;
	*fpx = eina_f16p16_mul(r->matrix.values.xx, xx) +
			eina_f16p16_mul(r->matrix.values.xy, yy) +
			r->matrix.values.xz;
	*fpz = eina_f16p16_mul(r->matrix.values.zx, xx) +
			eina_f16p16_mul(r->matrix.values.zy, yy) +
			r->matrix.values.zz;
}

Eina_Bool enesim_renderer_state_setup(Enesim_Renderer *r);
void enesim_renderer_state_cleanup(Enesim_Renderer *r);
void enesim_renderer_span_fill(Enesim_Renderer *r, int x, int y,
		unsigned int len, uint32_t *dst);

void enesim_renderer_shape_init(Enesim_Renderer *r);
void enesim_renderer_gradient_init(Enesim_Renderer *r);
void enesim_renderer_gradient_state_setup(Enesim_Renderer *r, int len);

/* some built-in renderer type identifiers */
#define SURFACE_RENDERER (1)
//#define FIGURE_RENDERER (2)
#define IMAGE_RENDERER (3)
#define RECTANGLE_RENDERER (1 << 8)
#define CIRCLE_RENDERER (2 << 8)
#define ELLIPSE_RENDERER (3 << 8)

#endif
