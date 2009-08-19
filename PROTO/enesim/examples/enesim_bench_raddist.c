#include "enesim_bench_common.h"
/******************************************************************************
 *                         Raddist benchmark functions                        *
 ******************************************************************************/
/*
 * TODO transform with a mask/color and without
 */
void raddist_bench(void)
{
	Enesim_Surface *src = NULL;
	Enesim_Surface *dst = NULL;
	Enesim_Renderer *r;

	printf("*********************\n");
	printf("* Raddist Bench *\n");
	printf("*********************\n");
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, 0);
	r = enesim_renderer_raddist_new();
	enesim_renderer_raddist_src_set(r, src);
	enesim_renderer_raddist_radius_set(r, opt_width / 2);
	enesim_renderer_raddist_scale_set(r, 0.5);
	enesim_renderer_raddist_center_set(r, opt_width / 2, opt_height / 2);
	renderer_run(r, dst, "Raddist", "raddist");
}
