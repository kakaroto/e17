#include "enesim_bench_common.h"

void shape_bench(Enesim_Renderer *r, Enesim_Surface *dst, const char *bname,
		const char *name)
{
	Enesim_Surface *src = NULL;
	Enesim_Renderer *fill;
	char rbname[256];
	char rname[256];

	surfaces_create(&src, opt_fmt, NULL, 0, NULL, 0);
	fill = enesim_renderer_surface_new();
	enesim_renderer_surface_src_set(fill, src);
	enesim_renderer_surface_w_set(fill, opt_width);
	enesim_renderer_surface_h_set(fill, opt_height);
	enesim_renderer_shape_draw_mode_set(r, ENESIM_SHAPE_DRAW_MODE_FILL);
	enesim_renderer_shape_fill_renderer_set(r, fill);
	snprintf(rbname, 256, "%s fill", bname);
	snprintf(rname, 256, "%s_fill", name);
	renderer_run(r, dst, rbname, rname);
	/* stroke only */
	/* fill only */
	/* stroke and fill */
	enesim_renderer_shape_draw_mode_set(r, ENESIM_SHAPE_DRAW_MODE_STROKE_FILL);
	enesim_renderer_shape_outline_color_set(r, 0xffff0000);
	enesim_renderer_shape_outline_weight_set(r, 3);
	snprintf(rbname, 256, "%s stroke", bname);
	snprintf(rname, 256, "%s_stroke", name);
	renderer_run(r, dst, rbname, rname);
}
