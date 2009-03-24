#include "enesim_bench_common.h"
/******************************************************************************
 *                        Renderer benchmark functions                        *
 ******************************************************************************/
static void _repeater_run_normal(Enesim_Renderer *r, double *start, double *end,
		Enesim_Surface *dst, Enesim_Surface *src)
{
	Enesim_Surface_Data ddata, sdata;
	Enesim_Renderer_Span rend;
	int t;
	int i;
	Enesim_Surface_Data tdata;
	Enesim_Drawer_Span sp;
	int w;

	tdata.plane0 = alloca(sizeof(uint32_t) * opt_width);
	tdata.format = opt_fmt;
	sp = enesim_drawer_span_pixel_get(opt_rop, opt_fmt, opt_fmt);

	enesim_surface_data_get(dst, &ddata);
	enesim_surface_data_get(src, &sdata);
	enesim_surface_size_get(src, &w, NULL);
	rend = enesim_renderer_func_get(r, opt_fmt);
	*start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_surface_data_get(dst, &ddata);
		for (i = 0; i < opt_height; i++)
		{
			int ysrc;

			enesim_surface_data_get(src, &sdata);
			if (!enesim_renderer_repeater_src_y(r, i, &ysrc))
			{
				continue;
			}
			enesim_surface_data_increment(&sdata, ysrc * w);
			enesim_renderer_repeater_src_set(r, &sdata);
			rend(r, 0, i, opt_width, &tdata);
			sp(&ddata, opt_width, &tdata, NULL, NULL);
			//enesim_surface_data_increment(&ddata, opt_width);
			ddata.plane0 += opt_width;
		}
	}
	*end = get_time();
}

static void _repeater_run_mask(Enesim_Renderer *r, double *start, double *end,
		Enesim_Surface *dst, Enesim_Surface *src, Enesim_Surface *msk)
{
	Enesim_Surface_Data ddata, sdata, mdata;
	Enesim_Renderer_Span rend;
	int t;
	int i;
	int sw;
	int mw;

	enesim_surface_data_get(dst, &ddata);

	enesim_surface_data_get(src, &sdata);
	enesim_surface_size_get(src, &sw, NULL);

	enesim_surface_data_get(msk, &mdata);
	enesim_surface_size_get(msk, &mw, NULL);

	enesim_renderer_repeater_mask_set(r, &mdata);
	rend = enesim_renderer_func_get(r, opt_fmt);

	*start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_surface_data_get(dst, &ddata);
		for (i = 0; i < opt_height; i++)
		{
			int ysrc;

			enesim_surface_data_get(src, &sdata);
			if (!enesim_renderer_repeater_src_y(r, i, &ysrc))
			{
				continue;
			}
			enesim_surface_data_increment(&sdata, ysrc * sw);
			enesim_surface_data_get(msk, &mdata);
			if (!enesim_renderer_repeater_mask_y(r, i, &ysrc))
			{
				continue;
			}
			enesim_surface_data_increment(&mdata, ysrc * mw);
			enesim_renderer_repeater_src_set(r, &sdata);
			enesim_renderer_repeater_mask_set(r, &mdata);
			rend(r, 0, i, opt_width, &ddata);
			enesim_surface_data_increment(&ddata, opt_width);
		}
	}
	*end = get_time();
}

