#include "enesim_bench_common.h"

static void hswitch(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *right = NULL;
	Enesim_Surface *left= NULL;
	Enesim_Surface *dst= NULL;

	surfaces_create(&left, opt_fmt, &right, opt_fmt, &dst, opt_fmt);
	r = enesim_renderer_hswitch_new();
	enesim_renderer_hswitch_left_set(r, left);
	enesim_renderer_hswitch_right_set(r, right);
	enesim_renderer_hswitch_h_set(r, opt_height);
	enesim_renderer_hswitch_w_set(r, opt_width);
	enesim_renderer_hswitch_step_set(r, 0.3);
	renderer_run(r, dst, "Horizontal Switch", "horswitch");
}

void hswitch_bench(void)
{

	printf("****************\n");
	printf("*   Horswitch  *\n");
	printf("****************\n");
	hswitch();
}

