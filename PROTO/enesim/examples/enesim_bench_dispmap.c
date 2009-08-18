#include "enesim_bench_common.h"
/******************************************************************************
 *                         Raddist benchmark functions                        *
 ******************************************************************************/
static const char * quality_name_get(Enesim_Quality q)
{
	switch (q)
	{
		case ENESIM_FAST:
		return "fast";

		case ENESIM_GOOD:
		return "good";

		default:
		return NULL;
	}
}

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

static void dispmap_1d_run(Enesim_Quality q)
{
	Enesim_Surface *src = NULL;
	Enesim_Surface *dst= NULL;
	Enesim_Surface *map= NULL;
	Enesim_Operator op;
	uint32_t *s;
	uint32_t *d;
	uint32_t *m;
	int t;
	double start, end;
	char name[256];

	snprintf(name, 256, "dispmap_%s", quality_name_get(q));
	if (!enesim_dispmap_1d_op_get(&op, opt_cpu, opt_fmt, q, opt_fmt))
	{
		printf("%s         [NOT BUILT]\n", name);
		return;
	}
#if 1
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, &map, opt_fmt);
#else
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, 0);
	dispmap_map_create(&map);
#endif

	s = enesim_surface_data_get(src);
	m = enesim_surface_data_get(map);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		int h = opt_height;
		int y = 0;

		d = enesim_surface_data_get(dst);
		while (h--)
		{
			enesim_operator_dispmap_1d(&op,
					s, opt_width, opt_width, opt_height,
					64, m,
					0, y, opt_width, d);
			y++;
			d += opt_width;
			m += opt_width;
		}
	}
	end = get_time();
	printf("%s         [%3.3f sec]\n", name, end - start);
	test_finish(name, ENESIM_FILL, dst, src, NULL, NULL);
	enesim_surface_delete(map);
}

void dispmap_bench(void)
{
	Enesim_Quality q;

	printf("*********************\n");
	printf("*   Dispmap Bench   *\n");
	printf("*********************\n");

	//for (q = 0; q < ENESIM_QUALITIES; q++)
		dispmap_1d_run(ENESIM_FAST);
}
