#include "Enesim.h"
#include "enesim_private.h"
/*
 * TODO
 * + Use mmx if possible, that means that we need to refactor several
 * parts of the header and the _argb8888_mmx source file
 *
 * + all the above generic functions should convert the src, dst and mask
 * to argb8888 (internal format) and handle the real operation in that
 * format. This will incredible slow, but works as a generic way
 * to handle this, any surface format implementor should just
 * create the converters from/to argb8888. In case he wants an accelerated
 * drawer he must implement a specific drawer
 */
/*============================================================================*
 *                                 Blend                                      *
 *============================================================================*/
static void generic_pt_color_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	Enesim_Surface_Pixel c_argb8888, d_argb8888, tmp;
	Enesim_Format *f = enesim_format_argb8888_get();

	enesim_surface_pixel_convert(color, &c_argb8888, f);
	enesim_surface_data_pixel_get(d, &tmp);
	enesim_surface_pixel_convert(&tmp, &d_argb8888, f);
	argb8888_blend(&d_argb8888.plane0, c_argb8888.plane0 >> 24, c_argb8888.plane0);
	enesim_surface_pixel_convert(&d_argb8888, &tmp, d->format);
	enesim_surface_data_pixel_set(d, &tmp);
}
static void generic_pt_pixel_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}
static void generic_pt_pixel_color_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}
static void generic_pt_mask_color_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}

static void generic_pt_pixel_mask_blend(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}

static void generic_sp_color_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp = *d;

	while (len--)
	{
		Enesim_Surface_Pixel sp;

		generic_pt_color_blend(&dtmp, NULL, color, NULL);
		enesim_surface_data_increment(&dtmp, 1);
	}
}

static void generic_sp_mask_color_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}

static void generic_sp_pixel_color_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}

static void generic_sp_pixel_mask_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}

static void generic_sp_pixel_blend(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}
/*============================================================================*
 *                                  Fill                                      *
 *============================================================================*/
static void generic_pt_color_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	Enesim_Surface_Pixel c_argb8888, d_argb8888, tmp;

	enesim_surface_pixel_convert(color, &c_argb8888, enesim_format_argb8888_get());
	enesim_surface_data_pixel_get(d, &tmp);
	enesim_surface_pixel_convert(&tmp, &d_argb8888, enesim_format_argb8888_get());
	argb8888_fill(&d_argb8888.plane0, c_argb8888.plane0);
	enesim_surface_pixel_convert(&d_argb8888, &tmp, d->format);
	enesim_surface_data_pixel_set(d, &tmp);
}
static void generic_pt_pixel_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	Enesim_Surface_Pixel s_argb8888, d_argb8888, tmp;

	enesim_surface_pixel_convert(s, &s_argb8888, enesim_format_argb8888_get());
	enesim_surface_data_pixel_get(d, &tmp);
	enesim_surface_pixel_convert(&tmp, &d_argb8888, enesim_format_argb8888_get());
	argb8888_fill(&d_argb8888.plane0, s_argb8888.plane0);
	enesim_surface_pixel_convert(&d_argb8888, &tmp, d->format);
	enesim_surface_data_pixel_set(d, &tmp);
}
static void generic_pt_pixel_color_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{
	//argb8888_fill(d->data.argb8888.plane0, argb8888_mul4_sym(color->pixel.argb8888.plane0, s->pixel.argb8888.plane0));
}
static void generic_pt_mask_color_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}

static void generic_pt_pixel_mask_fill(Enesim_Surface_Data *d,
		Enesim_Surface_Pixel *s, Enesim_Surface_Pixel *color,
		Enesim_Surface_Pixel *m)
{

}

static void generic_sp_color_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}
static void generic_sp_pixel_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{
	Enesim_Surface_Data dtmp = *d;
	Enesim_Surface_Data stmp = *s;
	while (len--)
	{
		Enesim_Surface_Pixel sp;

		enesim_surface_data_pixel_get(&stmp, &sp);
		generic_pt_pixel_fill(&dtmp, &sp, NULL, NULL);
		enesim_surface_data_increment(&dtmp, 1);
		enesim_surface_data_increment(&stmp, 1);
	}
}

