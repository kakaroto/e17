#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

/**
 * TODO
 * make the gradient functions draw pattern boxes
 */

#include "Eina.h"
#include "Enesim.h"

#include "image.h"

int opt_shift = 0;
int opt_width = 1020;
int opt_height = 1020;
int opt_times = 1;
FILE *opt_file;
int opt_debug = 0;
int opt_rop = ENESIM_FILL;
Enesim_Surface_Format opt_fmt = ENESIM_SURFACE_ARGB8888;

/* Place to store the result timmings for every operation */
typedef struct _Result
{
	double time;
} Result;

const char * rop_name(Enesim_Rop rop)
{
	switch (rop)
	{
		case ENESIM_BLEND:
			return "blend";
		case ENESIM_FILL:
			return "fill";
		default:
			return NULL;
	}
	return NULL;
}

int rop_get(const char *name, int *rop)
{
	int ret = 1;
	
	if (!strcmp(name, "blend"))
		*rop = ENESIM_BLEND;
	else if (!strcmp(name, "fill"))
		*rop = ENESIM_FILL;
	else
		ret = 0;
	return ret;
}

int fmt_get(const char *name, Enesim_Surface_Format *fmt)
{
	int ret = 1;
	if (!strcmp(name, "argb8888"))
		*fmt = ENESIM_SURFACE_ARGB8888;
	else
		ret = 0;
	return ret;
}

double get_time(void)
{
	struct timeval timev;

	gettimeofday(&timev, NULL);
	return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}


void surface_save(Enesim_Surface *s, const char *name)
{
	Enesim_Surface *img;
	int w, h;
	
	enesim_surface_size_get(s, &w, &h);
	img = enesim_surface_new(ENESIM_SURFACE_ARGB8888_UNPRE, w, h);
	//printf("Saving image %s\n", name);
	enesim_surface_convert(s, img);
	image_save(img, name, 0);
	enesim_surface_delete(img);
}

void test_gradient(Enesim_Surface *s)
{
	Enesim_Drawer_Span dspan;
	Enesim_Surface_Data sdata;
	Enesim_Surface_Format sfmt;
	int i;
	Enesim_Color color;
	int skip = 0;

	enesim_surface_data_get(s, &sdata);
	sfmt = enesim_surface_format_get(s);
	/* create a simple gradient */
	dspan = enesim_drawer_span_color_get(ENESIM_FILL, sfmt, 0xffffffff);
	if (!dspan)
	{
		return;
	}
	for (i = 0; i < opt_height; i++)
	{
		int j;
		Enesim_Surface_Data sdata_tmp;
		unsigned char col = ((opt_height - 1) - i) >> opt_shift;

		sdata_tmp = sdata;
		enesim_color_get(&color, col, col, 0, col);
#if 1
		if (!((i + 1) % 16))
			skip = (skip + 1) % 2;
		if (skip)
			enesim_surface_data_increment(&sdata_tmp, sfmt, 16);
		for (j = 0; j < opt_width; j += 32)
		{
			dspan(&sdata_tmp, 16, NULL, color, NULL);
			enesim_surface_data_increment(&sdata_tmp, sfmt, 32);
		}
#else
		dspan(&sdata_tmp, opt_width, NULL, color, NULL);
#endif
		enesim_surface_data_increment(&sdata, sfmt, opt_width);
	}
	surface_save(s, "gradient.png");
}

void test_gradient2(Enesim_Surface *s)
{
	Enesim_Drawer_Span dspan;
	Enesim_Surface_Data sdata;
	Enesim_Surface_Format sfmt;
	int i;
	Enesim_Color color;

	enesim_surface_data_get(s, &sdata);
	sfmt = enesim_surface_format_get(s);
	/* create a simple gradient */
	dspan = enesim_drawer_span_color_get(ENESIM_FILL, sfmt, 0xffffffff);
	if (!dspan)
	{
		return;
	}
	for (i = 0; i < opt_height; i++)
	{
		unsigned char col = i >> opt_shift;

		enesim_color_get(&color, col, 0, 0, col);
		dspan(&sdata, opt_width, NULL, color, NULL);
		enesim_surface_data_increment(&sdata, sfmt, opt_width);
	}
	surface_save(s, "gradient2.png");
}

