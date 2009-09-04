#include "Enesim.h"
#include "enesim_private.h"

#include "argb8888_blend.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_compositor_argb8888_init(void)
{
	/* color */
	enesim_compositor_span_color_register(SP_C(argb8888, fill), ENESIM_FILL,
			ENESIM_FORMAT_ARGB8888);
	enesim_compositor_span_color_register(SP_C(argb8888, blend), ENESIM_BLEND,
			ENESIM_FORMAT_ARGB8888);
	/* pixel */
	enesim_compositor_span_pixel_register(SP_P(argb8888, argb8888, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	enesim_compositor_span_pixel_register(SP_P(argb8888, argb8888, blend),
			ENESIM_BLEND, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	/* mask color */
	enesim_compositor_span_mask_color_register(SP_MC(argb8888, argb8888, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	enesim_compositor_span_mask_color_register(SP_MC(argb8888, a8, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_A8);
	enesim_compositor_span_mask_color_register(SP_MC(argb8888, argb8888, blend),
			ENESIM_BLEND, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	enesim_compositor_span_mask_color_register(SP_MC(argb8888, a8, blend),
			ENESIM_BLEND, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_A8);
	/* pixel mask */
	enesim_compositor_span_pixel_mask_register(SP_PM(argb8888, argb8888, argb8888, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888, ENESIM_FORMAT_ARGB8888);
	enesim_compositor_span_pixel_mask_register(SP_PM(argb8888, argb8888, argb8888, blend),
			ENESIM_BLEND, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888, ENESIM_FORMAT_ARGB8888);
	/* pixel color */
	enesim_compositor_span_pixel_color_register(SP_PC(argb8888, argb8888, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	enesim_compositor_span_pixel_color_register(SP_PC(argb8888, argb8888, blend),
			ENESIM_BLEND, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
}

void enesim_drawer_argb8888_shutdown(void)
{
}

