#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"
#include "argb8888.h"

/*============================================================================*
 *                  argb8888_c_draw_blend_affine_no_no                        * 
 *============================================================================*/
#define RENDER_OP_CALL pt_pixel_blend(d, p0);
#define ARGB8888_CONVOLUTION_2X2_PROTO \
	argb8888_c_2x2conv_normal(DATA32 *p, DATA32 *d, enesim_16p16_t x, enesim_16p16_t y, int sw, int sh)
#define ARGB8888_CONVOLUTION_CALL \
	argb8888_c_2x2conv_normal(sp, d, sxx, syy, srect->w, srect->h);
#define ARGB8888_NOSCALE_AFFINE_ITERATOR_PROTO \
	argb8888_c_draw_blend_affine_no_no(Enesim_Surface *ss, Enesim_Rectangle *srect, \
			Enesim_Surface *ds, Enesim_Rectangle *drect, enesim_16p16_t *t)

#include "_2x2conv.h"
#include "_noscale_affine.h"

/*============================================================================*
 *              argb8888_c_draw_blend_mul_identity_no_no                      * 
 *============================================================================*/
#undef RENDER_OP_CALL
#undef ARGB8888_NOSCALE_AFFINE_ITERATOR_PROTO
#undef ARGB8888_CONVOLUTION_2X2_PROTO
#undef ARGB8888_CONVOLUTION_CALL

#define RENDER_OP_CALL pt_pixel_blend_mul(d, p0, mul_color);
#define ARGB8888_CONVOLUTION_2X2_PROTO \
	argb8888_c_2x2conv_mul(DATA32 *p, DATA32 *d, enesim_16p16_t x, enesim_16p16_t y, int sw, int sh, DATA32 mul_color)
#define ARGB8888_CONVOLUTION_CALL \
	argb8888_c_2x2conv_mul(sp, d, sxx, syy, srect->w, srect->h, mul_color);
#define ARGB8888_NOSCALE_AFFINE_ITERATOR_PROTO \
	argb8888_c_draw_blend_mul_affine_no_no(Enesim_Surface *ss, Enesim_Rectangle *srect, \
			Enesim_Surface *ds, Enesim_Rectangle *drect, enesim_16p16_t *t, DATA32 mul_color)
#include "_2x2conv.h"
#include "_noscale_affine.h"

/*============================================================================*
 *                argb8888_c_draw_blend_identity_no_no                        * 
 *============================================================================*/
#undef RENDER_OP_CALL

#define RENDER_OP_CALL pt_pixel_blend(dp, *sp);
#define ARGB8888_NOSCALE_IDENTITY_ITERATOR_PROTO \
	argb8888_c_draw_blend_identity_no_no(Enesim_Surface *ss, Enesim_Surface *ds)
#include "_noscale_identity.h"
