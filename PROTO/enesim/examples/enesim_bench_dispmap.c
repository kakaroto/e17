#include "enesim_bench_common.h"
/******************************************************************************
 *                             Dispmap functions                              *
 ******************************************************************************/

#if 0
static void dispmap_map_create(Enesim_Surface **s)
{
	uint32_t *d;
	uint32_t y;
	int i;
	int stride;

	*s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, opt_width, opt_height);
	enesim_compositor_span_color_get(opt_cpu, &op, ENESIM_FILL, opt_fmt, 0xaaffffff);
	d = enesim_surface_data_get(*s);
	stride = enesim_surface_stride_get(*s);

	for (i = 0; i < opt_height / 3; i++)
	{
		enesim_operator_drawer_span(&op, d, opt_width, NULL, 0xffffffff, NULL);
		d += stride;
	}
	for (i = 0; i < opt_height / 3; i++)
	{
		enesim_operator_drawer_span(&op, d, opt_width, NULL, 0x77777777, NULL);
		d += stride;
	}
	for (i = 0; i < opt_height / 3; i++)
	{
		enesim_operator_drawer_span(&op, d, opt_width, NULL, 0x0, NULL);
		d += stride;
	}

}
#endif

void dispmap_bench(void)
{
	Enesim_Surface *src = NULL;
	Enesim_Surface *dst = NULL;
	Enesim_Surface *map = NULL;
	Enesim_Renderer *r;

	printf("*********************\n");
	printf("*   Dispmap Bench   *\n");
	printf("*********************\n");

#if 1
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, &map, opt_fmt);
#else
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, 0);
	dispmap_map_create(&map);
#endif

	r = enesim_renderer_dispmap_new();
	enesim_renderer_dispmap_map_set(r, map);
	enesim_renderer_dispmap_src_set(r, src);
	enesim_renderer_dispmap_scale_set(r, 10);
	renderer_run(r, dst, "Dispmap", "dispmap");
}
