#include "Enesim.h"
#include "enesim_private.h"
#include "surface.h"
#include "argb8888.h"
/*============================================================================*
 *                  argb8888_c_draw_fill_affine_no_no                         * 
 *============================================================================*/
#undef RENDER_OP_CALL
#undef ARGB8888_CONVOLUTION_2X2_PROTO
#undef ARGB8888_CONVOLUTION_CALL
#undef ARGB8888_NOSCALE_AFFINE_ITERATOR_PROTO

#define RENDER_OP_CALL pt_pixel_fill(d, p0);
#define ARGB8888_CONVOLUTION_2X2_PROTO \
	argb8888_c_2x2conv_fill(DATA32 *p, DATA32 *d, enesim_16p16_t x, enesim_16p16_t y, int sw, int sh)
#define ARGB8888_CONVOLUTION_CALL \
	argb8888_c_2x2conv_fill(sp, d, sxx, syy, srect->w, srect->h);
#define ARGB8888_NOSCALE_AFFINE_ITERATOR_PROTO \
	argb8888_c_draw_fill_affine_no_no(Enesim_Surface *ss, Enesim_Rectangle *srect, \
			Enesim_Surface *ds, Enesim_Rectangle *drect, enesim_16p16_t *t)
#include "_2x2conv.h"
#include "_noscale_affine.h"
/*============================================================================*
 *                 argb8888_c_draw_fill_identity_no_no                        * 
 *============================================================================*/
#undef RENDER_OP_CALL
#undef ARGB8888_NOSCALE_IDENTITY_ITERATOR_PROTO

#define RENDER_OP_CALL pt_pixel_fill(dp, *sp);
#define ARGB8888_NOSCALE_IDENTITY_ITERATOR_PROTO \
	argb8888_c_draw_fill_identity_no_no(Enesim_Surface *ss, Enesim_Surface *ds)
#include "_noscale_identity.h"