static void generic_sp_mask_color_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}

static void generic_sp_pixel_color_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}

static void generic_sp_pixel_mask_fill(Enesim_Surface_Data *d,
		unsigned int len, Enesim_Surface_Data *s,
		Enesim_Surface_Pixel *color, Enesim_Surface_Data *m)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool enesim_drawer_generic_init(void)
{
	Eina_Iterator *it;
	Enesim_Format *df;

	/* iterate over all the available formats */
	it = enesim_format_iterator_new();
	while (eina_iterator_next(it, (void **)&df))
	{
		Eina_Iterator *sit;
		Enesim_Format *sf;
		/* register every generic drawer */
		/* color */
		enesim_drawer_point_register(ENESIM_FILL, generic_pt_color_fill, df, NULL, EINA_TRUE, NULL);
		enesim_drawer_span_register(ENESIM_FILL, generic_sp_color_blend, df, NULL, EINA_TRUE, NULL);
		enesim_drawer_point_register(ENESIM_BLEND, generic_pt_color_blend, df, NULL, EINA_TRUE, NULL);
		enesim_drawer_span_register(ENESIM_BLEND, generic_sp_color_blend, df, NULL, EINA_TRUE, NULL);

		sit = enesim_format_iterator_new();
		while (eina_iterator_next(sit, (void **)&sf))
		{
			Eina_Iterator *mit;
			Enesim_Format *mf;
			/* pixel */
			enesim_drawer_point_register(ENESIM_FILL, generic_pt_pixel_fill, df, sf, EINA_FALSE, NULL);
			enesim_drawer_span_register(ENESIM_FILL, generic_sp_pixel_fill, df, sf, EINA_FALSE, NULL);
			enesim_drawer_point_register(ENESIM_BLEND, generic_pt_pixel_blend, df, sf, EINA_FALSE, NULL);
			enesim_drawer_span_register(ENESIM_BLEND, generic_sp_pixel_blend, df, sf, EINA_FALSE, NULL);
			/* mask color */
			enesim_drawer_point_register(ENESIM_FILL, generic_pt_mask_color_fill, df, NULL, EINA_TRUE, sf);
			enesim_drawer_span_register(ENESIM_FILL, generic_sp_mask_color_fill, df, NULL, EINA_TRUE, sf);
			enesim_drawer_point_register(ENESIM_BLEND, generic_pt_mask_color_blend, df, NULL, EINA_TRUE, sf);
			enesim_drawer_span_register(ENESIM_BLEND, generic_sp_mask_color_blend, df, NULL, EINA_TRUE, sf);
			/* pixel color */
			enesim_drawer_point_register(ENESIM_FILL, generic_pt_pixel_color_fill, df, sf, EINA_TRUE, NULL);
			enesim_drawer_span_register(ENESIM_FILL, generic_sp_pixel_color_fill, df, sf, EINA_TRUE, NULL);
			enesim_drawer_point_register(ENESIM_BLEND, generic_pt_pixel_color_blend, df, sf, EINA_TRUE, NULL);
			enesim_drawer_span_register(ENESIM_BLEND, generic_sp_pixel_color_blend, df, sf, EINA_TRUE, NULL);
			/* pixel mask */
			mit = enesim_format_iterator_new();
			while (eina_iterator_next(mit, (void **)&mf))
			{
				enesim_drawer_point_register(ENESIM_FILL, generic_pt_pixel_mask_fill, df, sf, EINA_FALSE, mf);
				enesim_drawer_span_register(ENESIM_FILL, generic_sp_pixel_mask_fill, df, sf, EINA_FALSE, mf);
				enesim_drawer_point_register(ENESIM_BLEND, generic_pt_pixel_mask_blend, df, sf, EINA_FALSE, mf);
				enesim_drawer_span_register(ENESIM_BLEND, generic_sp_pixel_mask_blend, df, sf, EINA_FALSE, mf);
			}
			eina_iterator_free(mit);
		}
		eina_iterator_free(sit);
	}
	eina_iterator_free(it);
}

void enesim_drawer_generic_shutdown(void)
{

}
