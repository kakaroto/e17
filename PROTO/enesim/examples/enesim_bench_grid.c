#include "enesim_bench_common.h"

static void grid(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;

	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_grid_new();
	enesim_renderer_grid_inside_size_set(r, 15, 15);
	enesim_renderer_grid_inside_color_set(r, 0xffffffff);
	enesim_renderer_grid_outside_size_set(r, 3, 3);
	enesim_renderer_grid_outside_color_set(r, 0xff000000);
	renderer_run(r, dst, "Grid", "grid");
}

void grid_bench(void)
{

	printf("**********\n");
	printf("*  Grid  *\n");
	printf("**********\n");
	grid();
}
