#include "enesim_bench_common.h"

static void scaler(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *src = NULL;
	Enesim_Surface *dst= NULL;

	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, 0);
	r = enesim_renderer_surface_new();
	enesim_renderer_surface_src_set(r, src);
	enesim_renderer_surface_h_set(r, opt_height * 2);
	enesim_renderer_surface_w_set(r, opt_width * 2);
	enesim_renderer_surface_x_set(r, 0);
	enesim_renderer_surface_y_set(r, 0);
	renderer_run(r, dst, "Scaler", "scaler");
}

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
	scaler();
	//scaler_1d();
}