Enesim_Surface * test_pattern(int w)
{
	Enesim_Surface *s;
	Enesim_Drawer_Span spfnc;
	Enesim_Color color;
	Enesim_Surface_Data sdata;
	int i;
	int spaces = w / 2;
	
	enesim_color_get(&color, 0xff, 0xff, 0xff, 0);
	spfnc = enesim_drawer_span_color_get(ENESIM_FILL, opt_fmt, color);
	if (!spfnc)
		return NULL;
	s = enesim_surface_new(opt_fmt, w, w);
	
	/* draw the pattern */
	enesim_surface_data_get(s, &sdata);
	enesim_surface_data_increment(&sdata, opt_fmt, spaces);
	for (i = 0; i < w / 2; i++)
	{
		int len = i * 2 +1;
		
		spfnc(&sdata, len, NULL, color, NULL);
		enesim_surface_data_increment(&sdata, opt_fmt, len + spaces + --spaces);
	}
	spfnc(&sdata, w, NULL, color, NULL);
	enesim_surface_data_increment(&sdata, opt_fmt, w + 1);
	spaces = 1;
	for (i = 0; i < w / 2; i++)
	{
		int len = (w - 1) - (i * 2 +1);
		
		spfnc(&sdata, len, NULL, color, NULL);
		enesim_surface_data_increment(&sdata, opt_fmt, len + spaces + ++spaces);
	}
	surface_save(s, "pattern.png");
	return s;
}

void test_finish(const char *name, Enesim_Rop rop, Enesim_Surface *dst,
		Enesim_Surface *src, Enesim_Surface *mask)
{
	Enesim_Surface_Format sfmt;
	char file[256];
	char tmp[256];

	if (!opt_debug)
		return;
	
	sfmt = enesim_surface_format_get(dst);
	snprintf(tmp, 256, "%s_%s_%s", name, rop_name(rop), enesim_surface_format_name_get(sfmt));
	if (src)
	{
		char tmp2[256];

		sfmt = enesim_surface_format_get(src);
		snprintf(tmp2, 256, "_%s", enesim_surface_format_name_get(sfmt));
		strncat(tmp, tmp2, 256);
	}
	if (mask)
	{
		char tmp2[256];

		sfmt = enesim_surface_format_get(mask);
		snprintf(tmp2, 256, "_%s", enesim_surface_format_name_get(sfmt));
		strncat(tmp, tmp2, 256);
	}
	/* now save the source image in case it exists */
	if (src)
	{
		char tmp2[256];

		sfmt = enesim_surface_format_get(src);
		snprintf(tmp2, 256, "%s_source.png", tmp);
		surface_save(src, tmp2);
	}
	snprintf(file, 256, "%s.png", tmp);
	surface_save(dst, file);
}

void point_color_draw(Enesim_Drawer_Point point, Enesim_Surface *dst, Enesim_Color color)
{
	int i;
	int t;
	Enesim_Surface_Data dtmp;
	enesim_surface_data_get(dst, &dtmp);

	for (t = 0; t < opt_times; t++)
	{
		point(&dtmp, NULL, color, NULL);
	}
}

