#include "Enesim.h"
#include "enesim_private.h"

#include "argb8888_blend.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_drawer_argb8888_init(Enesim_Cpu *cpu)
{
	printf("registering drawers\n");
	/* TODO check that the cpu is host */
	/* color */
	enesim_drawer_span_color_register(cpu,
			SP_C(argb8888, fill), ENESIM_FILL,
			ENESIM_FORMAT_ARGB8888);
	enesim_drawer_span_color_register(cpu,
			SP_C(argb8888, blend), ENESIM_BLEND,
			ENESIM_FORMAT_ARGB8888);
	/* pixel */
	enesim_drawer_span_pixel_register(cpu,
			SP_P(argb8888, argb8888, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	enesim_drawer_span_pixel_register(cpu,
			SP_P(argb8888, argb8888, blend),
			ENESIM_BLEND, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	/* mask color */
	enesim_drawer_span_mask_color_register(cpu,
			SP_MC(argb8888, argb8888, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	enesim_drawer_span_mask_color_register(cpu,
			SP_MC(argb8888, argb8888, blend),
			ENESIM_BLEND, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
	/* pixel mask */
	enesim_drawer_span_pixel_mask_register(cpu,
			SP_PM(argb8888, argb8888, argb8888, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888, ENESIM_FORMAT_ARGB8888);
	enesim_drawer_span_pixel_mask_register(cpu,
			SP_PM(argb8888, argb8888, argb8888, blend),
			ENESIM_BLEND, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888, ENESIM_FORMAT_ARGB8888);
	/* pixel color */
	enesim_drawer_span_pixel_color_register(cpu,
			SP_PC(argb8888, argb8888, fill),
			ENESIM_FILL, ENESIM_FORMAT_ARGB8888,
			ENESIM_FORMAT_ARGB8888);
}

void enesim_drawer_argb8888_shutdown(void)
{
}

