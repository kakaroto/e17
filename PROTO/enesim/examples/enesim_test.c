#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Enesim.h"
#include "fixed_16p16.h"
#include "enesim_test.h"
#include "config.h"

#define DSTDIR "/tmp"

/*============================================================================*
 *                                 Common                                     * 
 *============================================================================*/
struct Rasterizer_Cb_Data
{
	Enesim_Renderer *r;
	Enesim_Surface *s;
};

#define TIGER_WIDTH 400
#define TIGER_HEIGHT 412
Enesim_Surface * tiger_draw(void)
{
	Enesim_Surface *s;
	s = surface_new(TIGER_WIDTH, TIGER_HEIGHT, ENESIM_SURFACE_ARGB8888);
	png_load(s, PACKAGE_DATA_DIR"/tiger.png");
	return s;
}
/*============================================================================*
 * Test3:                                                                     *
 * Surface draw test                                                          *
 *============================================================================*/
#define MATRIX_XX 0
#define MATRIX_XY 1
#define MATRIX_XZ 2
#define MATRIX_YX 3
#define MATRIX_YY 4
#define MATRIX_YZ 5
#define MATRIX_ZX 6
#define MATRIX_ZY 7
#define MATRIX_ZZ 8
#define MATRIX_SIZE 9

#define PI 3.14159265

static void transformation_compose(float *st, float *dt)
{
	int i;
	float tmp[MATRIX_SIZE];
	
	tmp[MATRIX_XX] = (st[MATRIX_XX] * dt[MATRIX_XX]) + (st[MATRIX_XY] * dt[MATRIX_YX]) + (st[MATRIX_XZ] * dt[MATRIX_ZX]);
	tmp[MATRIX_XY] = (st[MATRIX_XX] * dt[MATRIX_XY]) + (st[MATRIX_XY] * dt[MATRIX_YY]) + (st[MATRIX_XZ] * dt[MATRIX_ZY]);
	tmp[MATRIX_XZ] = (st[MATRIX_XX] * dt[MATRIX_XZ]) + (st[MATRIX_XY] * dt[MATRIX_YZ]) + (st[MATRIX_XZ] * dt[MATRIX_ZZ]);
	
	tmp[MATRIX_YX] = (st[MATRIX_YX] * dt[MATRIX_XX]) + (st[MATRIX_YY] * dt[MATRIX_YX]) + (st[MATRIX_YZ] * dt[MATRIX_ZX]);
	tmp[MATRIX_YY] = (st[MATRIX_YX] * dt[MATRIX_XY]) + (st[MATRIX_YY] * dt[MATRIX_YY]) + (st[MATRIX_YZ] * dt[MATRIX_ZY]);
	tmp[MATRIX_YZ] = (st[MATRIX_YX] * dt[MATRIX_XZ]) + (st[MATRIX_YY] * dt[MATRIX_YZ]) + (st[MATRIX_YZ] * dt[MATRIX_ZZ]);

	tmp[MATRIX_ZX] = (st[MATRIX_ZX] * dt[MATRIX_XX]) + (st[MATRIX_ZY] * dt[MATRIX_YX]) + (st[MATRIX_ZZ] * dt[MATRIX_ZX]);
	tmp[MATRIX_ZY] = (st[MATRIX_ZX] * dt[MATRIX_XY]) + (st[MATRIX_ZY] * dt[MATRIX_YY]) + (st[MATRIX_ZZ] * dt[MATRIX_ZY]);
	tmp[MATRIX_ZZ] = (st[MATRIX_ZX] * dt[MATRIX_XZ]) + (st[MATRIX_ZY] * dt[MATRIX_YZ]) + (st[MATRIX_ZZ] * dt[MATRIX_ZZ]);

	for (i = 0; i < MATRIX_SIZE; i++)
		st[i] = tmp[i];
}

static void transformation_translate(float *t, float tx, float ty)
{
	t[MATRIX_XX] = 1;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = tx;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = 1;
	t[MATRIX_YZ] = ty;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;	
}
static void transformation_scale(float *t, float sx, float sy)
{
	t[MATRIX_XX] = sx;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = sy;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;
}
static void transformation_rotate(float *t, float rad)
{
	float c = cos(rad);
	float s = sin(rad);
	
	t[MATRIX_XX] = c;
	t[MATRIX_XY] = -s;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = s;
	t[MATRIX_YY] = c;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;	
}

static void transformation_identity(float *t)
{
	t[MATRIX_XX] = 1;
	t[MATRIX_XY] = 0;
	t[MATRIX_XZ] = 0;
	t[MATRIX_YX] = 0;
	t[MATRIX_YY] = 1;
	t[MATRIX_YZ] = 0;
	t[MATRIX_ZX] = 0;
	t[MATRIX_ZY] = 0;
	t[MATRIX_ZZ] = 1;
}
void test3(void)
{
	Enesim_Surface *dst, *src;
	Enesim_Rectangle srect;
	Enesim_Rectangle drect;
	float m1[9], m2[9];

	src = tiger_draw();
	dst = surface_new(TIGER_WIDTH, TIGER_HEIGHT, ENESIM_SURFACE_ARGB8888);
	png_save(src, DSTDIR"/enesim_test3_src.png", 0);
	
	srect.x = 0;
	srect.y = 0;
	enesim_surface_size_get(src, &srect.w, &srect.h);
	
	drect.x = 0;
	drect.y = 0;
	drect.w = srect.w;
	drect.h = srect.h;
	
	transformation_scale(m1, 2.0, 2.0);
	transformation_translate(m2, -20, -26);
	transformation_compose(m1, m2);
	transformation_rotate(m2, PI/4);
	transformation_compose(m1, m2);
	/*
	transformation_translate(m2, 200, 206);
	transformation_compose(m1, m2);*/
	
	enesim_surface_transformation_set(src, m1);
	enesim_surface_draw(src, &srect, dst, &drect, 0, 0);
	png_save(dst, DSTDIR"/enesim_test3_dst.png", 0);
	surface_free(dst);
	surface_free(src);
}
/*============================================================================*
 * Test2:                                                                     *
 * Rasterizers comparative                                                    *
 *============================================================================*/
#define STAR_WIDTH 400
#define STAR_HEIGHT 400

void test2(void)
{
}

/*============================================================================*
 * Test1:                                                                     *
 * Fixed point calculations                                                   *
 *============================================================================*/
void test1(void)
{
	enesim_16p16_t fp, res;
	float r;

	/* sqrt(2) */
	fp = enesim_16p16_int_from(2);
	res = enesim_16p16_sqrt(fp);
	r = enesim_16p16_float_to(res);
	printf("sqrt(2) = %f\n", r);
}

int main(int argc, char **argv)
{
	int i = 1;

	if (argc < 2) return -1;
	while (i < argc)
	{
		if (!strcmp(argv[i], "test1"))
			test1();
		else if (!strcmp(argv[i], "test2"))
			test2();
		else if (!strcmp(argv[i], "test3"))
			test3();
		i++;
	}
	return 0;
}
