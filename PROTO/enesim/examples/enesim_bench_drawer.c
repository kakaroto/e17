#include "enesim_bench_common.h"
/******************************************************************************
 *                         Drawer benchmark functions                         *
 ******************************************************************************/
static void surfaces_create(Enesim_Surface **src, Enesim_Surface_Format sfmt,
		Enesim_Surface **dst, Enesim_Surface_Format dfmt,
		Enesim_Surface **msk, Enesim_Surface_Format mfmt)
{
	if (src)
	{
		if (*src) enesim_surface_delete(*src);
		*src = enesim_surface_new(sfmt, opt_width, opt_height);
		test_gradient(*src);
	}
	if (dst)
	{
		if (*dst) enesim_surface_delete(*dst);
		*dst = enesim_surface_new(dfmt, opt_width, opt_height);
		test_gradient2(*dst);
	}
	if (msk)
	{
		if (*msk) enesim_surface_delete(*msk);
		*msk = enesim_surface_new(mfmt, opt_width, opt_height);
		test_gradient3(*msk);
	}
}



static void point_color_draw(Enesim_Drawer_Point point, Enesim_Surface *dst, Enesim_Surface_Pixel *color)
{
	int t;
	Enesim_Surface_Data dtmp;
	enesim_surface_data_get(dst, &dtmp);

	for (t = 0; t < opt_times; t++)
	{
		point(&dtmp, NULL, color, NULL);
	}
}

static void span_color_draw(Enesim_Drawer_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface_Pixel *color)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		Enesim_Surface_Data dtmp;

		enesim_surface_data_get(dst, &dtmp);
		for (i = 0; i < opt_height; i++)
		{
			span(&dtmp, len, NULL, color, NULL);
			enesim_surface_data_increment(&dtmp, len);
		}
	}
}

void span_pixel_draw(Enesim_Drawer_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface *src)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		Enesim_Surface_Data dtmp;
		Enesim_Surface_Data stmp;

		enesim_surface_data_get(dst, &dtmp);
		enesim_surface_data_get(src, &stmp);
		for (i = 0; i < opt_height; i++)
		{
			span(&dtmp, len, &stmp, 0, NULL);
			enesim_surface_data_increment(&dtmp, len);
			enesim_surface_data_increment(&stmp, len);
		}
	}
}


void span_pixel_mask_draw(Enesim_Drawer_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface *src, Enesim_Surface *msk)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		Enesim_Surface_Data dtmp;
		Enesim_Surface_Data stmp;
		Enesim_Surface_Data mtmp;

		enesim_surface_data_get(dst, &dtmp);
		enesim_surface_data_get(src, &stmp);
		enesim_surface_data_get(msk, &mtmp);
		for (i = 0; i < opt_height; i++)
		{
			span(&dtmp, len, &stmp, 0, &mtmp);
			enesim_surface_data_increment(&dtmp, len);
			enesim_surface_data_increment(&stmp, len);
			enesim_surface_data_increment(&mtmp, len);
		}
	}
}
void span_mask_color_draw(Enesim_Drawer_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface *m, Enesim_Surface_Pixel *color)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		Enesim_Surface_Data dtmp;
		Enesim_Surface_Data mtmp;

		enesim_surface_data_get(dst, &dtmp);
		enesim_surface_data_get(m, &mtmp);
		for (i = 0; i < opt_height; i++)
		{
			span(&dtmp, len, NULL, color, &mtmp);
			enesim_surface_data_increment(&dtmp, len);
			enesim_surface_data_increment(&mtmp, len);
		}
	}
}

static void drawer_point_color_transparent_bench(Enesim_Rop rop, Enesim_Surface_Format dsf)
{
	double start, end;
	Enesim_Surface *dst;
	Enesim_Drawer_Point dpoint;
	Enesim_Surface_Pixel color;

	dst = enesim_surface_new(dsf, 1, 1);
	enesim_surface_pixel_components_from(&color, dsf, 0x11, 0x22, 0x33, 0x44);
	dpoint = enesim_drawer_point_color_get(rop, dsf, &color);
	if (dpoint)
	{
		start = get_time();
		point_color_draw(dpoint, dst, &color);
		end = get_time();
		printf("    Point transparent color [%3.3f sec]\n", end - start);
	}
	else
	{
		printf("    Point transparent color [NOT BUILT]\n");
		return;
	}
	test_finish("point_color_transparent", rop, dst, NULL, &color, NULL);
	enesim_surface_delete(dst);
}

static void drawer_span_color_bench(Enesim_Rop rop, Enesim_Surface *dst, Enesim_Surface_Pixel *color)
{
	double start, end;
	Enesim_Surface_Format dfmt;
	Enesim_Drawer_Span dspan;

	dfmt = enesim_surface_format_get(dst);
	dspan = enesim_drawer_span_color_get(rop, dfmt, color);
	if (dspan)
	{
		start = get_time();
		span_color_draw(dspan, dst, opt_width, color);
		end = get_time();
		printf("        %s [%3.3f sec]\n", opacity_get(color), end - start);
	}
	else
	{
		printf("        %s [NOT BUILT]\n", opacity_get(color));
		return;
	}
	test_finish("span_color", rop, dst, NULL, color, NULL);
}

