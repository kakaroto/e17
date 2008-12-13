#include "enesim_bench_common.h"
/******************************************************************************
 *                        Renderer benchmark functions                        *
 ******************************************************************************/
void renderer_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *src;
	Enesim_Surface *dst;
	Enesim_Scanline_Alias sl;
	int i;
	double start, end;
	int t;
	
	printf("******************\n");
	printf("* Renderer Bench *\n");
	printf("******************\n");
	/* surface renderer */
	src = test_pattern(25);
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	r = enesim_renderer_surface_new();
	enesim_renderer_rop_set(r, opt_rop);
	enesim_renderer_surface_mode_set(r, ENESIM_SURFACE_REPEAT_Y | ENESIM_SURFACE_REPEAT_X);  
	enesim_renderer_surface_dst_area_set(r, 0, 0, opt_width, opt_height);
	enesim_renderer_surface_src_area_set(r, 0, 0, 25, 25);
	enesim_renderer_surface_surface_set(r, src);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		for (i = 0; i < opt_height; i++)
		{
			sl.x = 0;
			sl.y = i;
			sl.w = opt_width;
			enesim_renderer_draw(r, ENESIM_SCANLINE_ALIAS, &sl, dst);
		}
	}
	end = get_time();
	printf("Surface Renderer [%3.3f sec]\n", end - start);
	test_finish("renderer_surface", opt_rop, dst, NULL, NULL, NULL);
	/* color renderer */
}
