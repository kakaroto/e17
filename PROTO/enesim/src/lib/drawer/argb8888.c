#include "Enesim.h"
#include "enesim_private.h"

#include "_argb8888_c.c"
#include "_argb8888_mmx.c"
#include "_argb8888_sse2.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool enesim_drawer_argb8888_init(void)
{
	Eina_Iterator *it;
	Enesim_Format *df;
	Enesim_Format *sf;

	df = enesim_format_argb8888_get();
	if (!df)
		return EINA_FALSE;

	/* native format first */
	/* color */
	enesim_drawer_point_register(ENESIM_FILL, argb8888_pt_color_fill, df, NULL, EINA_TRUE, NULL);
	enesim_drawer_span_register(ENESIM_FILL, argb8888_sp_color_blend, df, NULL, EINA_TRUE, NULL);
	enesim_drawer_point_register(ENESIM_BLEND, argb8888_pt_color_blend, df, NULL, EINA_TRUE, NULL);
	enesim_drawer_span_register(ENESIM_BLEND, argb8888_sp_color_blend, df, NULL, EINA_TRUE, NULL);
	/* mask color */
	enesim_drawer_point_register(ENESIM_FILL, argb8888_pt_mask_color_fill_argb8888, df, NULL, EINA_TRUE, df);
	enesim_drawer_span_register(ENESIM_FILL, argb8888_sp_mask_color_fill_argb8888, df, NULL, EINA_TRUE, df);
	enesim_drawer_point_register(ENESIM_BLEND, argb8888_pt_mask_color_blend_argb8888, df, NULL, EINA_TRUE, df);
	enesim_drawer_span_register(ENESIM_BLEND, argb8888_sp_mask_color_blend_argb8888, df, NULL, EINA_TRUE, df);
	/* pixel */
	enesim_drawer_point_register(ENESIM_FILL, argb8888_pt_pixel_fill_argb8888, df, df, EINA_FALSE, NULL);
	enesim_drawer_span_register(ENESIM_FILL, argb8888_sp_pixel_fill_argb8888, df, df, EINA_FALSE, NULL);
	enesim_drawer_point_register(ENESIM_BLEND, argb8888_pt_pixel_blend_argb8888, df, df, EINA_FALSE, NULL);
	enesim_drawer_span_register(ENESIM_BLEND, argb8888_sp_pixel_blend_argb8888, df, df, EINA_FALSE, NULL);
	/* pixel color */
	enesim_drawer_point_register(ENESIM_FILL, argb8888_pt_pixel_color_fill_argb8888, df, df, EINA_TRUE, NULL);
	/* pixel mask */
	enesim_drawer_span_register(ENESIM_BLEND, argb8888_sp_pixel_mask_blend_argb8888_argb8888_c, df, df, EINA_FALSE, df);
	enesim_drawer_span_register(ENESIM_FILL, argb8888_sp_pixel_mask_fill_argb8888_argb8888_c, df, df, EINA_FALSE, df);

	/* TODO
	enesim_drawer_span_register(ENESIM_FILL, argb8888_sp_pixel_color_fill_argb8888, df, df, EINA_TRUE, NULL);
	enesim_drawer_point_register(ENESIM_BLEND, argb8888_pt_pixel_color_blend_argb8888, df, df, EINA_TRUE, NULL);
	enesim_drawer_span_register(ENESIM_BLEND, argb8888_sp_pixel_color_blend_argb8888, df, df, EINA_TRUE, NULL);
	*/
	/* other formats */
	/* argb8888_unpre */
	sf = enesim_format_argb8888_unpre_get();
	if (!sf)
		goto end;
	/* pixel */
	enesim_drawer_point_register(ENESIM_FILL, argb8888_pt_pixel_fill_argb8888_unpre, df, sf, EINA_FALSE, NULL);
	enesim_drawer_span_register(ENESIM_FILL, argb8888_sp_pixel_fill_argb8888_unpre, df, sf, EINA_FALSE, NULL);
	enesim_drawer_point_register(ENESIM_BLEND, argb8888_pt_pixel_blend_argb8888_unpre, df, sf, EINA_FALSE, NULL);
	enesim_drawer_span_register(ENESIM_BLEND, argb8888_sp_pixel_blend_argb8888_unpre, df, sf, EINA_FALSE, NULL);
end:
	return EINA_TRUE;
}

void enesim_drawer_argb8888_shutdown(void)
{
}

