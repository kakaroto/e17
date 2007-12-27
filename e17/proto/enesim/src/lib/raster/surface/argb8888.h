#ifndef _ARGB8888_H
#define _ARGB8888_H

#include "fixed_16p16.h"

#define ARGB_JOIN(a,r,g,b) (((a) << 24) + ((r) << 16) + ((g) << 8) + (b))
#define A_VAL(p) ((DATA8 *)(p))[3]
#define R_VAL(p) ((DATA8 *)(p))[2]
#define G_VAL(p) ((DATA8 *)(p))[1]
#define B_VAL(p) ((DATA8 *)(p))[0]

static inline DATA32 mul_256(DATA8 a, DATA32 c)
{
	return  ( (((((c) >> 8) & 0x00ff00ff) * (a)) & 0xff00ff00) +
	(((((c) & 0x00ff00ff) * (a)) >> 8) & 0x00ff00ff) );
}

static inline DATA32 interp_256(DATA8 a, DATA32 c0, DATA32 c1)
{
	return ( (((((((c0) >> 8) & 0xff00ff) - (((c1) >> 8) & 0xff00ff)) * (a))
		   + ((c1) & 0xff00ff00)) & 0xff00ff00) + 
		   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (a)) >> 8)
		   + ((c1) & 0xff00ff)) & 0xff00ff) );
}

static inline DATA32 interp_a_256(DATA8 a, DATA32 m0, DATA32 m1)
{
	return ( ((((m0) - (m1)) * (a)) >> 8) + (m1) );	
}

static inline DATA32 mul4_256(DATA8 a, DATA32 r, DATA32 g, DATA32 b, DATA32 c)
{
	return ( (((((c) >> 8) & 0xff0000) * (a)) & 0xff000000) +
			   (((((c) & 0xff0000) * (r)) >> 8) & 0xff0000) +
			   (((((c) & 0xff00) * (g)) >> 8) & 0xff00) +
			   ((((c) & 0xff) * (b)) >> 8) );
}

static inline DATA32 mul_sym(DATA32 a, DATA32 x)
{
 	return ( (((((x) >> 8) & 0x00ff00ff) * (a) + 0xff00ff) & 0xff00ff00) +
	(((((x) & 0x00ff00ff) * (a) + 0xff00ff) >> 8) & 0x00ff00ff) );
}

static inline DATA32 mul4_sym(DATA32 x, DATA32 y)
{
	return ( ((((((x) >> 16) & 0xff00) * (((y) >> 16) & 0xff00)) + 0xff0000) & 0xff000000) +
   ((((((x) >> 8) & 0xff00) * (((y) >> 16) & 0xff)) + 0xff00) & 0xff0000) +
   ((((((x) & 0xff00) * ((y) & 0xff00)) + 0xff00) >> 16) & 0xff00) +
   (((((x) & 0xff) * ((y) & 0xff)) + 0xff) >> 8) );
}


static inline void pt_pixel_blend(DATA32 *d, DATA32 s)
{
	DATA32 a;
	
	a = 256 - (s >> 24);
	*d = s + mul_256(a, *d);
}

static inline void pt_pixel_blend_mul(DATA32 *d, DATA32 s, DATA32 mul_col)
{
	s = mul4_sym(mul_col, s);
	pt_pixel_blend(d, s);
}

static inline void pt_pixel_fill(DATA32 *d, DATA32 s)
{
	*d = s;
}

/* argb8888_CPU_draw_ROP_MOD_TRNS_SCALEX_SCALEY where:
 * CPU = c
 * ROP = blend, fill, etc
 * MOD = mul, none 
 * TRNS = affine, proj, identity
 * SCALEX = no, up, down
 * SCALEY = no, up, down
 */
void argb8888_c_color(Enesim_Surface_Data *data, int off, DATA32 c, int len);
void argb8888_c_color_mask(Enesim_Surface_Data *data, int off, DATA32 c, int len, DATA8 *mask);

void argb8888_c_draw_blend_affine_no_no(Enesim_Surface *ss, Enesim_Rectangle *srect,
			Enesim_Surface *ds, Enesim_Rectangle *drect, enesim_16p16_t *t);
void argb8888_c_draw_fill_affine_no_no(Enesim_Surface *ss, Enesim_Rectangle *srect,
			Enesim_Surface *ds, Enesim_Rectangle *drect, enesim_16p16_t *t);
void argb8888_c_draw_blend_identity_no_no(Enesim_Surface *ss, Enesim_Surface *ds);
void argb8888_c_draw_blend_mul_affine_no_no(Enesim_Surface *ss, Enesim_Rectangle *srect,
			Enesim_Surface *ds, Enesim_Rectangle *drect, enesim_16p16_t *t, DATA32 mul_color);
void argb8888_c_draw_fill_identity_no_no(Enesim_Surface *ss, Enesim_Surface *ds);
#endif
