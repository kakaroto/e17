#include "enesim_bench_common.h"

static void stripes(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;

	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_stripes_new();
	enesim_renderer_stripes_color_set(r, 0xffffff00, 0xff000000);
	enesim_renderer_stripes_thickness_set(r, 25, 25);
	renderer_run(r, dst, "Stripes", "stripes");
	enesim_renderer_delete(r);
}

void stripes_bench(void)
{

	printf("**************\n");
	printf("*   Stripes  *\n");
	printf("**************\n");
	stripes();
}
