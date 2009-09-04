#include "enesim_bench_common.h"
/******************************************************************************
 *                         Drawer benchmark functions                         *
 ******************************************************************************/
static void point_color_draw(Enesim_Compositor_Point point, Enesim_Surface *dst, uint32_t color)
{
	int t;
	uint32_t *dtmp;

	dtmp = enesim_surface_data_get(dst);
	for (t = 0; t < opt_times; t++)
	{
		point(dtmp, 0, color, 0);
	}
}

static void span_color_draw(Enesim_Compositor_Span span, Enesim_Surface *dst, unsigned int len,
		uint32_t color)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		uint32_t *dtmp;

		dtmp = enesim_surface_data_get(dst);
		for (i = 0; i < opt_height; i++)
		{
			span(dtmp, len, NULL, color, NULL);
			dtmp += len;
		}
	}
}

void span_pixel_draw(Enesim_Compositor_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface *src)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		uint32_t *dtmp;
		uint32_t *stmp;

		dtmp = enesim_surface_data_get(dst);
		stmp = enesim_surface_data_get(src);
		for (i = 0; i < opt_height; i++)
		{
			span(dtmp, len, stmp, 0, NULL);
			dtmp += len;
			stmp += len;
		}
	}
}


void span_pixel_mask_draw(Enesim_Compositor_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface *src, Enesim_Surface *msk)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		uint32_t *dtmp;
		uint32_t *stmp;
		uint32_t *mtmp;

		dtmp = enesim_surface_data_get(dst);
		stmp = enesim_surface_data_get(src);
		mtmp = enesim_surface_data_get(msk);
		for (i = 0; i < opt_height; i++)
		{
			span(dtmp, len, stmp, 0, mtmp);
			dtmp += len;
			stmp += len;
			mtmp += len;
		}
	}
}
void span_mask_color_draw(Enesim_Compositor_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface *m, uint32_t color)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		uint32_t *dtmp;
		uint32_t *mtmp;

		dtmp = enesim_surface_data_get(dst);
		mtmp = enesim_surface_data_get(m);
		for (i = 0; i < opt_height; i++)
		{
			span(dtmp, len, NULL, color, mtmp);
			dtmp += len;
			mtmp += len;
		}
	}
}

void span_pixel_color_draw(Enesim_Compositor_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface *s, uint32_t color)
{
	int i;
	int t;

	for (t = 0; t < opt_times; t++)
	{
		uint32_t *dtmp;
		uint32_t *stmp;

		dtmp = enesim_surface_data_get(dst);
		stmp = enesim_surface_data_get(s);
		for (i = 0; i < opt_height; i++)
		{
			span(dtmp, len, stmp, color, NULL);
			dtmp += len;
			stmp += len;
		}
	}
}

