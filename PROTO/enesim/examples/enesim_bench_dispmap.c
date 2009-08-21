#include "enesim_bench_common.h"
/******************************************************************************
 *                             Dispmap functions                              *
 ******************************************************************************/
static void dispmap_map_create(Enesim_Surface **s)
{
	Enesim_Renderer *r;
	int i;
	uint32_t *data;

	*s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, opt_width, opt_height);
	data = enesim_surface_data_get(*s);

	r = enesim_renderer_perlin_new();
	enesim_renderer_perlin_octaves_set(r, 4);
	enesim_renderer_perlin_frequency_set(r, 0.01);
	enesim_renderer_perlin_persistence_set(r, 0.9);
	enesim_renderer_state_setup(r);
	for (i = 0; i < opt_height; i++)
	{
		enesim_renderer_span_fill(r, 0, i, opt_width, data);
		data += opt_width;
	}
	enesim_renderer_state_cleanup(r);
	enesim_renderer_delete(r);
}

void dispmap_bench(void)
{
	Enesim_Surface *src = NULL;
	Enesim_Surface *dst = NULL;
	Enesim_Surface *map = NULL;
	Enesim_Renderer *r;

	printf("*********************\n");
	printf("*   Dispmap Bench   *\n");
	printf("*********************\n");

	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, 0);
	dispmap_map_create(&map);

	r = enesim_renderer_dispmap_new();
	enesim_renderer_dispmap_map_set(r, map);
	enesim_renderer_dispmap_src_set(r, src);
	enesim_renderer_dispmap_scale_set(r, 10);
	renderer_run(r, dst, "Dispmap", "dispmap");
}
