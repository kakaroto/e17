#include "enesim_bench_common.h"

void path_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;

	printf("**********\n");
	printf("*  Path  *\n");
	printf("**********\n");
	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_path_new();
	enesim_renderer_path_move_to(r, 2*12,2*27);
	enesim_renderer_path_cubic_to(r, 2*7,2*37, 2*18,2*50, 2*18,2*60);
	enesim_renderer_path_scubic_to(r, 2*0,2*80, 2*10,2*94);
	enesim_renderer_path_scubic_to(r, 2*40,2*74, 2*50,2*78);
	enesim_renderer_path_scubic_to(r, 2*60,2*99, 2*76,2*95);
	enesim_renderer_path_scubic_to(r, 2*72,2*70, 2*75,2*65);
	enesim_renderer_path_scubic_to(r, 2*95,2*55, 2*95,2*42);
	enesim_renderer_path_scubic_to(r, 2*69,2*37, 2*66,2*32);
	enesim_renderer_path_scubic_to(r, 2*67,2*2, 2*53,2*7);
	enesim_renderer_path_scubic_to(r, 2*43,2*17, 2*35,2*22);
	enesim_renderer_path_scubic_to(r, 2*17,2*17, 2*12,2*27);

#if 0
	enesim_renderer_path_move_to(r, 170, 42.85);
	enesim_renderer_path_line_to(r, 90, 354.28);
	enesim_renderer_path_line_to(r, 332.85, 92.85);
	enesim_renderer_path_line_to(r, 37.14, 107.14);
	enesim_renderer_path_line_to(r, 334.28, 324.28);
	enesim_renderer_path_line_to(r, 170, 42.85);
#endif

	shape_bench(r, dst, "Path", "path");
}
