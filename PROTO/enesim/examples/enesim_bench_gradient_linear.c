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
	renderer_run(r, dst, "Gradient Linear", "gradient_linear");
}
