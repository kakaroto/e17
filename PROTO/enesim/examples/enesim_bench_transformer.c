#include "enesim_bench_common.h"
/******************************************************************************
 *                      Transfomer benchmark functions                        *
 ******************************************************************************/
/* FIXME, this code is repeated, shall we export this function from the lib? */
static const char * transformer_get(Enesim_Transformation *tx)
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

static const char * _quality_get(Enesim_Transformation *tx)
{
	Enesim_Quality q = enesim_transformation_quality_get(tx);

	switch (q)
	{
		case ENESIM_FAST:
		return "fast";
		break;
		
		case ENESIM_GOOD:
		return "good";
		break;
	}
	return NULL;
}

static void transformer_go(Enesim_Transformation *tx)
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
		char name[256];

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
		printf("        %s %s [%3.3f sec]\n", enesim_surface_format_name_get(ssf), _quality_get(tx), end - start);

		snprintf(name, 256, "%s_%s", transformer_get(tx), _quality_get(tx));
		test_finish(name, opt_rop, dst, src, NULL, NULL);
		enesim_surface_delete(src);
		enesim_surface_delete(dst);
	}
}
/*
 * TODO transform with a mask/color and without
 */
void transformer_bench(void)
{
	Enesim_Surface_Pixel color;
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
	enesim_surface_pixel_components_from(&color, opt_fmt, 0xff, 0x00, 0x00, 0xff);
	enesim_transformation_color_set(tx, &color);

	/* identity matrix */
	enesim_matrix_identity(matrix);
	enesim_transformation_matrix_set(tx, matrix);
	printf("Identity\n");
	enesim_transformation_quality_set(tx, ENESIM_GOOD);
	transformer_go(tx);
	enesim_transformation_quality_set(tx, ENESIM_FAST);
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
	enesim_transformation_quality_set(tx, ENESIM_GOOD);
	transformer_go(tx);
	enesim_transformation_quality_set(tx, ENESIM_FAST);
	transformer_go(tx);
	/* projective */	
	q1 = enesim_quad_new();
	q2 = enesim_quad_new();
	enesim_quad_coords_set(q1, 0, 0, opt_width, 0, opt_width, opt_height, 0, opt_height);
	enesim_quad_coords_set(q2, 0, 100, 180, 0, 250, 180, 190, 240);
	//enesim_quad_coords_set(q2, 50, 50, 190, 10, 195, 140, 50, 240);
	enesim_matrix_quad_quad_to(matrix, q2, q1);
	enesim_transformation_matrix_set(tx, matrix);
	printf("Projective\n");
	enesim_transformation_quality_set(tx, ENESIM_GOOD);
	transformer_go(tx);
	enesim_transformation_quality_set(tx, ENESIM_FAST);
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
