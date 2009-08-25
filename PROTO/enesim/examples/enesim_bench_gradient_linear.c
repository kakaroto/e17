#include "enesim_bench_common.h"

void gradient_linear_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;

	printf("***********************\n");
	printf("*   Linear Gradient   *\n");
	printf("***********************\n");

	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_gradient_linear_new();
	enesim_renderer_gradient_linear_pos_set(r, 10, 10, -150, -150);
	enesim_renderer_gradient_stop_add(r, 0xff000000, 0);
	enesim_renderer_gradient_stop_add(r, 0xffff0000, 0.75);
	enesim_renderer_gradient_stop_add(r, 0xffffffff, 1);
	renderer_run(r, dst, "Gradient Linear", "gradient_linear");
}