void span_color_draw(Enesim_Drawer_Span span, Enesim_Surface *dst, unsigned int len,
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
/******************************************************************************
 *                         Drawer benchmark functions                         *
 ******************************************************************************/
/* drawer span solid color bench */
void drawer_span_color_solid_bench(Enesim_Rop rop, Enesim_Surface_Format dsf)
{
	double start, end;
	Enesim_Color color;
	Enesim_Surface *dst;
	Enesim_Surface_Data ddata;
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
void drawer_span_color_transparent_bench(Enesim_Rop rop, Enesim_Surface_Format dsf)
{
	double start, end;
	Enesim_Surface *dst;
	Enesim_Surface_Data ddata;
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
void drawer_span_pixel_bench(Enesim_Rop rop, Enesim_Surface_Format dsf, Enesim_Surface_Format ssf)
{
	double start, end;
	int color;
	Enesim_Surface *dst, *src;
	Enesim_Surface_Data ddata;
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

void drawer_point_color_transparent_bench(Enesim_Rop rop, Enesim_Surface_Format dsf)
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
/******************************************************************************
 *                      Transfomer benchmark functions                        *
 ******************************************************************************/
/* FIXME, this code is repeated, shall we export this function from the lib? */
const char * transformer_get(Enesim_Transformation *tx)
{
	Enesim_Matrix *t;
	float a, b, c, d, e, f, g, h, i;

#define MATRIX_XX 0
#define MATRIX_XY 1
#define MATRIX_XZ 2
#define MATRIX_YX 3
#define MATRIX_YY 4
#define MATRIX_YZ 5
#define MATRIX_ZX 6
#define MATRIX_ZY 7
#define MATRIX_ZZ 8
	t = enesim_matrix_new();
	enesim_transformation_matrix_get(tx, t);
	enesim_matrix_values_get(t, &a, &b, &c, &d, &e, &f, &g, &h, &i);
	
	if ((g != 0) || (h != 0) || (i != 1))
	            return "projective";
	else
	{
		if ((a == 1) && (b == 0) && (c == 0) && (d == 0) && (e == 1)
				&& (f == 0))
			return "identity";
		else
			return "affine";
	}
	enesim_matrix_delete(t);
}
void transformer_go(Enesim_Transformation *tx)
{
	Eina_Rectangle sr, dr;
	Enesim_Surface_Format ssf;
	double start, end;

	eina_rectangle_coords_from(&sr, 0, 0, opt_width, opt_height);
	eina_rectangle_coords_from(&dr, 0, 0, opt_width, opt_height);
	for (ssf = ENESIM_SURFACE_ARGB8888; ssf < ENESIM_SURFACE_FORMATS; ssf++)
	{
		Enesim_Surface *dst;
		Enesim_Surface *src;
		int t;

		src = enesim_surface_new(opt_fmt, opt_width, opt_height);
		test_gradient(src);
		dst = enesim_surface_new(ssf, opt_width, opt_height);
		test_gradient2(dst);	
			
		start = get_time();
		for (t = 0; t < opt_times; t++)
		{
			enesim_transformation_apply(tx, src, &sr, dst, &dr);
		}
		end = get_time();
		printf("        %s [%3.3f sec]\n", enesim_surface_format_name_get(ssf), end - start);

		test_finish(transformer_get(tx), opt_rop, dst,
					src, NULL);
		enesim_surface_delete(src);
		enesim_surface_delete(dst);
	}
}

void transformer_bench(void)
{
	Enesim_Matrix *matrix, *tmp;
	Enesim_Quad *q1, *q2;
	Enesim_Transformation *tx;
	float angle, ca, sa;

	printf("*********************\n");
	printf("* Transformer Bench *\n");
	printf("*********************\n");

	matrix = enesim_matrix_new();
	tmp = enesim_matrix_new();
	
	tx = enesim_transformation_new();
	enesim_transformation_rop_set(tx, opt_rop);
	
	/* identity matrix */
	enesim_matrix_identity(matrix);
	enesim_transformation_matrix_set(tx, matrix);
	printf("Identity\n");
	transformer_go(tx);
	/* affine matrix */
	/* do a scale, rotate and translate */
	enesim_matrix_translate(matrix, opt_width/2, opt_height/2);
	enesim_matrix_scale(tmp, 2, 2);
	enesim_matrix_compose(matrix, tmp, matrix);
	enesim_matrix_rotate(tmp, M_PI/4);
	enesim_matrix_compose(matrix, tmp, matrix);
	enesim_matrix_translate(tmp, -opt_width/2, -opt_height/2);
	enesim_matrix_compose(matrix, tmp, matrix);
	enesim_transformation_matrix_set(tx, matrix);
	printf("Affine\n");
	transformer_go(tx);
	/* projective */	
	q1 = enesim_quad_new();
	q2 = enesim_quad_new();
	enesim_quad_coords_set(q1, 0, 0, opt_width, 0, opt_width, opt_height, 0, opt_height);
	enesim_quad_coords_set(q2, 0, 100, 180, 0, 250, 180, 190, 240);
	//enesim_quad_coords_from(q2, 50, 50, 190, 10, 195, 140, 50, 240);
	enesim_matrix_quad_quad_to(matrix, q2, q1);
	enesim_transformation_matrix_set(tx, matrix);
	printf("Projective\n");
	transformer_go(tx);
	
	enesim_quad_delete(q1);
	enesim_quad_delete(q2);
	enesim_matrix_delete(matrix);
	enesim_matrix_delete(tmp);
}
void matrix_bench(void)
{
	Enesim_Quad *q1, *q2;
	Enesim_Matrix *m, *m2;
	float x, y, xr, yr;

	m = enesim_matrix_new();
	m2 = enesim_matrix_new();
	q1 = enesim_quad_new();
	q2 = enesim_quad_new();
	
	enesim_quad_coords_set(q1, 0, 0, opt_width, 0, opt_width, opt_height, 0, opt_height);
	enesim_quad_coords_set(q2, 0, 100, 180, 0, 250, 180, 190, 240);
			
	/* transforming from a square to a quad */
	printf("square to quad\n");
	enesim_matrix_square_quad_to(m, q2);
	x = 0;
	y = 1;
	enesim_matrix_point_transform(m, x, y, &xr, &yr);
	printf("x = %f x' = %f, y = %f y' = %f\n", x, xr, y, yr);
	/* scale */
	printf("scale\n");
	enesim_matrix_scale(m2, opt_width, opt_height);
	x = 0;
	y = 1;
	enesim_matrix_point_transform(m2, x, y, &xr, &yr);
	printf("x = %f x' = %f, y = %f y' = %f\n", x, xr, y, yr);
	/* quad to square */
	printf("quad to square\n");
#if 0
	enesim_matrix_quad_square_to(m, q2);
	x = q2[2];
	y = q2[3];
	enesim_matrix_point_transform(m, x, y, &xr, &yr);
	printf("x = %f x' = %f, y = %f y' = %f\n", x, xr, y, yr);
	/* quad to square */
	printf("quad to quad\n");
	enesim_matrix_quad_quad_to(m, q1, q2);
	x = q1[4];
	y = q1[5];
	enesim_matrix_point_transform(m, x, y, &xr, &yr);
	printf("x = %f x' = %f, y = %f y' = %f\n", x, xr, y, yr);
	
	enesim_matrix_delete(m);
	enesim_matrix_delete(m2);
	enesim_quad_delete(q1);
	enesim_quad_delete(q2);
#endif
}
/******************************************************************************
 *                      Rasterizer benchmark functions                        *
 ******************************************************************************/
/* this callbacks wont draw anything */
void rasterizer_callback(void *sl_data, int sl_type, void *data)
{

}
void rasterizer_bench(void)
{
	static struct Points {
		float x, y;
	} points[] = {
		{ 170, 42.85 },
		{ 90, 354.28 },
		{ 332.85, 92.85 },
		{ 37.14, 107.14 },
		{ 334.28, 324.28 },
		{ 170, 42.85 },
	};
	Eina_Rectangle rect;
        Enesim_Rasterizer *rs;
        int i;
        int t;
	double start, end;

#define POINTS_NUM 6
#define MAX_X 335
#define MAX_Y 325
	printf("********************\n");
	printf("* Rasterizer Bench *\n");
	printf("********************\n");
	/* Test every rasterizer */
	eina_rectangle_coords_from(&rect, 0, 0, MAX_X, MAX_Y);
	rs = enesim_rasterizer_cpsc_new(rect);
	for (i = 0; i < POINTS_NUM; i++)
	{
		enesim_rasterizer_vertex_add(rs, points[i].x, points[i].y);
	}
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_rasterizer_generate(rs, rasterizer_callback, NULL);
	}
	end = get_time();
	printf("CPSC [%3.3f sec]\n", end - start);
	enesim_rasterizer_delete(rs);
#if 0
	rs = enesim_rasterizer_kiia_new(ENESIM_RASTERIZER_KIIA_COUNT_8, rect);
	for (i = 0; i < POINTS_NUM; i++)
	{
		enesim_rasterizer_vertex_add(rs, points[i].x, points[i].y);
	}
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_rasterizer_generate(rs, rasterizer_callback, NULL);
	}
	end = get_time();
	printf("KIIA8 [%3.3f sec]\n", end - start);
	enesim_rasterizer_delete(rs);
#endif
#undef POINTS_NUM
#undef MAX_X
#undef MAX_Y
}

/******************************************************************************
 *                        Renderer benchmark functions                        *
 ******************************************************************************/
void renderer_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *src;
	Enesim_Surface *dst;
	Enesim_Scanline_Alias sl;
	int i;
	double start, end;
	int t;
	
	printf("******************\n");
	printf("* Renderer Bench *\n");
	printf("******************\n");
	/* surface renderer */
	src = test_pattern(25);
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	r = enesim_renderer_surface_new();
	enesim_renderer_rop_set(r, opt_rop);
	enesim_renderer_surface_mode_set(r, ENESIM_SURFACE_REPEAT_Y | ENESIM_SURFACE_REPEAT_X);  
	enesim_renderer_surface_dst_area_set(r, 0, 0, opt_width, opt_height);
	enesim_renderer_surface_src_area_set(r, 0, 0, 25, 25);
	enesim_renderer_surface_surface_set(r, src);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		for (i = 0; i < opt_height; i++)
		{
			sl.x = 0;
			sl.y = i;
			sl.w = opt_width;
			enesim_renderer_draw(r, ENESIM_SCANLINE_ALIAS, &sl, dst);
		}
	}
	end = get_time();
	printf("Surface Renderer [%3.3f sec]\n", end - start);
	test_finish("renderer_surface", opt_rop, dst, NULL, NULL);
	/* color renderer */
}

