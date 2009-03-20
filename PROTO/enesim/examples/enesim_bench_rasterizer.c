#include "enesim_bench_common.h"
/******************************************************************************
 *                      Rasterizer benchmark functions                        *
 ******************************************************************************/
/* this callbacks wont draw anything */
void rasterizer_callback(void *sl_data, int sl_type, void *data)
{

}
void rasterizer_bench(void)
{
	static struct Points {
		float x, y;
	} points[] = {
		{ 170, 42.85 },
		{ 90, 354.28 },
		{ 332.85, 92.85 },
		{ 37.14, 107.14 },
		{ 334.28, 324.28 },
		{ 170, 42.85 },
	};
	Eina_Rectangle rect;
	Enesim_Rasterizer *rs;
	int i;
	int t;
	double start, end;

#define POINTS_NUM 6
#define MAX_X 335
#define MAX_Y 325
	printf("********************\n");
	printf("* Rasterizer Bench *\n");
	printf("********************\n");
	/* Test every rasterizer */
	eina_rectangle_coords_from(&rect, 0, 0, MAX_X, MAX_Y);
	rs = enesim_rasterizer_cpsc_new(rect);
	for (i = 0; i < POINTS_NUM; i++)
	{
		enesim_rasterizer_vertex_add(rs, points[i].x, points[i].y);
	}
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_rasterizer_generate(rs, rasterizer_callback, NULL);
	}
	end = get_time();
	printf("CPSC [%3.3f sec]\n", end - start);
	enesim_rasterizer_delete(rs);
#if 0
	rs = enesim_rasterizer_kiia_new(ENESIM_RASTERIZER_KIIA_COUNT_8, rect);
	for (i = 0; i < POINTS_NUM; i++)
	{
		enesim_rasterizer_vertex_add(rs, points[i].x, points[i].y);
	}
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_rasterizer_generate(rs, rasterizer_callback, NULL);
	}
	end = get_time();
	printf("KIIA8 [%3.3f sec]\n", end - start);
	enesim_rasterizer_delete(rs);
#endif
#undef POINTS_NUM
#undef MAX_X
#undef MAX_Y
}
