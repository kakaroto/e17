#include "enesim_bench_common.h"

void ellipse_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;

	printf("**************\n");
	printf("*   Ellipse  *\n");
	printf("**************\n");
	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_ellipse_new();
	enesim_renderer_ellipse_center_set(r, opt_width/2, opt_height/2);
	enesim_renderer_ellipse_radii_set(r, opt_width/3, opt_width/4);
	shape_bench(r, dst, "Ellipse", "ellipse");
}
