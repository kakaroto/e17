#include "enesim_bench_common.h"

void circle_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;


	printf("**************\n");
	printf("*    Circle  *\n");
	printf("**************\n");
	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_circle_new();
	enesim_renderer_circle_center_set(r, opt_width/2, opt_height/2);
	enesim_renderer_circle_radius_set(r, opt_width/3);
	shape_bench(r, dst, "Circle", "circle");
}
