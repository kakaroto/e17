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

static void raddist_1d_run(Enesim_Quality q)
{
	Enesim_Surface *src = NULL;
	Enesim_Surface *dst= NULL;
	Enesim_Operator op;
	uint32_t *s;
	uint32_t *d;
	int t;
	double start, end;
	char name[256];

	snprintf(name, 256, "raddist_%s", quality_name_get(q));
	if (!enesim_raddist_1d_op_get(&op, opt_cpu, opt_fmt, q, opt_fmt))
	{
		printf("%s         [NOT BUILT]\n", name);
		return;
	}
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, 0);

	s = enesim_surface_data_get(src);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		int h = opt_height;
		int y = 0;

		d = enesim_surface_data_get(dst);
		while (h--)
		{
			enesim_operator_raddist_1d(&op,
					s, opt_width, opt_width, opt_height,
					100, opt_height / 2, hypot(opt_width, opt_height), 0,
					//0, 0, hypot(opt_width/2.0, opt_height/2.0), 0,
					//0, 0, opt_width / 2, 1,
					0, y, opt_width, d);
			y++;
			d += opt_width;
		}
	}
	end = get_time();
	printf("%s         [%3.3f sec]\n", name, end - start);
	test_finish(name, ENESIM_FILL, dst, src, NULL, NULL);
}
/*
 * TODO transform with a mask/color and without
 */
void raddist_bench(void)
{
	Enesim_Quality q;

	printf("*********************\n");
	printf("* Raddist Bench *\n");
	printf("*********************\n");

	//for (q = 0; q < ENESIM_QUALITIES; q++)
		raddist_1d_run(ENESIM_FAST);
}
