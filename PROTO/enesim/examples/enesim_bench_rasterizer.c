#include "enesim_bench_common.h"
/******************************************************************************
 *                      Rasterizer benchmark functions                        *
 ******************************************************************************/
typedef struct Points
{
	float x, y;
} Points;


typedef struct _Enesim_Rasterizer_Compositor
{
	Enesim_Surface *dst;
	Enesim_Compositor_Span span;
	Enesim_Compositor_Point point;
} Enesim_Rasterizer_Compositor;

#define POINTS_NUM 6

/* this callbacks wont draw anything */
static void rasterizer_dummy_callback(Enesim_Scanline *sl, void *data)
{
}

static void rasterizer_point_alias_callback(Enesim_Scanline *sl, void *data)
{
	Enesim_Rasterizer_Compositor *rcmp = data;
	uint32_t *ddata;
	uint32_t stride;

	ddata = enesim_surface_data_get(rcmp->dst);
	stride = enesim_surface_stride_get(rcmp->dst);
	ddata = ddata +(sl->data.alias.y * stride) + sl->data.alias.x;
	*ddata = 0xffffffff;//enesim_operator_drawer_point(&rcmp->op, ddata, 0, 0x00000000, 0);
}

static void rasterizer_span_alias_callback(Enesim_Scanline *sl, void *data)
{
	Enesim_Rasterizer_Compositor *rcmp = data;
	uint32_t *ddata;
	uint32_t stride;

	ddata = enesim_surface_data_get(rcmp->dst);
	stride = enesim_surface_stride_get(rcmp->dst);
	ddata = ddata +(sl->data.alias.y * stride) + sl->data.alias.x;
	rcmp->span(ddata, sl->data.alias.w, NULL, 0xffffffff, NULL);
}

static void _cpsc_bench(Points *p, Eina_Rectangle *rect)
{
	Enesim_Rasterizer *rs;
	int i;
	int t;
	double start, end;

	rs = enesim_rasterizer_cpsc_new();
	for (i = 0; i < POINTS_NUM; i++)
	{
		enesim_rasterizer_vertex_add(rs, p[i].x, p[i].y);
	}
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_rasterizer_generate(rs, rect, rasterizer_dummy_callback, NULL);
	}
	end = get_time();
	printf("CPSC [%3.3f sec]\n", end - start);
	enesim_rasterizer_delete(rs);

}

static void _circle_bench(Eina_Rectangle *rect)
{
	Enesim_Rasterizer *rs;
	Enesim_Scanline_Callback cb;
	Enesim_Rasterizer_Compositor rcmp;
	Enesim_Surface *dst;
	int t;
	double start, end;
	void *data = NULL;
	uint32_t color = 0xaaaaaaa;
	Enesim_Format dfmt = opt_fmt;

	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	if (opt_debug)
	{
		rcmp.point = enesim_compositor_point_get(ENESIM_FILL, &dfmt, ENESIM_FORMAT_NONE,
				color, ENESIM_FORMAT_NONE);
		rcmp.dst = dst;
		cb = rasterizer_point_alias_callback;
		data = &rcmp;
	}
	else
	{
		cb = rasterizer_dummy_callback;
	}
	rs = enesim_rasterizer_circle_new();
	enesim_rasterizer_circle_radius_set(rs, opt_width/4);
	enesim_rasterizer_vertex_add(rs, opt_width/2, opt_height/2);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_rasterizer_generate(rs, rect, cb, data);
	}
	end = get_time();
	printf("Circle [%3.3f sec]\n", end - start);
	test_finish("rasterizer_circle", opt_rop, dst, NULL, &color, NULL);
	enesim_surface_delete(dst);
	enesim_rasterizer_delete(rs);
}

static void _circle_fill_bench(Eina_Rectangle *rect)
{
	Enesim_Rasterizer *rs;
	Enesim_Scanline_Callback cb;
	Enesim_Rasterizer_Compositor rcmp;
	Enesim_Surface *dst;
	int t;
	double start, end;
	void *data = NULL;
	uint32_t color = 0xaaaaaaa;
	Enesim_Format dfmt = opt_fmt;

	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	if (opt_debug)
	{
		rcmp.span = enesim_compositor_span_get(ENESIM_FILL, &dfmt, ENESIM_FORMAT_NONE,
				color, ENESIM_FORMAT_NONE);
		rcmp.dst = dst;
		cb = rasterizer_span_alias_callback;
		data = &rcmp;
	}
	else
	{
		cb = rasterizer_dummy_callback;
	}
	rs = enesim_rasterizer_circle_new();
	enesim_rasterizer_circle_radius_set(rs, opt_width/4);
	enesim_rasterizer_vertex_add(rs, opt_width/2, opt_height/2);
	enesim_rasterizer_circle_fill_policy_set(rs, ENESIM_RASTERIZER_FILL_POLICY_FILL);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_rasterizer_generate(rs, rect, cb, data);
	}
	end = get_time();
	printf("Circle Fill [%3.3f sec]\n", end - start);
	test_finish("rasterizer_circle_fill", opt_rop, dst, NULL, &color, NULL);
	enesim_surface_delete(dst);
	enesim_rasterizer_delete(rs);
}


void rasterizer_bench(void)
{
	Points points[] = {
		{ 170, 42.85 },
		{ 90, 354.28 },
		{ 332.85, 92.85 },
		{ 37.14, 107.14 },
		{ 334.28, 324.28 },
		{ 170, 42.85 },
	};
	Eina_Rectangle rect;


	printf("********************\n");
	printf("* Rasterizer Bench *\n");
	printf("********************\n");

	/* Test every rasterizer */
	eina_rectangle_coords_from(&rect, 0, 0, opt_width, opt_height);
	_cpsc_bench(points, &rect);
	_circle_bench(&rect);
	_circle_fill_bench(&rect);



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
#undef POINTS_NUM
#undef MAX_X
#undef MAX_Y
#endif
}
