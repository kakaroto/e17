#include "enesim_bench_common.h"

static void checker(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;

	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_checker_new();
	enesim_renderer_checker_color1_set(r, 0xff0000ff);
	enesim_renderer_checker_color2_set(r, 0xff8080ff);
	enesim_renderer_checker_size_set(r, 20, 20);
	renderer_run(r, dst, "Checker", "checker");
}

void checker_bench(void)
{

	printf("**************\n");
	printf("*   Checker  *\n");
	printf("**************\n");
	checker();
}