static void drawer_point_color_transparent_bench(Enesim_Rop rop, Enesim_Format dsf)
{
	double start, end;
	Enesim_Surface *dst;
	uint32_t color;
	Enesim_Compositor_Point point;

	dst = enesim_surface_new(dsf, 1, 1);
	enesim_surface_pixel_components_from(&color, dsf, 0x11, 0x22, 0x33, 0x44);
	if (point = enesim_compositor_point_get(rop, &dsf, ENESIM_FORMAT_NONE, color, ENESIM_FORMAT_NONE))
	{
		start = get_time();
		point_color_draw(point, dst, color);
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

static void drawer_span_color_bench(Enesim_Rop rop, Enesim_Surface *dst, uint32_t color)
{
	double start, end;
	Enesim_Format dfmt;
	Enesim_Compositor_Span span;

	dfmt = enesim_surface_format_get(dst);
	span = enesim_compositor_span_get(rop, &dfmt, ENESIM_FORMAT_NONE, color, ENESIM_FORMAT_NONE);
	if (span)
	{
		start = get_time();
		span_color_draw(span, dst, opt_width, color);
		end = get_time();
		printf("        %s [%3.3f sec]\n", opacity_get(color, dfmt), end - start);
	}
	else
	{
		printf("        %s [NOT BUILT]\n", opacity_get(color, dfmt));
		return;
	}
	test_finish("span_color", rop, dst, NULL, &color, NULL);
}

/* drawer span pixel bench */
static void drawer_span_pixel_bench(Enesim_Rop rop, Enesim_Surface *dst,
		Enesim_Surface *src)
{
	double start, end;
	Enesim_Format dfmt;
	Enesim_Format sfmt;
	Enesim_Compositor_Span span;

	dfmt = enesim_surface_format_get(dst);
	sfmt = enesim_surface_format_get(src);
	span = enesim_compositor_span_get(rop, &dfmt, sfmt, ENESIM_COLOR_FULL, ENESIM_FORMAT_NONE);
	if (span)
	{
		start = get_time();
		span_pixel_draw(span, dst, opt_width, src);
		end = get_time();
		printf("        %s [%3.3f sec]\n", enesim_format_name_get(sfmt), end - start);
	}
	else
	{
		printf("        %s [NOT BUILT]\n", enesim_format_name_get(sfmt));
		return;
	}
	test_finish("span_pixel", rop, dst, src, NULL, NULL);
}

static void drawer_span_mask_color_bench(Enesim_Rop rop, Enesim_Surface *dst,
		Enesim_Surface *msk, uint32_t color)
{
	double start, end;
	Enesim_Format dfmt, mfmt;
	Enesim_Compositor_Span span;

	dfmt = enesim_surface_format_get(dst);
	mfmt = enesim_surface_format_get(msk);
	if (span = enesim_compositor_span_get(rop, &dfmt, ENESIM_FORMAT_NONE, color, mfmt))
	{
		start = get_time();
		span_mask_color_draw(span, dst, opt_width, msk, color);
		end = get_time();
		printf("        %s %s [%3.3f sec]\n", enesim_format_name_get(mfmt), opacity_get(color, dfmt), end - start);
	}
	else
	{
		printf("        %s %s [NOT BUILT]\n", enesim_format_name_get(mfmt), opacity_get(color, dfmt));
		return;
	}
	test_finish("span_mask_color", rop, dst, NULL, &color, msk);
}

static void drawer_span_pixel_color_bench(Enesim_Rop rop, Enesim_Surface *dst,
		Enesim_Surface *src, uint32_t color)
{
	double start, end;
	Enesim_Format dfmt, sfmt;
	Enesim_Compositor_Span span;

	dfmt = enesim_surface_format_get(dst);
	sfmt = enesim_surface_format_get(src);
	if (span = enesim_compositor_span_get(rop, &dfmt, sfmt, color, ENESIM_FORMAT_NONE))
	{
		start = get_time();
		span_pixel_color_draw(span, dst, opt_width, src, color);
		end = get_time();
		printf("        %s %s [%3.3f sec]\n", enesim_format_name_get(sfmt), opacity_get(color, dfmt), end - start);
	}
	else
	{
		printf("        %s %s [NOT BUILT]\n", enesim_format_name_get(sfmt), opacity_get(color, dfmt));
		return;
	}
	test_finish("span_pixel_color", rop, dst, NULL, &color, src);
}

static void drawer_span_pixel_mask_bench(Enesim_Rop rop,
		Enesim_Surface *dst, Enesim_Surface *src, Enesim_Surface *msk)
{
	double start, end;
	Enesim_Format dfmt;
	Enesim_Format sfmt;
	Enesim_Format mfmt;
	Enesim_Compositor_Span span;

	dfmt = enesim_surface_format_get(dst);
	sfmt = enesim_surface_format_get(src);
	mfmt = enesim_surface_format_get(msk);
	span = enesim_compositor_span_get(rop, &dfmt, sfmt, ENESIM_COLOR_FULL, mfmt);
	if (span)
	{
		start = get_time();
		span_pixel_mask_draw(span, dst, opt_width, src, msk);
		end = get_time();
		printf("        %s %s [%3.3f sec]\n", enesim_format_name_get(sfmt), enesim_format_name_get(mfmt), end - start);
	}
	else
	{
		printf("        %s %s [NOT BUILT]\n", enesim_format_name_get(sfmt), enesim_format_name_get(mfmt));
		return;
	}
	test_finish("span_pixel_mask", rop, dst, src, NULL, msk);
}

void compositor_bench(void)
{
	Enesim_Format ssf, msf;
	Enesim_Surface *src = NULL, *dst = NULL, *msk = NULL;
	uint32_t opaque, transparent;

	printf("********************\n");
	printf("* Compositor Bench *\n");
	printf("********************\n");

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
	drawer_span_color_bench(opt_rop, dst, opaque);
	surfaces_create(NULL, 0, &dst, opt_fmt, NULL, 0);
	drawer_span_color_bench(opt_rop, dst, transparent);

	printf("    Span mask color\n");
	for (msf = 1; msf < ENESIM_FORMATS; msf++)
	{
		surfaces_create(NULL, 0, &dst, opt_fmt, &msk, msf);
		drawer_span_mask_color_bench(opt_rop, dst, msk, opaque);
		drawer_span_mask_color_bench(opt_rop, dst, msk, transparent);
	}

	printf("    Span pixel\n");
	for (ssf = 1; ssf < ENESIM_FORMATS; ssf++)
	{
		surfaces_create(&src, ssf, &dst, opt_fmt, NULL, 0);
		drawer_span_pixel_bench(opt_rop, dst, src);
	}
	printf("    Span pixel color\n");
	for (ssf = 1; ssf < ENESIM_FORMATS; ssf++)
	{
		surfaces_create(&src, ssf, &dst, opt_fmt, NULL, 0);
		drawer_span_pixel_color_bench(opt_rop, dst, src, opaque);
	}
	printf("    Span pixel mask\n");
	for (ssf = 1; ssf < ENESIM_FORMATS; ssf++)
	{
		for (msf = 1; msf < ENESIM_FORMATS; msf++)
		{
			surfaces_create(&src, ssf, &dst, opt_fmt, &msk, msf);
			drawer_span_pixel_mask_bench(opt_rop, dst, src, msk);
		}
	}
}
