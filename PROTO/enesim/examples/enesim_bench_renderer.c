#include "enesim_bench_common.h"
#define DIRECT 1
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
#if DIRECT
	Enesim_Surface_Data tdata;
	Enesim_Drawer_Span sp;

	tdata.plane0 = alloca(sizeof(uint32_t) * opt_width);
	tdata.format = opt_fmt;
	sp = enesim_drawer_span_pixel_get(opt_rop, opt_fmt, opt_fmt);
#endif
	enesim_surface_data_get(dst, &ddata);
	enesim_surface_data_get(src, &sdata);
	rend = enesim_renderer_func_get(r, opt_fmt);
	*start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_surface_data_get(dst, &ddata);
		for (i = 0; i < opt_height; i++)
		{
			int offset;
			enesim_surface_data_get(src, &sdata);
			enesim_renderer_repeater_src_offset(r, i, &offset);
			//enesim_surface_data_increment(&sdata, offset);
			sdata.plane0 += offset;
			enesim_renderer_repeater_src_set(r, &sdata);
#if DIRECT
			rend(r, 0, i, opt_width, &tdata);
			sp(&ddata, opt_width, &tdata, NULL, NULL);
#else
			rend(r, 0, i, opt_width, &ddata);
#endif
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

	enesim_surface_data_get(dst, &ddata);
	enesim_surface_data_get(src, &sdata);
	enesim_surface_data_get(msk, &mdata);
	enesim_renderer_repeater_mask_set(r, &mdata);
	rend = enesim_renderer_func_get(r, opt_fmt);

	*start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_surface_data_get(dst, &ddata);
		for (i = 0; i < opt_height; i++)
		{
			int offset;

			enesim_surface_data_get(src, &sdata);
			enesim_renderer_repeater_src_offset(r, i, &offset);
			//enesim_surface_data_increment(&sdata, offset);
			sdata.plane0 += offset;

			enesim_surface_data_get(msk, &mdata);
			enesim_renderer_repeater_mask_offset(r, i, &offset);
			//enesim_surface_data_increment(&mdata, offset);
			mdata.plane0 += offset;

			enesim_renderer_repeater_src_set(r, &sdata);
			enesim_renderer_repeater_mask_set(r, &mdata);

			rend(r, 0, i, opt_width, &ddata);
			//enesim_surface_data_increment(&ddata, opt_width);
			ddata.plane0 += opt_width;
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
	enesim_renderer_repeater_src_size_set(r, 25, 25);
	enesim_renderer_repeater_mask_area_set(r, 0, 0, 25, 25);
	enesim_renderer_repeater_mask_size_set(r, 25, 25);

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
#if DIRECT
	Enesim_Surface_Data tdata;
	Enesim_Drawer_Span sp;

	tdata.plane0 = alloca(sizeof(uint32_t) * opt_width);
	tdata.format = opt_fmt;
	sp = enesim_drawer_span_pixel_get(opt_rop, opt_fmt, opt_fmt);
#else
	Enesim_Drawer_Point pt;
	Enesim_Surface_Pixel p;

	enesim_surface_pixel_components_from(&p, opt_fmt, 0xaa, 0xff, 0xaa, 0xff);
	pt = enesim_drawer_point_color_get(opt_rop, opt_fmt, &p);
#endif

	r = enesim_renderer_scaler_new();
	/* setup the dest */
	src = test_pattern(81); // solid pattern
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	enesim_renderer_scaler_dst_area_set(r, 0, 0, opt_width, opt_height);
	enesim_renderer_scaler_src_area_set(r, 0, 0, 81, 81);
	enesim_renderer_scaler_src_size_set(r, 81, 81);
	rend = enesim_renderer_func_get(r, opt_fmt);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_surface_data_get(dst, &ddata);
		for (i = 0; i < opt_height; i++)
		{
			int offset;

			enesim_surface_data_get(src, &sdata);
			enesim_renderer_scaler_src_offset(r, i, &offset);
			enesim_surface_data_increment(&sdata, offset);
			//sdata.plane0 += offset;
			enesim_renderer_scaler_src_set(r, &sdata);
#if DIRECT
			rend(r, 0, i, opt_width, &tdata);
			sp(&ddata, opt_width, &tdata, NULL, NULL);
#else

			rend(r, 0, i, opt_width, &ddata);
#endif
			enesim_surface_data_increment(&ddata, opt_width);
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
#if DIRECT
	Enesim_Surface_Data tscaler, trepeater;
	Enesim_Drawer_Span sp;

	tscaler.plane0 = alloca(sizeof(uint32_t) * 128);
	tscaler.format = opt_fmt;
	trepeater.plane0 = alloca(sizeof(uint32_t) * opt_width);
	trepeater.format = opt_fmt;
	sp = enesim_drawer_span_pixel_get(opt_rop, opt_fmt, opt_fmt);
#else
	Enesim_Drawer_Point pt;
	Enesim_Surface_Pixel p;

	enesim_surface_pixel_components_from(&p, opt_fmt, 0xaa, 0xff, 0xaa, 0xff);
	pt = enesim_drawer_point_color_get(opt_rop, opt_fmt, &p);
#endif

	scaler = enesim_renderer_scaler_new();
	/* setup the dest */
	src = test_pattern(81); // solid pattern
	dst = enesim_surface_new(opt_fmt, opt_width, opt_height);
	enesim_renderer_scaler_dst_area_set(scaler, 0, 0, 128, 128);
	enesim_renderer_scaler_src_area_set(scaler, 0, 0, 81, 81);
	enesim_renderer_scaler_src_size_set(scaler, 81, 81);
	repeater = enesim_renderer_repeater_new();
	enesim_renderer_repeater_dst_area_set(repeater, 0, 0, opt_width, opt_height);
	enesim_renderer_repeater_src_area_set(repeater, 0, 0, 128, 128);
	enesim_renderer_repeater_src_size_set(repeater, 128, 128);
	enesim_renderer_repeater_mode_set(repeater, ENESIM_SURFACE_REPEAT_Y | ENESIM_SURFACE_REPEAT_X);

	#if !DIRECT
		enesim_renderer_scaler_pt_st(scaler, pt);
	#endif
	scaler_func = enesim_renderer_func_get(scaler, opt_fmt);
	repeater_func = enesim_renderer_func_get(repeater, opt_fmt);

	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		enesim_surface_data_get(dst, &ddata);
		for (i = 0; i < opt_height; i++)
		{
			int ysrc, yscaler;
#if 1
			//printf("Ydst at %d ", i);
			if (!enesim_renderer_repeater_src_y(repeater, i, &yscaler))
			{
				continue;
			}
			//printf("repeater ysrc at %d ", yscaler);
			if (!enesim_renderer_scaler_src_y(scaler, yscaler, &ysrc))
			{
				//printf("quitting the scaler\n");
				continue;
			}
			//printf("scaler ysrc at %d\n", ysrc);
			enesim_surface_data_get(src, &sdata);
			enesim_surface_data_increment(&sdata, ysrc * 81);
			enesim_renderer_scaler_src_set(scaler, &sdata);
#if DIRECT
			scaler_func(scaler, 0, i, 128, &tscaler);
			enesim_renderer_repeater_src_set(repeater, &tscaler);
			repeater_func(repeater, 0, i, opt_width, &trepeater);
			sp(&ddata, opt_width, &trepeater, NULL, NULL);
#else
			scaler_func(scaler, 0, i, opt_width, &ddata);
#endif
			enesim_surface_data_increment(&ddata, opt_width);
#endif
		}
	}
	end = get_time();
	printf("Scaler Repeater Renderer [%3.3f sec]\n", end - start);
	test_finish("renderer_scaler_repeater", opt_rop, dst, NULL, NULL, NULL);
}


void renderer_bench(void)
{
	printf("******************\n");
	printf("* Renderer Bench *\n");
	printf("******************\n");
	_repeater_bench();
	_scaler_bench();
	_scaler_repeater_bench();
}