static void _repeater_bench()
{
	Enesim_Renderer *r;
	Enesim_Surface *src;
	Enesim_Surface *dst;
	Enesim_Surface *msk;
	double start, end;

	/* setup the src */
	src = test_pattern(25); // solid pattern
	msk = test_pattern2(25); // transparent pattern
	/* setup the renderer with common parameters */
	r = enesim_renderer_repeater_new();
	enesim_renderer_repeater_dst_area_set(r, 0, 0, opt_width, opt_height);
	enesim_renderer_repeater_src_area_set(r, 0, 0, 25, 25);
	enesim_renderer_repeater_mask_area_set(r, 0, 0, 25, 25);

	printf("Repeater Renderer:\n");
	/* no repeat, no masked */
	enesim_renderer_repeater_mode_set(r, ENESIM_SURFACE_REPEAT_NONE);
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	_repeater_run_normal(r, &start, &end, dst, src);
	printf("\tNo Repeat X, No Repeat Y, No Masked [%3.3f sec]\n", end - start);
	test_finish("renderer_repeater", opt_rop, dst, NULL, NULL, NULL);
	enesim_surface_delete(dst);

	/* repeat x, y, no masked */
	enesim_renderer_repeater_mode_set(r, ENESIM_SURFACE_REPEAT_Y | ENESIM_SURFACE_REPEAT_X);
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	_repeater_run_normal(r, &start, &end, dst, src);
	printf("\tRepeat X, Repeat Y, No Masked [%3.3f sec]\n", end - start);
	test_finish("renderer_repeater_rx_ry", opt_rop, dst, NULL, NULL, NULL);
	enesim_surface_delete(dst);

	/* repeat x and y, masked */
	enesim_renderer_repeater_mode_set(r, ENESIM_SURFACE_REPEAT_Y | ENESIM_SURFACE_REPEAT_X);
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	_repeater_run_mask(r, &start, &end, dst, src, msk);
	printf("\tRepeat X, Repeat Y, Masked [%3.3f sec]\n", end - start);
	test_finish("renderer_surface_rx_ry_masked", opt_rop, dst, NULL, NULL, NULL);
	enesim_surface_delete(dst);

	enesim_renderer_delete(r);
}

static void _scaler_bench(void)
{
	Enesim_Renderer *r;
	Enesim_Surface *src;
	Enesim_Surface *dst;
	Enesim_Surface *msk;
	double start, end;

	Enesim_Surface_Data ddata, sdata, mdata;
	Enesim_Renderer_Span rend;
	int t;
	int i;
	Enesim_Surface_Data tdata;
	Enesim_Drawer_Span sp;

	tdata.plane0 = alloca(sizeof(uint32_t) * opt_width);
	tdata.format = opt_fmt;
	sp = enesim_drawer_span_pixel_get(opt_rop, opt_fmt, opt_fmt);
	r = enesim_renderer_scaler_new();
	/* setup the dest */
	src = test_pattern(81); // solid pattern
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	enesim_renderer_scaler_dst_area_set(r, 0, 0, opt_width, opt_height);
	enesim_renderer_scaler_src_area_set(r, 0, 0, 81, 81);
	rend = enesim_renderer_func_get(r, opt_fmt);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		for (i = 0; i < opt_height; i++)
		{
			int offset;
			int yscaler;

			enesim_surface_data_get(src, &sdata);
			if (!enesim_renderer_scaler_src_y(r, i, &yscaler))
			{
				continue;
			}
			enesim_surface_data_increment(&sdata, (yscaler * 81) + 0);
			enesim_renderer_scaler_src_set(r, &sdata);
			if (!rend(r, 0, i, opt_width, &tdata))
				continue;
			enesim_surface_data_get(dst, &ddata);
			enesim_surface_data_increment(&ddata, opt_width * i);
			sp(&ddata, opt_width, &tdata, NULL, NULL);
		}
	}
	end = get_time();
	printf("Scaler Renderer [%3.3f sec]\n", end - start);
	test_finish("renderer_scaler_upx_upy", opt_rop, dst, NULL, NULL, NULL);
}

