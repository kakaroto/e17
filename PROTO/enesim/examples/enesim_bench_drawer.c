#include "enesim_bench_common.h"
/******************************************************************************
 *                         Drawer benchmark functions                         *
 ******************************************************************************/
static void point_color_draw(Enesim_Drawer_Point point, Enesim_Surface *dst, Enesim_Color color)
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
		Enesim_Color color)
{
	int i;
	int t;
	Enesim_Surface_Format format = enesim_surface_format_get(dst);

	for (t = 0; t < opt_times; t++)
	{
		Enesim_Surface_Data dtmp;

		enesim_surface_data_get(dst, &dtmp);
		for (i = 0; i < opt_height; i++)
		{
			span(&dtmp, len, NULL, color, NULL);
			enesim_surface_data_increment(&dtmp, format, len);
		}
	}
}

void span_pixel_draw(Enesim_Drawer_Span span, Enesim_Surface *dst, unsigned int len,
		Enesim_Surface *src)
{
	int i;
	int t;
	Enesim_Surface_Format dfmt = enesim_surface_format_get(dst);
	Enesim_Surface_Format sfmt = enesim_surface_format_get(src);

	for (t = 0; t < opt_times; t++)
	{
		Enesim_Surface_Data dtmp;
		Enesim_Surface_Data stmp;

		enesim_surface_data_get(dst, &dtmp);
		enesim_surface_data_get(src, &stmp);
		for (i = 0; i < opt_height; i++)
		{
			span(&dtmp, len, &stmp, 0, NULL);
			enesim_surface_data_increment(&dtmp, dfmt, len);
			enesim_surface_data_increment(&stmp, sfmt, len);
		}
	}
}


/* drawer span solid color bench */
static void drawer_span_color_solid_bench(Enesim_Rop rop, Enesim_Surface_Format dsf)
{
	double start, end;
	Enesim_Color color;
	Enesim_Surface *dst;
	Enesim_Drawer_Span dspan;

	dst = enesim_surface_new(dsf, opt_width, opt_height);
	test_gradient2(dst);
	enesim_color_get(&color, 0xff, 0xff, 0x89, 0x89);
	dspan = enesim_drawer_span_color_get(rop, dsf, color);
	if (dspan)
	{
		start = get_time();
		span_color_draw(dspan, dst, opt_width, color);
		end = get_time();
		printf("    Span solid color [%3.3f sec]\n", end - start);
	}
	else
	{
		printf("    Span solid color [NOT BUILT]\n");
		return;
	}
	test_finish("span_color_solid", rop, dst, NULL, NULL);
	enesim_surface_delete(dst);
}
/* drawer span transparent color bench */
static void drawer_span_color_transparent_bench(Enesim_Rop rop, Enesim_Surface_Format dsf)
{
	double start, end;
	Enesim_Surface *dst;
	Enesim_Drawer_Span dspan;
	Enesim_Color color;

	dst = enesim_surface_new(dsf, opt_width, opt_height);
	test_gradient2(dst);
	enesim_color_get(&color, 0x55, 0x00, 0xff, 0x00);
	dspan = enesim_drawer_span_color_get(rop, dsf, color);
	if (dspan)
	{
		start = get_time();
		span_color_draw(dspan, dst, opt_width, color);
		end = get_time();
		printf("    Span transparent color [%3.3f sec]\n", end - start);
	}
	else
	{
		printf("    Span transparent color [NOT BUILT]\n");
		return;
	}
	test_finish("span_color_transparent", rop, dst, NULL, NULL);
	enesim_surface_delete(dst);
}
/* drawer span pixel bench */
static void drawer_span_pixel_bench(Enesim_Rop rop, Enesim_Surface_Format dsf, Enesim_Surface_Format ssf)
{
	double start, end;
	Enesim_Surface *dst, *src;
	Enesim_Drawer_Span dspan;

	dst = enesim_surface_new(dsf, opt_width, opt_height);
	test_gradient2(dst);
	src = enesim_surface_new(ssf, opt_width, opt_height);
	/* fill the source with the gradient */
	test_gradient(src);
	dspan = enesim_drawer_span_pixel_get(rop, dsf, ssf);
	if (dspan)
	{
		start = get_time();
		span_pixel_draw(dspan, dst, opt_width, src);
		end = get_time();
		printf("        %s [%3.3f sec]\n", enesim_surface_format_name_get(ssf), end - start);
	}
	else
	{
		printf("        %s [NOT BUILT]\n", enesim_surface_format_name_get(ssf));
		return;
	}
	test_finish("span_pixel", rop, dst, src, NULL);
	enesim_surface_delete(dst);
	enesim_surface_delete(src);
}

static void drawer_point_color_transparent_bench(Enesim_Rop rop, Enesim_Surface_Format dsf)
{
	double start, end;
	Enesim_Surface *dst;
	Enesim_Surface_Data ddata;
	Enesim_Drawer_Point dpoint;
	Enesim_Color color;

	dst = enesim_surface_new(dsf, 1, 1);
	enesim_color_get(&color, 0xaa, 0xbb, 0xcc, 0xdd);
	enesim_surface_data_get(dst, &ddata);
	*ddata.argb8888.plane0 = color;
	enesim_color_get(&color, 0x11, 0x22, 0x33, 0x44);
	dpoint = enesim_drawer_point_color_get(rop, dsf, color);
	if (dpoint)
	{
		start = get_time();
		point_color_draw(dpoint, dst, color);
		end = get_time();
		printf("    Point transparent color [%3.3f sec]\n", end - start);
	}
	else
	{
		printf("    Point transparent color [NOT BUILT]\n");
		return;
	}
	test_finish("point_color_transparent", rop, dst, NULL, NULL);
	enesim_surface_delete(dst);
}

void drawer_bench(void)
{
	Enesim_Surface_Format ssf;

	printf("****************\n");
	printf("* Drawer Bench *\n");
	printf("****************\n");

	/* point functions */
	drawer_point_color_transparent_bench(opt_rop, opt_fmt);
	/* span functions */
	drawer_span_color_solid_bench(opt_rop, opt_fmt);
	drawer_span_color_transparent_bench(opt_rop, opt_fmt);

	printf("    Span pixel\n");
	for (ssf = ENESIM_SURFACE_ARGB8888; ssf < ENESIM_SURFACE_FORMATS; ssf++)
	{
		drawer_span_pixel_bench(opt_rop, opt_fmt, ssf);
	}
}
