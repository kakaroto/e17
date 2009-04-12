#include "enesim_bench_common.h"

static void scaler_1d(void)
{
	Enesim_Surface *src = NULL;
	Enesim_Surface *dst= NULL;
	Enesim_Operator op;
	uint32_t *s;
	uint32_t *d;
	int t;
	double start, end;

	enesim_scaler_1d_op_get(&op, opt_cpu, opt_fmt, opt_fmt);
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, 0);

	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		int y = opt_height;

		s = enesim_surface_data_get(src);
		d = enesim_surface_data_get(dst);
		while (y--)
		{
			enesim_operator_scaler_1d(&op, s, opt_width, 0,
					opt_width, opt_width * 2, d);
			s += opt_width;
			d += opt_width;
		}
	}
	end = get_time();
	printf("Scaler 1D         [%3.3f sec]\n", end - start);
	test_finish("scaler", ENESIM_FILL, dst, src, NULL, NULL);
}

void scaler_bench(void)
{

	printf("****************\n");
	printf("* Scaler Bench *\n");
	printf("****************\n");
	scaler_1d();
}

