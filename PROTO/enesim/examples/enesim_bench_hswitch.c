#include "enesim_bench_common.h"

static void surface_set(Enesim_Renderer **r, Enesim_Surface *s)
{
	*r = enesim_renderer_surface_new();
	enesim_renderer_surface_src_set(*r, s);
	enesim_renderer_surface_w_set(*r, opt_width);
	enesim_renderer_surface_h_set(*r, opt_height);
}

static void hswitch(void)
{
	Enesim_Renderer *r;
	Enesim_Renderer *lrend, *rrend;
	Enesim_Surface *right = NULL;
	Enesim_Surface *left= NULL;
	Enesim_Surface *dst= NULL;

	surfaces_create(&left, opt_fmt, &right, opt_fmt, &dst, opt_fmt);
	surface_set(&lrend, left);
	surface_set(&rrend, right);

	r = enesim_renderer_hswitch_new();
	enesim_renderer_hswitch_left_set(r, lrend);
	enesim_renderer_hswitch_right_set(r, rrend);
	enesim_renderer_hswitch_h_set(r, opt_height);
	enesim_renderer_hswitch_w_set(r, opt_width);
	enesim_renderer_hswitch_step_set(r, 0.48);
	renderer_run(r, dst, "Horizontal Switch", "horswitch");
}

void hswitch_bench(void)
{

	printf("****************\n");
	printf("*   Horswitch  *\n");
	printf("****************\n");
	hswitch();
}