static void _scaler_repeater_bench(void)
{
	Enesim_Renderer *scaler, *repeater;
	Enesim_Surface *src;
	Enesim_Surface *dst;
	Enesim_Surface *msk;
	double start, end;

	Enesim_Surface_Data ddata, sdata, mdata;
	Enesim_Renderer_Span scaler_func, repeater_func;
	int t;
	int i;
	Enesim_Surface_Data tscaler, trepeater;
	Enesim_Drawer_Span sp;

	tscaler.plane0 = alloca(sizeof(uint32_t) * 128);
	tscaler.format = opt_fmt;
	trepeater.plane0 = alloca(sizeof(uint32_t) * opt_width);
	trepeater.format = opt_fmt;
	sp = enesim_drawer_span_pixel_get(opt_rop, opt_fmt, opt_fmt);
	scaler = enesim_renderer_scaler_new();
	/* setup the dest */
	src = test_pattern(81); // solid pattern
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	enesim_renderer_scaler_dst_area_set(scaler, 0, 0, 128, 128);
	enesim_renderer_scaler_src_area_set(scaler, 0, 0, 81, 81);
	repeater = enesim_renderer_repeater_new();
	enesim_renderer_repeater_dst_area_set(repeater, 0, 0, opt_width, opt_height);
	enesim_renderer_repeater_src_area_set(repeater, 0, 0, 128, 128);
	enesim_renderer_repeater_mode_set(repeater, ENESIM_SURFACE_REPEAT_Y | ENESIM_SURFACE_REPEAT_X);

	scaler_func = enesim_renderer_func_get(scaler, opt_fmt);
	repeater_func = enesim_renderer_func_get(repeater, opt_fmt);

	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		for (i = 0; i < opt_height; i++)
		{
			int ysrc, yscaler;

			if (!enesim_renderer_repeater_src_y(repeater, i, &yscaler))
			{

				continue;
			}
			if (!enesim_renderer_scaler_src_y(scaler, yscaler, &ysrc))
			{
				continue;
			}
			enesim_surface_data_get(src, &sdata);
			enesim_surface_data_get(dst, &ddata);
			enesim_surface_data_increment(&ddata, opt_width * i);
			enesim_surface_data_increment(&sdata, ysrc * 81);
			enesim_renderer_scaler_src_set(scaler, &sdata);
			if (!scaler_func(scaler, 0, ysrc, 128, &tscaler))
				continue;
			enesim_renderer_repeater_src_set(repeater, &tscaler);
			if (!repeater_func(repeater, 0, i, opt_width, &trepeater))
				continue;
			sp(&ddata, opt_width, &trepeater, NULL, NULL);


		}
	}
	end = get_time();
	printf("Scaler Repeater Renderer [%3.3f sec]\n", end - start);
	test_finish("renderer_scaler_repeater", opt_rop, dst, NULL, NULL, NULL);
}

static void _transformer_bench(void)
{
	Enesim_Renderer *transformer;
	Enesim_Surface *src;
	Enesim_Surface *dst;
	Enesim_Surface *msk;
	double start, end;

	Enesim_Surface_Data ddata, sdata, mdata;
	Enesim_Renderer_Span func;
	int t;
	int i;
	Enesim_Surface_Data tmp;
	Enesim_Drawer_Span sp;
	Enesim_Matrix m1, m2;

	tmp.plane0 = alloca(sizeof(uint32_t) * opt_width);
	tmp.format = opt_fmt;
	sp = enesim_drawer_span_pixel_get(opt_rop, opt_fmt, opt_fmt);
	transformer = enesim_renderer_transformer_new();
	/* setup the dest */
	src = test_pattern(121); // solid pattern
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	/* setup the transformer */
	enesim_matrix_translate(&m2, opt_width / 2, opt_height / 2);
	enesim_matrix_rotate(&m1, 1.15f);
	enesim_matrix_compose(&m2, &m1, &m1);
	enesim_matrix_translate(&m2, - opt_width / 2, - opt_height / 2);
	enesim_matrix_compose(&m1, &m2, &m1);
	enesim_renderer_transformer_matrix_set(transformer, &m1);
	enesim_renderer_transformer_src_set(transformer, src);
	func = enesim_renderer_func_get(transformer, opt_fmt);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		for (i = 0; i < opt_height; i++)
		{
			enesim_surface_data_get(dst, &ddata);
			enesim_surface_data_increment(&ddata, opt_width * i);
			func(transformer, 0, i, opt_width, &tmp);
			sp(&ddata, opt_width, &tmp, NULL, NULL);
		}
	}
	end = get_time();
	printf("Transformer Renderer [%3.3f sec]\n", end - start);
	test_finish("renderer_transformer", opt_rop, dst, NULL, NULL, NULL);
}

void renderer_bench(void)
{
	printf("******************\n");
	printf("* Renderer Bench *\n");
	printf("******************\n");
	_repeater_bench();
	_scaler_bench();
	_scaler_repeater_bench();
	_transformer_bench();
}