/* drawer span pixel bench */
static void drawer_span_pixel_bench(Enesim_Rop rop, Enesim_Surface *dst, 
		Enesim_Surface *src)
{
	double start, end;
	Enesim_Surface_Format dfmt;
	Enesim_Surface_Format sfmt;
	Enesim_Drawer_Span dspan;

	dfmt = enesim_surface_format_get(dst);
	sfmt = enesim_surface_format_get(src);
	dspan = enesim_drawer_span_pixel_get(rop, dfmt, sfmt);
	if (dspan)
	{
		start = get_time();
		span_pixel_draw(dspan, dst, opt_width, src);
		end = get_time();
		printf("        %s [%3.3f sec]\n", enesim_surface_format_name_get(sfmt), end - start);
	}
	else
	{
		printf("        %s [NOT BUILT]\n", enesim_surface_format_name_get(sfmt));
		return;
	}
	test_finish("span_pixel", rop, dst, src, NULL, NULL);
}

static void drawer_span_mask_color_bench(Enesim_Rop rop, Enesim_Surface *dst,
		Enesim_Surface *msk, Enesim_Surface_Pixel *color)
{
	double start, end;
	Enesim_Surface_Format dfmt, mfmt;
	Enesim_Drawer_Span dspan;
	
	dfmt = enesim_surface_format_get(dst);
	mfmt = enesim_surface_format_get(msk);
	dspan = enesim_drawer_span_mask_color_get(rop, dfmt, mfmt, color);
	if (dspan)
	{
		start = get_time();
		span_mask_color_draw(dspan, dst, opt_width, msk, color);
		end = get_time();
		printf("        %s %s [%3.3f sec]\n", enesim_surface_format_name_get(mfmt), opacity_get(color), end - start);
	}
	else
	{
		printf("        %s %s [NOT BUILT]\n", enesim_surface_format_name_get(mfmt), opacity_get(color));
		return;
	}
	test_finish("span_mask_color", rop, dst, NULL, color, msk);
}

static void drawer_span_pixel_mask_bench(Enesim_Rop rop,
		Enesim_Surface *dst, Enesim_Surface *src, Enesim_Surface *msk)
{
	double start, end;
	Enesim_Surface_Format dfmt;
	Enesim_Surface_Format sfmt;
	Enesim_Surface_Format mfmt;
	Enesim_Drawer_Span dspan;

	dfmt = enesim_surface_format_get(dst);
	sfmt = enesim_surface_format_get(src);
	mfmt = enesim_surface_format_get(msk);
	dspan = enesim_drawer_span_pixel_mask_get(rop, dfmt, sfmt, mfmt);
	if (dspan)
	{
		start = get_time();
		span_pixel_mask_draw(dspan, dst, opt_width, src, msk);
		end = get_time();
		printf("        %s %s [%3.3f sec]\n", enesim_surface_format_name_get(sfmt), enesim_surface_format_name_get(mfmt), end - start);
	}
	else
	{
		printf("        %s %s [NOT BUILT]\n", enesim_surface_format_name_get(sfmt), enesim_surface_format_name_get(mfmt));
		return;
	}
	test_finish("span_pixel_mask", rop, dst, src, NULL, msk);
}

void drawer_bench(void)
{
	Enesim_Surface_Format ssf, msf;
	Enesim_Surface *src = NULL, *dst = NULL, *msk = NULL;
	Enesim_Surface_Pixel opaque, transparent;

	printf("****************\n");
	printf("* Drawer Bench *\n");
	printf("****************\n");

	/* Dump the default surfaces for the different operations */
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, &msk, opt_fmt);
	surface_save(dst, "destination.png");
	surface_save(src, "source.png");
	surface_save(msk, "mask.png");
	/* Default colors for every bench */
	enesim_surface_pixel_components_from(&opaque, opt_fmt, 0xff, 0xff, 0x00, 0x00);
	enesim_surface_pixel_components_from(&transparent, opt_fmt, 0x88, 0x00, 0xff, 0x00);
	/* point functions */
	drawer_point_color_transparent_bench(opt_rop, opt_fmt);
	/* span functions */
	printf("    Span color\n");
	surfaces_create(NULL, 0, &dst, opt_fmt, NULL, 0);
	drawer_span_color_bench(opt_rop, dst, &opaque);
	surfaces_create(NULL, 0, &dst, opt_fmt, NULL, 0);
	drawer_span_color_bench(opt_rop, dst, &transparent);
	
	printf("    Span mask color\n");
	for (msf = ENESIM_SURFACE_ARGB8888; msf < ENESIM_SURFACE_FORMATS; msf++)
	{
		surfaces_create(NULL, 0, &dst, opt_fmt, &msk, msf);
		drawer_span_mask_color_bench(opt_rop, dst, msk, &opaque);
		drawer_span_mask_color_bench(opt_rop, dst, msk, &transparent);
	}
	printf("    Span pixel\n");
	for (ssf = ENESIM_SURFACE_ARGB8888; ssf < ENESIM_SURFACE_FORMATS; ssf++)
	{
		surfaces_create(&src, ssf, &dst, opt_fmt, NULL, 0);
		drawer_span_pixel_bench(opt_rop, dst, src);
	}
	printf("    Span pixel mask\n");
	for (ssf = ENESIM_SURFACE_ARGB8888; ssf < ENESIM_SURFACE_FORMATS; ssf++)
	{
		
		for (msf = ENESIM_SURFACE_ARGB8888; msf < ENESIM_SURFACE_FORMATS; msf++)
		{
			surfaces_create(&src, ssf, &dst, opt_fmt, &msk, msf);
			drawer_span_pixel_mask_bench(opt_rop, dst, src, msk);
		}
	}
}
