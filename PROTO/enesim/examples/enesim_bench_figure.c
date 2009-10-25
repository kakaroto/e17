#include "enesim_bench_common.h"

typedef struct _Point
{
	float x, y;
} Point;

void figure_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *dst = NULL;
	int i;
	Point p[] = {
		{ 170, 42.85 },
		{ 90, 354.28 },
		{ 332.85, 92.85 },
		{ 37.14, 107.14 },
		{ 334.28, 324.28 },
		{ 170, 42.85 },
	};

	printf("*************\n");
	printf("*  Polygon  *\n");
	printf("*************\n");
	surfaces_create(NULL, 0, NULL, 0, &dst, opt_fmt);

	r = enesim_renderer_figure_new();
	enesim_renderer_figure_polygon_add(r);

	for (i = 0; i < sizeof(p) / sizeof(Point); i++)
	{
		enesim_renderer_figure_polygon_vertex_add(r, p[i].x, p[i].y);
	}
	shape_bench(r, dst, "Figure", "figure");
}
