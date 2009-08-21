#include "enesim_bench_common.h"

void rectangle_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;


	printf("**************\n");
	printf("*  Rectangle *\n");
	printf("**************\n");
	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_rectangle_new();
	enesim_renderer_rectangle_size_set(r, opt_width, opt_height);
	shape_bench(r, dst, "Rectangle", "rectangle");
}