void help(void)
{
	printf("Usage: enesim_bench [OPTION]...\n");
	printf("Runs a benchmark test for enesim operations\n");
	printf("Option can be one of the following:\n");
	printf("-h, --help            Print this screen\n");
	printf("-t, --times           Number of times to run the test [10]\n");
	printf("-d, --opt_debug       Dump temporal images [off]\n");
	printf("-f, --file            File to place the statistics [benchmark.txt]\n");
	printf("-r, --rop             Raster operation to use [fill], use help to get a list\n");
	printf("-m, --fmt             Surface format to use [argb8888_pre], use help to get a list\n");
}

void rop_help(void)
{
	int rop;
	
	for (rop = 0; rop < ENESIM_ROPS; rop++)
	{
		printf("%s\n", rop_name(rop));
	}
}

void fmt_help(void)
{
	int fmt;
	
	for (fmt = 0; fmt < ENESIM_SURFACE_FORMATS; fmt++)
	{
		printf("%s\n", enesim_surface_format_name_get(fmt));
	}
}


int main(int argc, char **argv)
{
	char *short_options = "dhf:t:r:m:";
	struct option long_options[] = {
		{"opt_debug", 0, 0, 'd'},
		{"help", 0, 0, 'h'},
		{"times", 0, 0, 't'},
		{"file", 0, 0, 'f'},
		{"rop", 0, 0, 'r'},
		{"fmt", 0, 0, 'm'},
		{0, 0, 0, 0}
	};
	int option;
	char c;
	char *file = "benchmark.txt";

	/* handle the parameters */
	while ((c = getopt_long(argc, argv, short_options, long_options,
	                                &option)) != -1)
	{
		/* arm bug ? */
		if (c == 255)
			goto ok;
		switch (c)
		{
			case 'h':
				help();
				return 0;
			case 't':
				opt_times = atoi(optarg);
				break;
			case 'f':
				file = optarg;
				opt_file = fopen(file, "w+");
				if (!opt_file)
				{
					help();
					return 0;
				}
				break;
			case 'd':
				opt_debug = 1;
				break;
			case 'r':
				if (!rop_get(optarg, &opt_rop))
				{
					rop_help();
					return 1;
				}
				break;
			case 'm':
				
				if (!fmt_get(optarg, &opt_fmt))
				{
					fmt_help();
					return 1;
				}
				break;
			default:
				break;
		}
	}
ok:
	printf("Enesim Bench\n");
	printf("* ROP = %s\n", rop_name(opt_rop));
	printf("* FMT = %s\n", enesim_surface_format_name_get(opt_fmt));
	enesim_init();
	drawer_bench();
	transformer_bench();
	rasterizer_bench();
	renderer_bench();
	enesim_shutdown();
	/* this bench should be on test
	 * matrix_bench();
	 */

	return 0;
}
